
// RTSP_Camera_Test_ViewerDlg.h: 헤더 파일
//

#pragma once

#include <thread>
#include <atomic>

#include "CRTSPDecoder.h"
#include "CVideoView.h"


// CRTSPCameraTestViewerDlg 대화 상자
class CRTSPCameraTestViewerDlg : public CDialogEx
{
	// 생성입니다.
public:
	CRTSPCameraTestViewerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RTSP_CAMERA_TEST_VIEWER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:

	// ================================
	// [UI Control 변수]
	// ================================

	// 영상 출력용 Picture Control
	CStatic m_staticCameraView;

	// RTSP 주소 입력 Edit Control
	CEdit m_editsRtspUrl;

	// 카메라 ID 입력 Edit Control
	CEdit m_editCameraId;

	// 카메라 PW 입력 Edit Control
	CEdit m_editCameraPw;

	// Connect 버튼 Control
	CButton m_btnConnect;

	// Disconnect 버튼 Control
	CButton m_btnDisconnect;

	// 십자선 표시 ON/OFF 체크박스 Control
	CButton m_checkCrossLine;

	// ================================
	// [RTSP / Video 처리 변수]
	// ================================

	// RTSP 연결 및 프레임 수신을 담당하는 디코더
	CRTSPDecoder m_rtspDecoder;

	// Picture Control에 영상을 그려주는 View 객체
	CVideoView m_cameraView;

	// ================================
	// 현재 입력 영상 해상도 표시 Static Text
	// ================================
	CStatic m_staticResolutions;

	// ================================
	// 이전 해상도 문자열 저장
	// ================================
	CString m_prevResolutionText;

	// ================================
	// ================================
	// 영상 수신 / 출력용 Thread
	std::thread m_videoThread;

	// [비디오 재생] 여부
	std::atomic<bool> m_isPlaying = false;

	// [십자선 표시] 여부
	std::atomic<bool> m_showCrossLine = true;

	// [현재 십자선] 색상 인덱스
	// 0 : 초록색
	// 1 : 노란색
	// 2 : 하늘색
	int m_crossLineColorIndex = 0;

	// [현재 십자선] 색상 (BGRA)
	cv::Scalar m_crossLineColor =
		cv::Scalar(0, 255, 0, 255);

	CFont m_fontUI; // [UI Control] 폰트

public:
	// [RTSP] [Connect] 버튼 클릭 이벤트 함수
	afx_msg void OnBnClickedBtnConnect();

	// [RTSP] [DisConnect] 버튼 클릭 이벤트 함수
	afx_msg void OnBnClickedBtnDisconnect();

	// [십자선 표시] 체크박스 클릭 이벤트 함수
	afx_msg void OnBnClickedCheckCrossLine();

private:
	// RTSP URL을 받아 실제 영상 출력 Thread에서 실행되는 함수
	void PlayRTSPVideo(std::string rtspUrl);

	// 1. [RTSP] 영상 종료 함수
	void StopRTSPVideo();

	// 2. [Video] [Thread join] 처리 함수
	void JoinVidThread();

	// 3. [RTSP] 내부상태 초기화 함수
	void ResetRTSPView();

	// 4. [RTSP] 영상 초기화 & 검은 화면 출력 함수
	void ClearRTSPView();
};
