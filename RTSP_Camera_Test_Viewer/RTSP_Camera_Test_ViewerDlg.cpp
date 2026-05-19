
// RTSP_Camera_Test_ViewerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "RTSP_Camera_Test_Viewer.h"
#include "RTSP_Camera_Test_ViewerDlg.h"
#include "afxdialogex.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRTSPCameraTestViewerDlg 대화 상자


CRTSPCameraTestViewerDlg::CRTSPCameraTestViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RTSP_CAMERA_TEST_VIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CRTSPCameraTestViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_CAMERA_VIEW, m_staticCameraView);

	DDX_Control(pDX, IDC_EDIT_RTSP_URL, m_editsRtspUrl);
	DDX_Control(pDX, IDC_EDIT_CAMERA_ID, m_editCameraId);
	DDX_Control(pDX, IDC_EDIT_CAMERA_PW, m_editCameraPw);

	DDX_Control(pDX, IDC_BTN_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BTN_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_CHECK_CROSS_LINE, m_checkCrossLine);
}


BEGIN_MESSAGE_MAP(CRTSPCameraTestViewerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_BTN_CONNECT, &CRTSPCameraTestViewerDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CRTSPCameraTestViewerDlg::OnBnClickedBtnDisconnect)
	ON_BN_CLICKED(IDC_CHECK_CROSS_LINE, &CRTSPCameraTestViewerDlg::OnBnClickedCheckCrossLine)
END_MESSAGE_MAP()


// CRTSPCameraTestViewerDlg 메시지 처리기

BOOL CRTSPCameraTestViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// OpenCV 로그 출력 완전 비활성화
	cv::utils::logging::setLogLevel(
		cv::utils::logging::LOG_LEVEL_SILENT);

	// FFmpeg 로그 레벨 최소화
	_putenv_s("OPENCV_FFMPEG_DEBUG", "0");
	_putenv_s("OPENCV_FFMPEG_LOGLEVEL", "quiet");

	// 콘솔 한글 출력 코드페이지 설정
	SetConsoleOutputCP(949);
	SetConsoleCP(949);

	// 콘솔 창 생성
	AllocConsole();

	// [stdout] / [stderr] / [stdin] 연결
	FILE* fpOut = nullptr;
	FILE* fpErr = nullptr;
	FILE* fpIn = nullptr;

	// [stdout] → 콘솔 출력
	freopen_s(&fpOut, "CONOUT$", "w", stdout);

	// [stderr] → 버림 (FFmpeg/OpenCV Warning 숨김)
	freopen_s(&fpErr, "NUL", "w", stderr);

	// [stdin] → 콘솔 입력
	freopen_s(&fpIn, "CONIN$", "r", stdin);

	// 콘솔 제목 설정
	SetConsoleTitle(_T("Toruss_V3 Debug Console"));

	std::cout << "=== [RTSP CAMERA TEST VIEWER START] ===" << std::endl;

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 영상 출력 View와 Picture Control 연결
	m_cameraView.Attach(&m_staticCameraView);

	// 폰트 생성 (Segoe UI, 10pt)
	m_fontUI.CreatePointFont(100, _T("Segoe UI"));

	// [Edit] 폰트 적용
	m_editsRtspUrl.SetFont(&m_fontUI);
	m_editCameraId.SetFont(&m_fontUI);
	m_editCameraPw.SetFont(&m_fontUI);

	// [CheckBox] 폰트 적용
	m_checkCrossLine.SetFont(&m_fontUI);

	m_editsRtspUrl.SendMessage(
		EM_SETMARGINS,
		EC_LEFTMARGIN,
		MAKELPARAM(5, 0));

	m_editCameraId.SendMessage(
		EM_SETMARGINS,
		EC_LEFTMARGIN,
		MAKELPARAM(5, 0));

	m_editCameraPw.SendMessage(
		EM_SETMARGINS,
		EC_LEFTMARGIN,
		MAKELPARAM(5, 0));

	// [기본 입력값] 설정 1 (RTSP 주소)
	m_editsRtspUrl.SetWindowText(_T("rtsp://192.168.1.129:554/trackID=1"));

	// [기본 입력값] 설정 2 (기본 ID)
	m_editCameraId.SetWindowText(_T("admin"));

	// [기본 입력값] 설정 3 (기본 PW)
	m_editCameraPw.SetWindowText(_T("rmffhqjf1!"));

	// [십자선 표시] 디폴트 [ON]
	m_checkCrossLine.SetCheck(BST_CHECKED);
	m_showCrossLine = true;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
// 아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CRTSPCameraTestViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
// 이 함수를 호출합니다.
HCURSOR CRTSPCameraTestViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// [Dialog 종료 시] 리소스 정리 함수
void CRTSPCameraTestViewerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_isPlaying = false; // 재생 상태 OFF

	// RTSP 디코더 종료
	m_rtspDecoder.Close();

	// 2. [Video] [Thread join] 처리 함수
	JoinVidThread();
}


// [RTSP] 영상 출력 Thread 함수
void CRTSPCameraTestViewerDlg::PlayRTSPVideo(std::string rtspUrl)
{
	std::cout << "[RTSP CONNECT TRY]" << std::endl;

	// 전달받은 RTSP 주소로 디코더 Open 시도
	m_rtspDecoder.Open(rtspUrl);

	// 연결 대기 // 최대 5초 대기 필요
	for (int i = 0; i < 50; i++)
	{
		// 디코더가 정상적으로 열렸으면 대기 종료
		if (m_rtspDecoder.IsOpened())
		{
			break;
		}
		// 0.1초 대기
		Sleep(100);
	}

	// 5초 대기 후에도 연결되지 않은 경우
	if (!m_rtspDecoder.IsOpened())
	{
		std::cout << "[RTSP CONNECT FAILED]" << std::endl;

		std::cout << " " << std::endl;

		std::cout << "[CHECK] URL / ID / Password / Camera Auth / Network Status" << std::endl;

		std::cout << " " << std::endl;

		m_isPlaying = false; // 재생 상태 OFF

		// 실패한 디코더 정리!
		m_rtspDecoder.Close();

		// 연결 실패 메시지 출력
		AfxMessageBox(_T("RTSP 연결 실패\nURL, ID/PW, IP, Port 또는 네트워크 상태를 확인해주세요."));

		return;
	}

	std::cout << "[RTSP CONNECT SUCCESS]" << std::endl;

	std::cout << "=======================================" << std::endl;

	VideoFrame frame; // 최신 프레임을 저장할 변수

	// 재생 상태가 true인 동안 반복
	while (m_isPlaying)
	{
		// 디코더가 열려 있고 최신 프레임을 가져올 수 있으면
		if (m_rtspDecoder.IsOpened() &&
			m_rtspDecoder.GetLatestFrame(frame))
		{
			// Picture Control에 프레임 출력
			m_cameraView.DrawFrame(
				frame.bgr,
				m_showCrossLine,
				m_crossLineColor);
		}
		// CPU 과부하 방지용 짧은 대기
		Sleep(1);
	}
	// 반복문 종료 후 RTSP 디코더 종료
	m_rtspDecoder.Close();
}


// [RTSP] [Connect] 버튼 클릭 이벤트 함수
void CRTSPCameraTestViewerDlg::OnBnClickedBtnConnect()
{
	std::cout << "=======================================" << std::endl;

	std::cout << "[CONNECT BUTTON CLICK]" << std::endl;

	std::cout << " " << std::endl;

	if (m_isPlaying)
	{
		std::cout << "[VIDEO] Already Playing." << std::endl;
		return;
	}

	ResetRTSPView(); // 3. [RTSP] 내부상태 초기화 함수

	CString strUrl;
	CString strId;
	CString strPw;

	m_editsRtspUrl.GetWindowText(strUrl.Trim()); // [RTSP URL] 입력값 가져오기

	// 1-1. 빈 값 검사 (RTSP 주소)
	if (strUrl.IsEmpty() || strUrl == _T("rtsp://"))
	{
		AfxMessageBox(_T("RTSP URL을 입력해주세요."));
		return;
	}

	// 1-2. rtsp:// 시작 여부 검사
	if (strUrl.Left(7).CompareNoCase(_T("rtsp://")) != 0)
	{
		AfxMessageBox(_T("RTSP URL 형식이 올바르지 않습니다.\n다시 입력해주세요."));
		return;
	}

	// 1-3. 최소 주소 형식 검사
	if (strUrl.Find(_T("://")) < 0 || strUrl.Find(_T(".")) < 0)
	{
		AfxMessageBox(_T("RTSP 주소를 다시 확인해주세요."));
		return;
	}

	m_editCameraId.GetWindowText(strId.Trim()); // [ID] 입력값 가져오기

	m_editCameraPw.GetWindowText(strPw.Trim()); // [PW] 입력값 가져오기

	// 2. 빈 값 검사 (ID)
	if (strId.IsEmpty())
	{
		AfxMessageBox(_T("ID를 입력해주세요."));
		return;
	}

	// 3. 빈 값 검사 (PW)
	if (strPw.IsEmpty())
	{
		AfxMessageBox(_T("PW를 입력해주세요."));
		return;
	}

	CString strFinalRtspUrl; // 최종 RTSP 주소 조합

	// 4. rtsp:// 뒤의 실제 주소 부분만 추출
	CString strRtspBody = strUrl;

	if (strRtspBody.Left(7).CompareNoCase(_T("rtsp://")) == 0)
	{
		strRtspBody = strRtspBody.Mid(7);
	}

	// 5. [RTSP URL] 안에 이미 [ID / PW] 정보가 포함되어 있는지 확인
	// Ex)
	// service:Xhddlf1!@192.168.0.107:554/rtsp_tunnel
	//                 ↑
	//                 '@' 위치 찾기
	int atPos = strRtspBody.Find(_T("@"));

	// '@' 문자가 존재하는 경우,
	// 이미 [RTSP URL] 안에 [ID / PW]가 포함된 상태
	if (atPos >= 0)
	{
		// '@' 뒤쪽 문자열만 잘라서 사용
		// Ex)
		// service:Xhddlf1!@192.168.0.107:554/rtsp_tunnel
		// ↓
		// 192.168.0.107:554/rtsp_tunnel
		// 이유: 기존 [RTSP URL] 안의 [ID / PW]는 제거하고,
		// [ID / PW] 입력칸 값으로 다시 조합하기 위함.
		strRtspBody = strRtspBody.Mid(atPos + 1);
	}

	// 6. [ID / PW] 입력칸 기준으로, 최종 [RTSP] 주소 조합.
	strFinalRtspUrl.Format(
		_T("rtsp://%s:%s@%s"),
		strId.GetString(),
		strPw.GetString(),
		strRtspBody.GetString());

	// CString -> std::string 변환
	CStringA rtspA(strFinalRtspUrl);
	std::string rtspUrl = rtspA.GetString();

	// 6. [RTSP] 입력 주소 로그 출력
	std::cout << "[RTSP URL INPUT] "
		<< rtspUrl
		<< std::endl;

	std::cout << " " << std::endl;

	JoinVidThread(); // 2. [Video] [Thread join] 처리 함수

	m_isPlaying = true; // 영상 재생 상태 ON

	std::cout << "[VIDEO THREAD] Start..." << std::endl;

	std::cout << " " << std::endl;

	// RTSP 영상 출력 Thread 시작
	m_videoThread = std::thread(
		&CRTSPCameraTestViewerDlg::PlayRTSPVideo,
		this,
		rtspUrl);
}


// [RTSP] [DisConnect] 버튼 클릭 이벤트 함수
void CRTSPCameraTestViewerDlg::OnBnClickedBtnDisconnect()
{
	std::cout << "=======================================" << std::endl;

	std::cout << "[DISCONNECT BUTTON CLICK]" << std::endl;

	std::cout << " " << std::endl;

	// 이미 영상 종료 상태이면 중복 Disconnect 방지
	if (!m_isPlaying)
	{
		std::cout << "[VIDEO] Already Stopped." << std::endl;

		JoinVidThread(); // 2. [Video] [Thread join] 처리 함수

		return;
	}
	StopRTSPVideo(); // 1. [RTSP] 영상 종료 함수

	JoinVidThread(); // 2. [Video] [Thread join] 처리 함수

	ClearRTSPView(); // 3. [RTSP] 영상 초기화 & 검은 화면 출력 함수

	std::cout << "[CROSS LINE] END" << std::endl;

	std::cout << " " << std::endl;

	std::cout << "[DISCONNECT COMPLETE]" << std::endl;
}


// 1. [RTSP] 영상 종료 함수
void CRTSPCameraTestViewerDlg::StopRTSPVideo()
{
	m_isPlaying = false; // 재생 상태 OFF

	// RTSP 디코더 종료
	// av_read_frame 같은 내부 대기 상태를 빠져나오게 하는 역할도 함
	m_rtspDecoder.Close();
}


// 2. [Video] [Thread join] 처리 함수
void CRTSPCameraTestViewerDlg::JoinVidThread()
{
	// 종료된 thread 객체만 남아있을 수 있으므로 join 처리!
	if (m_videoThread.joinable())
	{
		std::cout << "[THREAD] Join Start..." << std::endl;

		m_videoThread.join();

		std::cout << "[THREAD] Join Complete" << std::endl;

		std::cout << " " << std::endl;
	}

}

// 3. [RTSP] 내부상태 초기화 함수
void CRTSPCameraTestViewerDlg::ResetRTSPView()
{
	std::cout << "[RTSP] Previous State Reset Start..." << std::endl;

	std::cout << " " << std::endl;

	m_isPlaying = false; // 재생 상태 OFF

	// 이전 디코더 정리
	m_rtspDecoder.Close();

	JoinVidThread(); // 2. [Video] [Thread join] 처리 함수

	std::cout << "[RTSP] Previous State Reset Complete" << std::endl;

	std::cout << " " << std::endl;
}


// 4. [RTSP] 영상 초기화 & 검은 화면 출력 함수
void CRTSPCameraTestViewerDlg::ClearRTSPView()
{
	// Picture Control의 DC 얻기
	CClientDC dc(&m_staticCameraView);

	// Picture Control의 영역 얻기
	CRect rect;
	m_staticCameraView.GetClientRect(&rect);

	// 해당 영역을 검정색으로 채우기
	dc.FillSolidRect(rect, RGB(0, 0, 0));
}


// [십자선 표시] 체크박스 클릭 이벤트 함수
void CRTSPCameraTestViewerDlg::OnBnClickedCheckCrossLine()
{
	// 체크 상태이면 [true] & 해제 상태이면 [false]
	m_showCrossLine =
		m_checkCrossLine.GetCheck() == BST_CHECKED;

	// [십자선 표시] ON!
	if (m_showCrossLine)
	{
		std::cout << "[CROSS LINE] ON" << std::endl;

	}
	// [십자선 표시] OFF!
	else
	{
		std::cout << "[CROSS LINE] OFF" << std::endl;

		std::cout << " " << std::endl;

		m_crossLineColorIndex++; // 색상 인덱스 증가

		// 0 ~ 2 반복
		if (m_crossLineColorIndex > 2)
		{
			m_crossLineColorIndex = 0;
		}

		switch (m_crossLineColorIndex)
		{
		case 0:
			// 0 : 초록색
			m_crossLineColor = cv::Scalar(0, 255, 0, 255);
			std::cout << "[CROSS LINE COLOR] GREEN" << std::endl;
			break;

		case 1:
			// 1 : 노란색
			m_crossLineColor = cv::Scalar(0, 255, 255, 255);
			std::cout << "[CROSS LINE COLOR] YELLOW" << std::endl;
			break;

		case 2:
			// 2 : 하늘색
			m_crossLineColor = cv::Scalar(255, 255, 0, 255);
			std::cout << "[CROSS LINE COLOR] SKY BLUE" << std::endl;
			break;

		default:
			m_crossLineColorIndex = 0;
			// 0 : 초록색
			m_crossLineColor = cv::Scalar(0, 255, 0, 255);
			std::cout << "[CROSS LINE COLOR] GREEN" << std::endl;
			break;
		}

	}

}
