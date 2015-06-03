#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
using namespace cv;
using namespace std;

Mat surf(Mat img);

int main(int argc, char *argv[]){

	// 変数宣言
	Mat im;
	Mat imc;
	int flag = 0;
	// カメラのキャプチャ
	VideoCapture cap(0);
	// キャプチャのエラー処理
	if (!cap.isOpened()) return -1;

	while (1) {
		// カメラ映像の取得
		cap >> im;
		// 映像の表示
		imshow("Camera", im);
		if (waitKey(30) == 'e') break;//eを押すと終わり
		// sキー入力があればキャプチャ
		if (waitKey(30) == 's'){
			cap >> imc;
			flag = 1;
		}
		if (flag == 1){
			Mat img = surf(imc);//キャプチャにSURF実行
			namedWindow("SURF", CV_WINDOW_AUTOSIZE);
			imshow("SURF", img);
			flag = 0;
			waitKey(0);
		}
		
	}
	
	return 0;

}

Mat surf(Mat img){
	SURF img_surf = SURF(500, 4, 2, true);//SURFを初期化
	vector<KeyPoint> kp;
	img_surf(img, Mat(), kp);//SURF
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();
	for (; it != it_end; ++it) {
		circle(img, Point(it->pt.x, it->pt.y),
			saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//○を描く(○を書く画像,中心座標,半径,色)
	}

	return img;
}