#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/legacy/compat.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>

using namespace cv;
using namespace std;


int main(int argc, char *argv[]){

	//ファイル名を入力
	string str;
	cin >> str;
	cout << str << endl; 
	string name;
	// 変数宣言
	Mat im(640,480,1);
	Mat imc(640, 480, 1);
	// カメラのキャプチャ
	VideoCapture cap(0);
	// キャプチャのエラー処理
	if (!cap.isOpened()) return -1;
	cap >> imc;
	imshow("Camera", imc);
	for (int i = 0;i<10;i++) {
		// カメラ映像の取得
		cap >> im;
	//	保存
		name = str + to_string(i);
		imwrite("data/" + name +".jpg", im);
		cout << i << endl;
		//少し待つ
		Sleep(500);

	}

	return 0;

}

