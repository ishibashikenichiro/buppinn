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
#include <iostream>
#include <utility>
#include <functional>

using namespace cv;
using namespace std;
const int DIM = 128;
const int SURF_PARAM = 400;
const int MAX_CLUSTER = 500;//k�̐�

int surf(Mat img, Mat &imgs){
	//SURF
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	extractor.extended = true;//�����x�N�g�����P�Q�W�����ɂ���
	vector<KeyPoint> kp;
	detector.detect(img, kp);
	extractor.compute(img, kp, imgs);
	/*
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();//�x�N�^�[�������ׂđ���
	for (; it != it_end; ++it) {
	circle(img, Point(it->pt.x, it->pt.y),
	saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//����`��(���������摜,���S���W,���a,�F)
	}*/
	return 0;
}

int loadDescriptors(Mat imgld, Mat& featureVectors) {


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

int compareHistograms(Mat imgch, Mat& visualWords,vector<double> &values) {
	flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);

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
	for (long i = 0; i < descriptors.rows; i++){//int ���@long��
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

		for (int i = 0; i < visualWords.rows; i++) {
			values[i+1] = (double)histogram[i]/(double)descriptors.rows;
		}
	// ��n��
	delete[] histogram;


	return 0;
}

void compare(vector<double> values2){
	ifstream file("histograms.txt");//�q�X�g�O������ǂݍ���
	ofstream result("result.txt");
	ofstream bihin("bihin.txt");
	vector<vector<string> > values;//�q�X�g�O�����i�[

	double sum[1000];
	double w = 0, x = 0, y = 0, z = 0;
	string str, str2,str3;
	int p;

	if (file.fail()){
		cerr << "failed." << endl;
		exit(0);
	}

	while (getline(file, str)){
		//�R�����g�ӏ��͏���
		if ((p = str.find("//")) != str.npos) continue;
		vector<string> inner;

		//tab�����邩��T���A�����܂ł�values�Ɋi�[
		while ((p = str.find("\t")) != str.npos){
			inner.push_back(str.substr(0, p));

			//str�̒��g��"\t"���΂�
			str = str.substr(p + 1);
		}
		inner.push_back(str);
		values.push_back(inner);
	}

	int n = values.size();
	vector<pair<double, string> > pairs(n);//���ёւ��p�̃y�A�쐬

	for (unsigned int i = 0; i < values.size(); ++i){
		for (unsigned int j = 0; j < values[i].size(); ++j){
			//string����double�ɃL���X�g�ϊ�
			istringstream is;
			int is2;
			is.str(values[i][j]);
			y =(double)values2[j];
			is >> w;
			x += w;//values�̃g�[�^��
			double m = 0;
			if ((w - y)>0){
				m = y;
			}
			else{
				m = w;
			}
			z += m;//�w�K�f�[�^�Ɣ�r�p�f�[�^�̓����������̍��v

		}
		sum[i] = z / x;
		cout << sum[i] << endl;
		//�y�A�ɒl���i�[
		pairs[i] = make_pair(sum[i], values[i][0]);

		z = 0;
		x = 0;
	}

	//�\�[�g
	stable_sort(pairs.begin(), pairs.end(), greater<pair<double, string>>());
	for (int k = 0; k < values.size(); k++){
		result << pairs[k].first << "\t" << pairs[k].second << endl;
	}
	str3 = pairs[0].second;
	int a = str3.find("_");
	vector<string> inner2;
	inner2.push_back(str3.substr(0, a));
	bihin << inner2[0] << endl;
	return;
}

int main(int argc, char *argv[]){
	initModule_nonfree();

	// �ϐ��錾
	Mat im(640,480,1);
	Mat imc(640, 480, 1);
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
				imwrite("test.jpg", imc);
				break;
				//			flag = 1;
			}

		}
		cout << "Load Descriptors ..." << endl;
		//�w�K�ς�VW��ǂݍ���
		Mat centroids;
		FileStorage cvfs("centroids.xml", CV_STORAGE_READ);
		FileNode node(cvfs.fs, NULL);
		read(node["centroids"], centroids);
		// �Ǐ������ʂ��N���X�^�����O���Ċe�N���X�^�̃Z���g���C�h���v�Z
		cout << "Clustering ..." << endl;
	//	Mat labels(featureVectors.rows, 1, CV_64F);        // �e�T���v���_�����蓖�Ă�ꂽ�N���X�^�̃��x��
		//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // �e�N���X�^�̒��S�i�Z���g���C�h�j DIM�����x�N�g��
	//	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	//	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

		// �e�摜��Visual Words�̃q�X�g�O�����ɕϊ�����
		// �e�N���X�^�[�̒��S�x�N�g���Acentroids�����ꂼ��Visual Words�ɂȂ�
		vector<double> values2(10000);//��r�p�q�X�g�O�����i�[
		cout << "Calc Histograms ..." << endl;
		compareHistograms(imc, centroids, values2);

		//��r
		cout << "Compare ..." << endl;
		compare(values2);

		return 0;
	
}



