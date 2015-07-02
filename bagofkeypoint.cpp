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
const int MAX_CLUSTER = 100;//クラスタkの数


int surf(Mat &imgs,string path){
	//SURF
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	extractor.extended = true;//特徴ベクトルを１２８次元にする
	//	SURF img_surf = SURF(500, 4, 2, true);//SURFを初期化
	vector<KeyPoint> kp;
	//	img_surf(img, Mat(), kp);// vector<KeyPoint> keypoints;
	Mat img = imread(path,0);
	detector.detect(img, kp);
	extractor.compute(img, kp, imgs);
	/*
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();//ベクター内をすべて走査
	for (; it != it_end; ++it) {
	circle(img, Point(it->pt.x, it->pt.y),
	saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//○を描く(○を書く画像,中心座標,半径,色)
	}*/
	//	imgs = img;
	return 0;
}

int loadDescriptors(Mat& featureVectors, vector<string> file_list) {


	vector< vector<float> > samples;

	for (auto &path : file_list) {
		// SURFを抽出
		Mat descriptors;
		int ret = surf(descriptors, path);
		if (ret != 0) {
			cerr << "error in extractSURF" << endl;
			return 1;
		}
		for (int i = 0; i < descriptors.rows; i++){
			samples.push_back(descriptors.row(i));
		}
		// ファイル名と局所特徴点の数を表示
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

	// 各画像のヒストグラムを出力するファイルを開く
	fstream fout;
	fout.open("histograms.txt", ios::out);
	if (!fout.is_open()) {
		cerr << "cannot open file: histograms.txt" << endl;
		return 1;
	}

	for (auto &path : file_list){


		// ヒストグラムを初期化
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
			Mat indices = (Mat_<int>(1, 1)); // dataの何行目か
			Mat dists = (Mat_<float>(1, 1)); // それぞれどれだけの距離だったか
			Mat query = (Mat_<float>(1, descriptors.cols));
			//descriptors.col(i).isContinuous()がfalseなので新しく行列queryを作る
			for (int m = 0; m < descriptors.cols; m++){
				query.at<float>(0, m) = descriptors.at<float>(i, m);
			}
			idx.radiusSearch(query, indices, dists, r, maxResults);
			histogram[indices.at<int>(0, 0)] += 1;
		}

		// ヒストグラムをファイルに出力
		if (0 != descriptors.rows){
			fout << path << "\t";
			for (int i = 0; i < visualWords.rows; i++) {
				fout << float(histogram[i]) / float(descriptors.rows) << "\t";
			}
			fout << endl;
		}
		// 後始末
		delete[] histogram;
	}

	fout.close();

	return 0;
}
int classHistograms(Mat& visualWords, Mat hclass, string classl) {
	flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);

	// 各画像のヒストグラムを出力するファイルを開く
	fstream fout;
	fout.open("histograms.txt", ios::app);
	if (!fout.is_open()) {
		cerr << "cannot open file: histograms.txt" << endl;
		return 1;
	}



		// ヒストグラムを初期化
		float* histogram = new float[visualWords.rows];

		for (int i = 0; i < visualWords.rows; i++) {
			histogram[i] = 0;
		}

		Mat descriptors;
		descriptors = hclass;
		int maxResults = 1;
		float r = 1.0f;
		for (long i = 0; i < descriptors.rows; i++){
			Mat indices = (Mat_<int>(1, 1)); // dataの何行目か
			Mat dists = (Mat_<float>(1, 1)); // それぞれどれだけの距離だったか
			Mat query = (Mat_<float>(1, descriptors.cols));
			//descriptors.col(i).isContinuous()がfalseなので新しく行列queryを作る
			for (int m = 0; m < descriptors.cols; m++){
				query.at<float>(0, m) = descriptors.at<float>(i, m);
			}
			idx.radiusSearch(query, indices, dists, r, maxResults);
			histogram[indices.at<int>(0, 0)] += 1;
		}

		// ヒストグラムをファイルに出力
		if (0 != descriptors.rows){
			fout << classl << "\t";
			for (int i = 0; i < visualWords.rows; i++) {
				fout << float(histogram[i]) / float(descriptors.rows) << "\t";
			}
			fout << endl;
		}
		// 後始末
		delete[] histogram;


	fout.close();

	return 0;
}
int main(int argc, char *argv[]){
	initModule_nonfree();
	vector<string> file_list;

	// \dataの中のファイル名を取得する
	for (tr2::sys::directory_iterator it("./data"), end; it != end; ++it) {

			file_list.push_back(it->path());

	}

	// 取得したファイル名をすべて表示する
	for (auto &path : file_list) {
		cout << path << endl;
	}
	
	ifstream filec("class.txt");//classを読み込む
	if (filec.fail()){
		cerr << "failed." << endl;
		exit(0);
	}
	string strc;
	vector<string> class_list;
	while (getline(filec, strc)){
		class_list.push_back(strc);//class_listにclass名を格納
	}

	// dataの各画像から局所特徴量を抽出
	cout << "Load Descriptors ..." << endl;
	Mat featureVectors = (Mat_<float>(1, DIM));
	cout << featureVectors.rows << "\n";
	//特徴ベクトルを取り出し、学習用DBを作る
	int ret = loadDescriptors(featureVectors, file_list);

	// 局所特徴量をクラスタリングして各クラスタのセントロイドを計算
	cout << "Clustering ..." << endl;
	Mat labels(featureVectors.rows, 1, CV_64F);        // 各サンプル点が割り当てられたクラスタのラベル
	//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // 各クラスタの中心（セントロイド） DIM次元ベクトル
	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

	// 各画像をVisual Wordsのヒストグラムに変換する
	// 各クラスターの中心ベクトル、centroidsがそれぞれVisual Wordsになる
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
		vector<string>().swap(new_list);//メモリ解放
		classVectors.release();
	}
	//Matをxmlファイルに書き出し
	FileStorage cvfs("test.xml", CV_STORAGE_WRITE);
	write(cvfs, "featureVectors", featureVectors);
	FileStorage cvfs2("centroids.xml", CV_STORAGE_WRITE);
	write(cvfs2, "centroids", centroids);
	

	return 0;

}



