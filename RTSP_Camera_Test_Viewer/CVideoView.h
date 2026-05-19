#pragma once

// MFC 기본 클래스(CStatic 등) 사용
#include "pch.h"


// OpenCV 사용
#include <opencv2/opencv.hpp>


// 영상 출력 담당 클래스
class CVideoView
{
public:

	// 생성자
	CVideoView();

	// Static Control 연결
	// ex) IDC_STATIC_COLOR_CAM
	void Attach(CStatic* pStatic);

	// 영상 프레임을 Picture Control에 출력하는 함수
	// showCrossLine  : 십자선 표시 여부
	// crossLineColor : 십자선 색상(BGRA)
	void DrawFrame(
		const cv::Mat& frame,
		bool showCrossLine,
		cv::Scalar crossLineColor);

private:

	// 출력 대상 Static Control
	CStatic* m_pStatic;
};