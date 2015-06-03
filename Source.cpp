#include <stdio.h>  
#include <iostream>  
#include "opencv2/core/core.hpp"  
#include "opencv2/nonfree/features2d.hpp"   //SurfFeatureDetector实际在该头文件中  
//#include "opencv2/legacy/legacy.hpp"  //BruteForceMatcher实际在该头文件中  
#include "opencv2/features2d/features2d.hpp"    //FlannBasedMatcher实际在该头文件中  
#include "opencv2/highgui/highgui.hpp"  
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat src_1 = imread("0003.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat src_2 = imread("0004.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	if (!src_1.data || !src_2.data)
	{
		cout << " --(!) Error reading images " << endl;
		return -1;	
	}

	//-- Step 1: 使用SURF算子检测特征点  
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	vector<KeyPoint> keypoints_1, keypoints_2;
	detector.detect(src_1, keypoints_1);
	detector.detect(src_2, keypoints_2);
	cout << "img1--number of keypoints: " << keypoints_1.size() << endl;
	cout << "img2--number of keypoints: " << keypoints_2.size() << endl;

	//-- Step 2: 使用SURF算子提取特征（计算特征向量）  
	SurfDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(src_1, keypoints_1, descriptors_1);
	extractor.compute(src_2, keypoints_2, descriptors_2);

	//-- Step 3: 使用FLANN法进行匹配  
	FlannBasedMatcher matcher;
	vector< DMatch > allMatches;
	matcher.match(descriptors_1, descriptors_2, allMatches);
	cout << "number of matches before filtering: " << allMatches.size() << endl;

	//-- 计算关键点间的最大最小距离  
	double maxDist = 0;
	double minDist = 100;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = allMatches[i].distance;
		if (dist < minDist)
			minDist = dist;
		if (dist > maxDist)
			maxDist = dist;
	}
	printf("    max dist : %f \n", maxDist);
	printf("    min dist : %f \n", minDist);

	//-- 过滤匹配点，保留好的匹配点（这里采用的标准：distance<2*minDist）  
	vector< DMatch > goodMatches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (allMatches[i].distance < 2 * minDist)
			goodMatches.push_back(allMatches[i]);
	}
	cout << "number of matches after filtering: " << goodMatches.size() << endl;

	//-- 显示匹配结果  
	Mat matchImg;
	drawMatches(src_1, keypoints_1, src_2, keypoints_2,
		goodMatches, matchImg, Scalar::all(-1), Scalar::all(-1),
		vector<char>(),
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS //不显示未匹配的点  
		);
	imshow("matching result", matchImg);
	//-- 输出匹配点的对应关系  
	for (int i = 0; i < goodMatches.size(); i++)
		printf("   good match %d: keypoints_1 [%d]  -- keypoints_2 [%d]\n", i,
		goodMatches[i].queryIdx, goodMatches[i].trainIdx);

	waitKey(0);
	return 0;
}
