#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <utility>
#include <functional>

using namespace std;

int main(){
	ifstream file("histograms.txt");//ヒストグラムを読み込む
	ifstream file2("hikaku.txt");//比較用ヒストグラムを読み込む
	ofstream result("result.txt");
	vector<vector<string> > values;//ヒストグラム格納
	vector<vector<string> > values2;//比較用ヒストグラム格納
	
	double sum[1000];
	double w=0,x=0,y=0,z=0;
	string str,str2;
	int p;

	if (file.fail()){
		cerr << "failed." << endl;
		exit(0);
	}
	if (file2.fail()){
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
	while (getline(file2, str2)){
		//コメント箇所は除く
		if ((p = str2.find("//")) != str2.npos) continue;
		vector<string> inner2;

		//tabがあるかを探し、そこまでをvalues2に格納
		while ((p = str2.find("\t")) != str2.npos){
			inner2.push_back(str2.substr(0, p));

			//str2の中身は"\t"を飛ばす
			str2 = str2.substr(p + 1);
		}
		inner2.push_back(str2);
		values2.push_back(inner2);
	}

	int n = values.size();
	vector<pair<double, string> > pairs(n);//並び替え用のペア作成

	for (unsigned int i = 0; i < values.size(); ++i){
		for (unsigned int j = 0; j < values[i].size(); ++j){
			//stringからdoubleにキャスト変換
			istringstream is;
			istringstream is2;
			is.str(values[i][j]);
			is2.str(values2[0][j]);
			is >> w;
			is2 >> y;
			x += w;//valuesのトータル
			double m=0;
			if ((w - y)>0){
				m = y;
			}
			else{
				m = w;
			}
			z += m;//学習データと比較用データの内小さい方の合計
//			cout << values[i][0] << endl;
//			 int x = (int)values[i][j];
//			cout << values[i][j] << "\t";
		}
		sum[i] = z / x;
		cout << sum[i] << endl;
		//ペアに値を格納
		pairs[i] = make_pair(sum[i], values[i][0]);

		z = 0;
		x = 0;
	}

	//ソート
	stable_sort(pairs.begin(), pairs.end(), greater<pair<double, string> >());
	for (int k = 0; k < values.size(); k++){
		result << pairs[k].first << "\t" << pairs[k].second << endl;
	}
	//	cout << y << endl;
	return 0;
}
