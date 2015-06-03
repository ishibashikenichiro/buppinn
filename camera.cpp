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

	// �ϐ��錾
	Mat im;
	Mat imc;
	int flag = 0;
	// �J�����̃L���v�`��
	VideoCapture cap(0);
	// �L���v�`���̃G���[����
	if (!cap.isOpened()) return -1;

	while (1) {
		// �J�����f���̎擾
		cap >> im;
		// �f���̕\��
		imshow("Camera", im);
		if (waitKey(30) == 'e') break;//e�������ƏI���
		// s�L�[���͂�����΃L���v�`��
		if (waitKey(30) == 's'){
			cap >> imc;
			flag = 1;
		}
		if (flag == 1){
			Mat img = surf(imc);//�L���v�`����SURF���s
			namedWindow("SURF", CV_WINDOW_AUTOSIZE);
			imshow("SURF", img);
			flag = 0;
			waitKey(0);
		}
		
	}
	
	return 0;

}

Mat surf(Mat img){
	SURF img_surf = SURF(500, 4, 2, true);//SURF��������
	vector<KeyPoint> kp;
	img_surf(img, Mat(), kp);//SURF
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();
	for (; it != it_end; ++it) {
		circle(img, Point(it->pt.x, it->pt.y),
			saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//����`��(���������摜,���S���W,���a,�F)
	}

	return img;
}