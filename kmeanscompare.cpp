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
const int MAX_CLUSTER = 500;//kの数

int surf(Mat img, Mat &imgs){
	//SURF
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	extractor.extended = true;//特徴ベクトルを１２８次元にする
	vector<KeyPoint> kp;
	detector.detect(img, kp);
	extractor.compute(img, kp, imgs);
	/*
	vector<KeyPoint>::iterator it = kp.begin(), it_end = kp.end();//ベクター内をすべて走査
	for (; it != it_end; ++it) {
	circle(img, Point(it->pt.x, it->pt.y),
	saturate_cast<int>(it->size*0.25), Scalar(255, 255, 0));//○を描く(○を書く画像,中心座標,半径,色)
	}*/
	return 0;
}

int loadDescriptors(Mat imgld, Mat& featureVectors) {


	vector< vector<float> > samples;


	// SURFを抽出
	Mat descriptors;
	int ret = surf(imgld, descriptors);
	if (ret != 0) {
		cerr << "error in extractSURF" << endl;
		return 1;
	}
	for (int i = 0; i < descriptors.rows; i++){
		samples.push_back(descriptors.row(i));
	}
	// ファイル名と局所特徴点の数を表示
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

	// ヒストグラムを初期化
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
	for (long i = 0; i < descriptors.rows; i++){//int →　longに
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

		for (int i = 0; i < visualWords.rows; i++) {
			values[i+1] = (double)histogram[i]/(double)descriptors.rows;
		}
	// 後始末
	delete[] histogram;


	return 0;
}

void compare(vector<double> values2){
	ifstream file("histograms.txt");//ヒストグラムを読み込む
	ofstream result("result.txt");
	ofstream bihin("bihin.txt");
	vector<vector<string> > values;//ヒストグラム格納

	double sum[1000];
	double w = 0, x = 0, y = 0, z = 0;
	string str, str2,str3;
	int p;

	if (file.fail()){
		cerr << "failed." << endl;
		exit(0);
	}

	while (getline(file, str)){
		//コメント箇所は除く
		if ((p = str.find("//")) != str.npos) continue;
		vector<string> inner;

		//tabがあるかを探し、そこまでをvaluesに格納
		while ((p = str.find("\t")) != str.npos){
			inner.push_back(str.substr(0, p));

			//strの中身は"\t"を飛ばす
			str = str.substr(p + 1);
		}
		inner.push_back(str);
		values.push_back(inner);
	}

	int n = values.size();
	vector<pair<double, string> > pairs(n);//並び替え用のペア作成

	for (unsigned int i = 0; i < values.size(); ++i){
		for (unsigned int j = 0; j < values[i].size(); ++j){
			//stringからdoubleにキャスト変換
			istringstream is;
			int is2;
			is.str(values[i][j]);
			y =(double)values2[j];
			is >> w;
			x += w;//valuesのトータル
			double m = 0;
			if ((w - y)>0){
				m = y;
			}
			else{
				m = w;
			}
			z += m;//学習データと比較用データの内小さい方の合計

		}
		sum[i] = z / x;
		cout << sum[i] << endl;
		//ペアに値を格納
		pairs[i] = make_pair(sum[i], values[i][0]);

		z = 0;
		x = 0;
	}

	//ソート
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

	// 変数宣言
	Mat im(640,480,1);
	Mat imc(640, 480, 1);
	//	int flag = 0;
		// カメラのキャプチャ
		VideoCapture cap(0);
		// キャプチャのエラー処理
		if (!cap.isOpened()) return -1;

		while (1) {
			// カメラ映像の取得
			cap >> im;
			// 映像の表示
			imshow("Camera", im);
			//		if (waitKey(30) == 'e') break;//eを押すと終わり
			// sキー入力があればキャプチャ
			if (waitKey(30) == 's'){
				cap >> imc;
				imwrite("test.jpg", imc);
				break;
				//			flag = 1;
			}

		}
		cout << "Load Descriptors ..." << endl;
		//学習済みVWを読み込み
		Mat centroids;
		FileStorage cvfs("centroids.xml", CV_STORAGE_READ);
		FileNode node(cvfs.fs, NULL);
		read(node["centroids"], centroids);
		// 局所特徴量をクラスタリングして各クラスタのセントロイドを計算
		cout << "Clustering ..." << endl;
	//	Mat labels(featureVectors.rows, 1, CV_64F);        // 各サンプル点が割り当てられたクラスタのラベル
		//Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // 各クラスタの中心（セントロイド） DIM次元ベクトル
	//	Mat centroids = Mat_<float>(MAX_CLUSTER, DIM);
	//	kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);

		// 各画像をVisual Wordsのヒストグラムに変換する
		// 各クラスターの中心ベクトル、centroidsがそれぞれVisual Wordsになる
		vector<double> values2(10000);//比較用ヒストグラム格納
		cout << "Calc Histograms ..." << endl;
		compareHistograms(imc, centroids, values2);

		//比較
		cout << "Compare ..." << endl;
		compare(values2);

		return 0;
	
}



