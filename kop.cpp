#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/legacy/compat.hpp"
#include "iostream"
#include "fstream"
#include "dirent.h"
 
using namespace std;
using namespace cv;
 
const string IMAGE_DIR = "./pic";
const int DIM = 128;
const int SURF_PARAM = 400;
const int MAX_CLUSTER = 500;  // クラスタ数 = Visual Wordsの次元数
 
/**
* 画像ファイルからSURF特徴量を抽出する
* @param[in]  filename            画像ファイル名
* @param[out] imageKeypoints      キーポイント
* @param[out] imageDescriptors    各キーポイントのSURF特徴量
* @param[out] storage             メモリ領域
* @return 成功なら0、失敗なら1
*/
int extractSURF(const string fileName, Mat* des) {
    // グレースケールで画像をロードする
    IplImage* img = cvLoadImage(fileName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    if (img == NULL) {
        cerr << "cannot load image: " << fileName << endl;
        return 1;
    }
    //画像読み込み
    Mat colorImg = imread(fileName);
 
    //特徴点抽出用のグレー画像用意
    Mat grayImg;
    cvtColor(colorImg, grayImg, CV_BGR2GRAY);
    normalize(grayImg, grayImg, 0, 255, NORM_MINMAX);
 
    //SURF
    SurfFeatureDetector detector;
    SurfDescriptorExtractor extractor;
    extractor.extended = true;//特徴ベクトルを１２８次元にする。
 
    //画像から特徴点を検出
    vector<KeyPoint> keypoints;
    detector.detect(grayImg, keypoints);
 
    // 画像1の特徴点における特徴量を抽出
    extractor.compute( grayImg, keypoints, *des );
 
    return 0;
}
 
 
/**
* IMAGE_DIRにある全画像から局所特徴量を抽出し行列へ格納する
* @param[out]   featureVectors    局所特徴量の行列
* @param[out]   data       featureVectorsのデータ領域
* @return 成功なら0、失敗なら1
*/
int loadDescriptors(Mat& featureVectors) {
    // IMAGE_DIRを開く
    DIR* dp;
    if ((dp = opendir(IMAGE_DIR.c_str())) == NULL) {
        cerr << "cannot open directory: " << IMAGE_DIR << endl;
        return 1;
 
    }
 
    vector< vector<float> > samples;
    // IMAGE_DIRの画像ファイル名を走査
    struct dirent* entry;
    while ((entry = readdir(dp)) != NULL) {
        string fileName = string(entry->d_name);
        if (fileName.compare(".") == 0 || fileName.compare("..") == 0) {
            continue;
        }
 
        // パス名に変換
        // XXX.jpg -> IMAGE_DIR/XXX.jpg
        string filePath;    
        filePath = IMAGE_DIR + "/" + fileName;
 
        // SURFを抽出
        Mat descriptors;
        int ret = extractSURF(filePath, &descriptors);
        if (ret != 0) {
            cerr << "error in extractSURF" << endl;
            return 1;
        }
        for ( int i = 0; i < descriptors.rows; i++ ){
            samples.push_back(descriptors.row(i));
        }
        // ファイル名と局所特徴点の数を表示
        cout << filePath << "\t" << descriptors.rows << endl;
    }
    featureVectors.resize(samples.size());
    for ( int i = 0; i < samples.size(); i++ ){
        for ( int j = 0; j < samples[i].size(); j++ ){
            featureVectors.at<float>( i, j ) = samples[i][j];
        }
    }
        closedir(dp);
    return 0;
}
 
/**
* IMAEG_DIRの全画像をヒストグラムに変換して出力
* 各画像の各局所特徴量を一番近いVisual Wordsに投票してヒストグラムを作成する
* @param[in]   visualWords     Visual Words
* @return 成功なら0、失敗なら1
*/
int calcHistograms(Mat& visualWords) {
    flann::Index idx(visualWords, flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);
 
    // 各画像のヒストグラムを出力するファイルを開く
    fstream fout;
    fout.open("histograms.txt", ios::out);
    if (!fout.is_open()) {
        cerr << "cannot open file: histograms.txt" << endl;
        return 1;
    }
 
    // IMAGE_DIRの各画像をヒストグラムに変換
    DIR* dp;
    if ((dp = opendir(IMAGE_DIR.c_str())) == NULL) {
        cerr << "cannot open directory: " << IMAGE_DIR << endl;
        return 1;
    }
 
    struct dirent* entry;
    while ((entry = readdir(dp)) != NULL) {
        string fileName = entry->d_name;
        if ( fileName.compare(".") == 0 || fileName.compare("..") == 0) {
            continue;
        }
 
        string filePath = IMAGE_DIR + "/" +  fileName;
 
        // ヒストグラムを初期化
        int* histogram = new int[visualWords.rows];
 
        for (int i = 0; i < visualWords.rows; i++) {
            histogram[i] = 0;
        }
 
        Mat descriptors;
        int ret = extractSURF(filePath, &descriptors);
        if (ret != 0) {
            cerr << "error in extractSURF" << endl;
            return 1;
        }
         int maxResults=1;
        float r = 1.0f;
        for ( int i = 0; i < descriptors.rows; i++ ){
            Mat indices = ( Mat_<int>( 1, 1 ) ); // dataの何行目か
            Mat dists = ( Mat_<float>( 1, 1) ); // それぞれどれだけの距離だったか
            Mat query = ( Mat_<float>(  1,descriptors.cols) );
            //descriptors.col(i).isContinuous()がfalseなので新しく行列queryを作る
            for (int m = 0; m < descriptors.cols; m++ ){
                query.at<float>(0,m) = descriptors.at<float>( i, m );
            }
            idx.radiusSearch(query, indices, dists, r, maxResults);
            histogram[indices.at<int>(0,0)] += 1;
        }
 
        // ヒストグラムをファイルに出力
        if( 0 != descriptors.rows ){
            fout << filePath << "\t";
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
 
int main() {
    initModule_nonfree();
 
    // IMAGE_DIRの各画像から局所特徴量を抽出
    cout << "Load Descriptors ..." << endl;
     Mat featureVectors = ( Mat_<float>( 1, DIM ) );
    cout << featureVectors.rows << "\n";
    //特徴ベクトルを取り出し、学習用DBを作る
    int ret = loadDescriptors(featureVectors);
 
    // 局所特徴量をクラスタリングして各クラスタのセントロイドを計算
    cout << "Clustering ..." << endl;
    Mat labels(featureVectors.rows, 1, CV_64F);        // 各サンプル点が割り当てられたクラスタのラベル
    //Mat centroids(MAX_CLUSTER, DIM, CV_64F);  // 各クラスタの中心（セントロイド） DIM次元ベクトル
    Mat centroids = Mat_<float>( MAX_CLUSTER, DIM );
    kmeans(featureVectors, MAX_CLUSTER, labels, cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 1, KMEANS_PP_CENTERS, centroids);
 
    // 各画像をVisual Wordsのヒストグラムに変換する
    // 各クラスターの中心ベクトル、centroidsがそれぞれVisual Wordsになる
    cout << "Calc Histograms ..." << endl;
    calcHistograms(centroids);
 
    return 0;
}
