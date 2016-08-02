// VideoShow.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv/cv.hpp>  
#include <opencv/highgui.h> 
#include <opencv/cxcore.h>
#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/types_c.h> 
//#include <opencv2\objdetect\objdetect_c.h> 
#include <opencv2/core/types.hpp> 
#include <cstdio>  
#include <cstdlib>  
#include <Windows.h> 

#include "inc/imageParam.h"

#include <iostream>

using namespace std;
using namespace cv;

char * dstName0 = "E:\\studyWorkResearch\\Video Projects\\Identification&Tracking\\Ant Videos\\videoSrc.avi";
char * dstName1 = "E:\\studyWorkResearch\\Video Projects\\Identification&Tracking\\Ant Videos\\videoDst.avi";
char * dstNameCanny = "E:\\studyWorkResearch\\Video Projects\\Identification&Tracking\\Ant Videos\\videoCanny.avi";

int main(int argc, _TCHAR* argv[])
{
	cv::VideoCapture vc;
	bool stop = false;
	bool bOpen = true;
	int delay = 0;
	long currentFrame = 0;
	IMAGE_SIZE imageSize;
	Mat vcFrame;

	Mat cannyImage;
	Mat imageTmp;
	Mat dstImage;
	cvNamedWindow("Extracted Frame",1);
	int i = 0;
	VideoWriter videoWrite;
	VideoWriter videoWriteCanny;

	Size dstVideoSize;
	//打开视频文件：其实就是建立一个VideoCapture结构
	vc.open("E:\\studyWorkResearch\\Video Projects\\Identification&Tracking\\Ant Videos\\video1.mp4");
	//检测是否正常打开:成功打开时，isOpened返回ture
	if (!vc.isOpened())
	{
		printf( "fail to open video !\n");
		return -1;
	}
		
	//获取整个帧数
	long totalFrameNumber = (long)vc.get(CV_CAP_PROP_FRAME_COUNT);
	
	imageSize.width = vc.get(CV_CAP_PROP_FRAME_WIDTH);
	imageSize.height = vc.get(CV_CAP_PROP_FRAME_HEIGHT);
	
	//获取帧率
	double rate = vc.get(CV_CAP_PROP_FPS);
	printf("The frame rate is %d.\n", rate);
	if (rate <10)/*有的视频可能获取不到帧率，防止出现帧率为0或很慢的情况*/
	{
		rate = 25;
	}
	printf("The video has %d frames, width:%d, height:%d, rate:%d.\n", totalFrameNumber, imageSize.width, imageSize.height, rate);

	//设置开始帧，结束帧
	int frameToStart = 4320;
	int frameToStop = 4435;
	vc.set(CV_CAP_PROP_POS_FRAMES, frameToStart);

	if (frameToStop < frameToStart)
	{
		printf("The ending position is earlier than begining position.\n");
		return -1;
	}
	dstVideoSize.width = imageSize.width;
	dstVideoSize.height = imageSize.height;
	videoWrite.open(dstName0, CV_FOURCC('P', 'I', 'M', '1'), rate, dstVideoSize,true);
	videoWriteCanny.open(dstNameCanny, CV_FOURCC('P', 'I', 'M', '1'), rate, dstVideoSize,false);


	//两帧间的间隔时间:
	delay = (int)(1000 / rate);

	currentFrame = frameToStart;

	//滤波器设计
	int kernel_size = 3;
	cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);
	Mat res;
	vector<Mat>spl;
	while (!stop)
	{
		if (currentFrame != frameToStart)
			imageTmp = vcFrame;
		

		if (!vc.read(vcFrame))//读取下一帧
		{
			printf("read video failure.\n");
			return -1;
		}
		else
		{
			if ((currentFrame - frameToStart) % 3 != 0)
			{
				currentFrame++;
				continue;
			}
			else
			{
				currentFrame++;
			}
		}
		


		//这里加滤波程序		
		//filter2D(vcFrame, vcFrame, -1, kernel);
		//imshow("after filter", vcFrame);

		cvtColor(vcFrame, cannyImage, CV_BGR2GRAY);
		Canny(vcFrame, cannyImage, 100, 200, 3, false);

		threshold(cannyImage, cannyImage, 128, 255, cv::THRESH_BINARY);

		imshow("Extracted Frame", vcFrame);
		
		
		if (currentFrame - frameToStart > 1)
		{
			subtract(vcFrame, imageTmp, dstImage);
			imshow("Canny Video", dstImage);
		}
			

		videoWrite << vcFrame;
		videoWriteCanny << cannyImage;


		

		//waitKey(int delay=0)当delay ≤ 0时会永远等待；当delay>0时会等待delay毫秒
		//当时间结束前没有按键按下时，返回值为-1；否则返回按键
		int c = cv::waitKey(delay);

		//按下ESC或者到达指定的结束帧后退出读取视频
		if ((char)c == 0x1B || currentFrame > frameToStop)
		{
			stop = true;
		}
		//按下按键后会停留在当前帧，等待下一次按键
		if (c >= 0)
		{
			cv::waitKey(0);
		}
		

		if (vcFrame.empty())
		{
			break;
		}

	}
	//关闭视频文件
	videoWrite.release();
	videoWriteCanny.release();

	vc.release();
	cv::waitKey(0);




	return 0;
}

