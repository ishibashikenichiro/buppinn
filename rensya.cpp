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

const int M = 10;//�B���

int main(int argc, char *argv[]){
	fstream fout;
	fout.open("class.txt", ios::app);
	if (!fout.is_open()) {
		cerr << "cannot open file: class.txt" << endl;
		return 1;
	}

	// �ϐ��錾
	Mat im(640, 480, 1);
	Mat imc(640, 480, 1);
	// �J�����̃L���v�`��
	VideoCapture cap(0);
	// �L���v�`���̃G���[����
	if (!cap.isOpened()) return -1;
		cap >> imc;
		imshow("Camera", imc);
		//�t�@�C���������
		string str;
		cin >> str;
		cout << str << endl;
		fout << str << endl;
		fout.close();
		string name;


		for (int i = 0; i < M; i++) {
			// �J�����f���̎擾
			cap >> im;
			//	�ۑ�
			name = str + "_" + to_string(i);
			imwrite("data/" + name + ".jpg", im);
			cout << i << endl;
			//�����҂�
			Sleep(500);

		}

	return 0;

}

