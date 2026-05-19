
// CVideoView.cpp: 구현 파일
//

#include "pch.h"
#include "CVideoView.h"


// 생성자
CVideoView::CVideoView()
{
	m_pStatic = nullptr; // 초기에는 연결된 Static Control 없음
}


// Static Control 연결
void CVideoView::Attach(CStatic* pStatic)
{
	m_pStatic = pStatic;
}


// 영상 프레임 출력
void CVideoView::DrawFrame(
	const cv::Mat& frame,
	bool showCrossLine,
	cv::Scalar crossLineColor)
{
	// 연결된 화면이 없으면 종료
	if (m_pStatic == nullptr)
		return;

	// 비어있는 프레임이면 종료
	if (frame.empty())
		return;

	// Static Control 크기 얻기
	CRect rect;
	m_pStatic->GetClientRect(&rect);

	if (rect.Width() <= 0 || rect.Height() <= 0)
		return;

	// ========================================
	// [원본 영상 비율 유지 Resize]
	// ========================================

	// 원본 영상 비율 계산
	double srcRatio =
		static_cast<double>(frame.cols) /
		static_cast<double>(frame.rows);

	// Viewer 영역 비율 계산
	double viewRatio =
		static_cast<double>(rect.Width()) /
		static_cast<double>(rect.Height());

	// 실제 출력할 영상 크기
	int drawWidth = rect.Width();
	int drawHeight = rect.Height();

	// Viewer가 원본보다 더 넓은 경우
	// → 높이를 Viewer에 맞추고, 너비는 원본 비율에 맞게 계산
	if (viewRatio > srcRatio)
	{
		drawHeight = rect.Height();
		drawWidth = static_cast<int>(drawHeight * srcRatio);
	}
	// Viewer가 원본보다 더 좁거나 같은 경우
	// → 너비를 Viewer에 맞추고, 높이는 원본 비율에 맞게 계산
	else
	{
		drawWidth = rect.Width();
		drawHeight = static_cast<int>(drawWidth / srcRatio);
	}

	// Viewer 안에서 영상이 중앙에 오도록 위치 계산
	int offsetX = (rect.Width() - drawWidth) / 2;
	int offsetY = (rect.Height() - drawHeight) / 2;

	// 원본 비율을 유지한 크기로 리사이즈
	cv::Mat resizedFrame;
	cv::resize(
		frame,
		resizedFrame,
		cv::Size(drawWidth, drawHeight),
		0,
		0,
		cv::INTER_LINEAR);

	// ========================================
	// [BGRA 변환]
	// ========================================

	cv::Mat bgraFrame;

	if (resizedFrame.channels() == 3)
	{
		// OpenCV 기본 BGR -> BGRA
		cv::cvtColor(resizedFrame, bgraFrame, cv::COLOR_BGR2BGRA);
	}
	else if (resizedFrame.channels() == 4)
	{
		// 이미 4채널이면 그대로 사용
		bgraFrame = resizedFrame.clone();
	}
	else if (resizedFrame.channels() == 1)
	{
		// 흑백 영상이면 Gray -> BGRA
		cv::cvtColor(resizedFrame, bgraFrame, cv::COLOR_GRAY2BGRA);
	}
	else
	{
		return;
	}

	// ========================================
	// [십자선 표시]
	// ========================================

	if (showCrossLine)
	{
		// 실제 출력 영상 기준 중앙 좌표 계산
		int centerX = bgraFrame.cols / 2;
		int centerY = bgraFrame.rows / 2;

		int lineThickness = 1;

		// 세로선 그리기
		cv::line(
			bgraFrame,
			cv::Point(centerX, 0),
			cv::Point(centerX, bgraFrame.rows - 1),
			crossLineColor,
			lineThickness,
			cv::LINE_AA);

		// 가로선 그리기
		cv::line(
			bgraFrame,
			cv::Point(0, centerY),
			cv::Point(bgraFrame.cols - 1, centerY),
			crossLineColor,
			lineThickness,
			cv::LINE_AA);
	}

	// ========================================
	// [검정 배경 Canvas 생성]
	// ========================================

	// Viewer 전체 크기만큼 검정색 BGRA Canvas 생성
	cv::Mat canvas(
		rect.Height(),
		rect.Width(),
		CV_8UC4,
		cv::Scalar(0, 0, 0, 255));

	// 영상이 들어갈 영역 지정
	cv::Rect roi(
		offsetX,
		offsetY,
		drawWidth,
		drawHeight);

	// 비율 유지된 영상을 검정 배경 위 중앙에 복사
	bgraFrame.copyTo(canvas(roi));

	// 메모리 연속성 보장
	if (!canvas.isContinuous())
	{
		canvas = canvas.clone();
	}

	// ========================================
	// [GDI 출력]
	// ========================================

	// Static Control의 Device Context 얻기
	CClientDC dc(m_pStatic);

	// 출력 품질 설정
	SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);

	// Windows Bitmap 정보 구조체
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = canvas.cols;

	// 음수면 Top-Down 방식 출력
	bmi.bmiHeader.biHeight = -canvas.rows;

	bmi.bmiHeader.biPlanes = 1;

	// 32bit BGRA 출력
	bmi.bmiHeader.biBitCount = 32;

	// 압축 없음
	bmi.bmiHeader.biCompression = BI_RGB;

	// 실제 영상 출력
	// canvas에는 검정 여백 + 실제 영상이 이미 합쳐져 있으므로
	// Viewer 전체에 한 번만 출력하면 됨
	StretchDIBits(
		dc.GetSafeHdc(),
		0,
		0,
		rect.Width(),
		rect.Height(),
		0,
		0,
		canvas.cols,
		canvas.rows,
		canvas.data,
		&bmi,
		DIB_RGB_COLORS,
		SRCCOPY);
}