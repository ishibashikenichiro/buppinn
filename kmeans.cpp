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

using namespace cv;
using namespace std;
const int DIM = 128;
const int SURF_PARAM = 400;
const int MAX_CLUSTER = 100;//k�̐�

int surf(Mat img,Mat &imgs){
	//SURF
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	extractor.extended = true;//�����x�N�g�����P�Q�W�����ɂ���
//	SURF img_surf = SURF(500, 4, 2, true);//SURF��������
	vector<KeyPoint> kp;
//	img_surf(img, Mat(), kp);// vector<KeyPoint> keypoints;
	detector.detect(img, kp);
	extractor.compute(img, kp, imgs);
	/*
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();//�x�N�^�[�������ׂđ���
	for (; it != it_end; ++it) {
		circle(img, Point(it->pt.x, it->pt.y),
			saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//����`��(���������摜,���S���W,���a,�F)
	}*/
//	imgs = img;
	return 0;
}

int loadDescriptors(Mat imgld,Mat& featureVectors) {


	vector< vector<float> > samples;


		// SURF�𒊏o
		Mat descriptors;
		int ret = surf(imgld, descriptors);
		if (ret != 0) {
			cerr << "error in extractSURF" << endl;
			return 1;
		}
		for (int i = 0; i < descriptors.rows; i++){
			samples.push_back(descriptors.row(i));
		}
		// �t�@�C�����ƋǏ������_�̐���\��
	//	cout << filePath << "\t" << descriptors.rows << endl;
	
	featureVectors.resize(samples.size());
	for (int i = 0; i < samples.size(); i++){
		for (int j = 0; j < samples[i].size(); j++){
			featureVectors.at<float>(i, j) = samples[i][j];
		}
	}
	return 0;
}

int calcHistograms(Mat imgch, Mat& visualWords) {
	flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);

	// �e�摜�̃q�X�g�O�������o�͂���t�@�C�����J��

	ofstream file("histograms.txt", ios::app);//�q�X�g�O�����������ݗp
	if (file.fail()){
		cerr << "failed." << endl;
		exit(0);
	}
	//�t�@�C���������
	std::string str;
	std::cin >> str;
	std::cout << str << std::endl;

	//�ʐ^��ۑ�
	imwrite(str + ".jpg", imgch);
	
		// �q�X�g�O������������
		int* histogram = new int[visualWords.rows];

		for (int i = 0; i < visualWords.rows; i++) {
			histogram[i] = 0;
		}

		Mat descriptors;
		int ret = surf(imgch, descriptors);
		if (ret != 0) {
			cerr << "error in extractSURF" << endl;
			return 1;
		}
		int maxResults = 1;
		float r = 1.0f;
		for (int i = 0; i < descriptors.rows; i++){
			Mat indices = (Mat_<int>(1, 1)); // data�̉��s�ڂ�
			Mat dists = (Mat_<float>(1, 1)); // ���ꂼ��ǂꂾ���̋�����������
			Mat query = (Mat_<float>(1, descriptors.cols));
			//descriptors.col(i).isContinuous()��false�Ȃ̂ŐV�����s��query�����
			for (int m = 0; m < descriptors.cols; m++){
				query.at<float>(0, m) = descriptors.at<float>(i, m);
			}
			idx.radiusSearch(query, indices, dists, r, maxResults);
			histogram[indices.at<int>(0, 0)] += 1;
		}

		// �q�X�g�O�������t�@�C���ɏo��
		if (0 != descriptors.rows){
			file << str << "\t";
			for (int i = 0; i < visualWords.rows; i++) {
				file << float(histogram[i]) / float(descriptors.rows) << "\t";
			}
			file << endl;
		}
		// ��n��
		delete[] histogram;


	file.close();

	return 0;
}

int main(int argc, char *argv[]){
	initModule_nonfree();

	ofstream file("histograms.txt", ios::app);//�q�X�g�O�����������ݗp
	if (file.fail()){
		cerr << "failed." << endl;
		exit(0);
	}

	// �ϐ��錾
	Mat im;
	Mat imc;
//	int flag = 0;
	// �J�����̃L���v�`��
	VideoCapture cap(0);
	// �L���v�`���̃G���[����
	if (!cap.isOpened()) return -1;

	while (1) {
		// �J�����f���̎擾
		cap >> im;
		// �f���̕\��
		imshow("Camera", im);
//		if (waitKey(30) == 'e') break;//e�������ƏI���
		// s�L�[���͂�����΃L���v�`��
		if (waitKey(30) == 's'){
			cap >> imc;
			break;
//			flag = 1;
		}

	}
	// IMAGE_DIR�̊e�摜����Ǐ������ʂ𒊏o
	cout << "Load Descriptors ..." << endl;
	Mat featureVectors = (Mat_<float>(1, DIM));
	//�����x�N�g�������o���A�w�K�pDB�����
	int ld = loadDescriptors(imc, featureVectors);
	// �Ǐ������ʂ��N���X�^�����O���Ċe�N���X�^�̃Z���g���C�h���v�Z
	cout << "Clustering ..." << endl;
	Mat labels(featureVectors.rows, 1, CV_64F);        // �e�T���v���_�����蓖�Ă�ꂽ�N���X�^�̃��x��
	//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // �e�N���X�^�̒��S�i�Z���g���C�h�j DIM�����x�N�g��
	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

	// �e�摜��Visual Words�̃q�X�g�O�����ɕϊ�����
	// �e�N���X�^�[�̒��S�x�N�g���Acentroids�����ꂼ��Visual Words�ɂȂ�
	cout << "Calc Histograms ..." << endl;
	calcHistograms(imc,centroids);
	/*
	// IMAGE_DIR�̊e�摜����Ǐ������ʂ𒊏o
	cout << "Load Descriptors ..." << endl;
	Mat featureVectors = (Mat_<float>(1, DIM));
	cout << featureVectors.rows << "\n";
	//�����x�N�g�������o���A�w�K�pDB�����
	int ret = loadDescriptors(featureVectors);

	// �Ǐ������ʂ��N���X�^�����O���Ċe�N���X�^�̃Z���g���C�h���v�Z
	cout << "Clustering ..." << endl;
	Mat labels(featureVectors.rows, 1, CV_64F);        // �e�T���v���_�����蓖�Ă�ꂽ�N���X�^�̃��x��
	//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // �e�N���X�^�̒��S�i�Z���g���C�h�j DIM�����x�N�g��
	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

	// �e�摜��Visual Words�̃q�X�g�O�����ɕϊ�����
	// �e�N���X�^�[�̒��S�x�N�g���Acentroids�����ꂼ��Visual Words�ɂȂ�
	cout << "Calc Histograms ..." << endl;
	calcHistograms(centroids);
	*/

	return 0;

}



