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
#include <string>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <exception>
#include <filesystem>


using namespace cv;
using namespace std;
const int DIM = 128;
const int SURF_PARAM = 400;
const int MAX_CLUSTER = 100;//�N���X�^k�̐�


int surf(Mat &imgs,string path){
	//SURF
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	extractor.extended = true;//�����x�N�g�����P�Q�W�����ɂ���
	//	SURF img_surf = SURF(500, 4, 2, true);//SURF��������
	vector<KeyPoint> kp;
	//	img_surf(img, Mat(), kp);// vector<KeyPoint> keypoints;
	Mat img = imread(path,0);
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

int loadDescriptors(Mat& featureVectors, vector<string> file_list) {


	vector< vector<float> > samples;

	for (auto &path : file_list) {
		// SURF�𒊏o
		Mat descriptors;
		int ret = surf(descriptors, path);
		if (ret != 0) {
			cerr << "error in extractSURF" << endl;
			return 1;
		}
		for (int i = 0; i < descriptors.rows; i++){
			samples.push_back(descriptors.row(i));
		}
		// �t�@�C�����ƋǏ������_�̐���\��
			cout << path << "\t" << descriptors.rows << endl;

		featureVectors.resize(samples.size());
		for (int i = 0; i < samples.size(); i++){
			for (int j = 0; j < samples[i].size(); j++){
				featureVectors.at<float>(i, j) = samples[i][j];
			}
		}
	}
	return 0;
}

int calcHistograms(Mat& visualWords, vector<string> file_list) {
	flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);

	// �e�摜�̃q�X�g�O�������o�͂���t�@�C�����J��
	fstream fout;
	fout.open("histograms.txt", ios::out);
	if (!fout.is_open()) {
		cerr << "cannot open file: histograms.txt" << endl;
		return 1;
	}

	for (auto &path : file_list){


		// �q�X�g�O������������
		float* histogram = new float[visualWords.rows];

		for (int i = 0; i < visualWords.rows; i++) {
			histogram[i] = 0;
		}

		Mat descriptors;
		int ret = surf(descriptors, path);
		if (ret != 0) {
			cerr << "error in extractSURF" << endl;
			return 1;
		}
		int maxResults = 1;
		float r = 1.0f;
		for (long i = 0; i < descriptors.rows; i++){
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
			fout << path << "\t";
			for (int i = 0; i < visualWords.rows; i++) {
				fout << float(histogram[i]) / float(descriptors.rows) << "\t";
			}
			fout << endl;
		}
		// ��n��
		delete[] histogram;
	}

	fout.close();

	return 0;
}
int classHistograms(Mat& visualWords, Mat hclass, string classl) {
	flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);

	// �e�摜�̃q�X�g�O�������o�͂���t�@�C�����J��
	fstream fout;
	fout.open("histograms.txt", ios::app);
	if (!fout.is_open()) {
		cerr << "cannot open file: histograms.txt" << endl;
		return 1;
	}



		// �q�X�g�O������������
		float* histogram = new float[visualWords.rows];

		for (int i = 0; i < visualWords.rows; i++) {
			histogram[i] = 0;
		}

		Mat descriptors;
		descriptors = hclass;
		int maxResults = 1;
		float r = 1.0f;
		for (long i = 0; i < descriptors.rows; i++){
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
			fout << classl << "\t";
			for (int i = 0; i < visualWords.rows; i++) {
				fout << float(histogram[i]) / float(descriptors.rows) << "\t";
			}
			fout << endl;
		}
		// ��n��
		delete[] histogram;


	fout.close();

	return 0;
}
int main(int argc, char *argv[]){
	initModule_nonfree();
	vector<string> file_list;

	// \data�̒��̃t�@�C�������擾����
	for (tr2::sys::directory_iterator it("./data"), end; it != end; ++it) {

			file_list.push_back(it->path());

	}

	// �擾�����t�@�C���������ׂĕ\������
	for (auto &path : file_list) {
		cout << path << endl;
	}
	
	ifstream filec("class.txt");//class��ǂݍ���
	if (filec.fail()){
		cerr << "failed." << endl;
		exit(0);
	}
	string strc;
	vector<string> class_list;
	while (getline(filec, strc)){
		class_list.push_back(strc);//class_list��class�����i�[
	}

	// data�̊e�摜����Ǐ������ʂ𒊏o
	cout << "Load Descriptors ..." << endl;
	Mat featureVectors = (Mat_<float>(1, DIM));
	cout << featureVectors.rows << "\n";
	//�����x�N�g�������o���A�w�K�pDB�����
	int ret = loadDescriptors(featureVectors, file_list);

	// �Ǐ������ʂ��N���X�^�����O���Ċe�N���X�^�̃Z���g���C�h���v�Z
	cout << "Clustering ..." << endl;
	Mat labels(featureVectors.rows, 1, CV_64F);        // �e�T���v���_�����蓖�Ă�ꂽ�N���X�^�̃��x��
	//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // �e�N���X�^�̒��S�i�Z���g���C�h�j DIM�����x�N�g��
	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

	// �e�摜��Visual Words�̃q�X�g�O�����ɕϊ�����
	// �e�N���X�^�[�̒��S�x�N�g���Acentroids�����ꂼ��Visual Words�ɂȂ�
	cout << "Calc Histograms ..." << endl;
	for (int i = 0; i < class_list.size(); i++){
		cout << class_list[i] << endl;
		vector<string> new_list;
		for (auto &path : file_list) {
			int loc = path.find(class_list[i], 0);
			if (loc != string::npos)
				new_list.push_back(path);
		}
		Mat classVectors = (Mat_<float>(1, DIM));
		int retc = loadDescriptors(classVectors, new_list);
		classHistograms(centroids, classVectors, class_list[i]);
		vector<string>().swap(new_list);//���������
		classVectors.release();
	}
	//Mat��xml�t�@�C���ɏ����o��
	FileStorage cvfs("test.xml", CV_STORAGE_WRITE);
	write(cvfs, "featureVectors", featureVectors);
	FileStorage cvfs2("centroids.xml", CV_STORAGE_WRITE);
	write(cvfs2, "centroids", centroids);
	

	return 0;

}



