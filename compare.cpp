#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>

using namespace std;

int main(){
	ifstream file("histograms.txt");//�q�X�g�O������ǂݍ���
	ifstream file2("hikaku.txt");//��r�p�q�X�g�O������ǂݍ���
	ofstream result("result.txt");
	vector<vector<string>> values;//�q�X�g�O�����i�[
	vector<vector<string>> values2;//��r�p�q�X�g�O�����i�[
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
	while (getline(file2, str2)){
		//�R�����g�ӏ��͏���
		if ((p = str2.find("//")) != str2.npos) continue;
		vector<string> inner2;

		//tab�����邩��T���A�����܂ł�values2�Ɋi�[
		while ((p = str2.find("\t")) != str2.npos){
			inner2.push_back(str2.substr(0, p));

			//str2�̒��g��"\t"���΂�
			str2 = str2.substr(p + 1);
		}
		inner2.push_back(str2);
		values2.push_back(inner2);
	}
	for (unsigned int i = 0; i < values.size(); ++i){
		for (unsigned int j = 0; j < values[i].size(); ++j){
			//string����double�ɃL���X�g�ϊ�
			istringstream is;
			istringstream is2;
			is.str(values[i][j]);
			is2.str(values2[0][j]);
			is >> w;
			is2 >> y;
			x += w;//values�̃g�[�^��
			double m=0;
			if ((w - y)>0){
				m = y;
			}
			else{
				m = w;
			}
			z += m;//�w�K�f�[�^�Ɣ�r�p�f�[�^�̓����������̍��v
//			cout << values[i][0] << endl;
//			 int x = (int)values[i][j];
//			cout << values[i][j] << "\t";
		}
		sum[i] = z / x;
		cout << sum[i] << endl;
		z = 0;
		x = 0;
	}
	stable_sort(&sum[0], &sum[values.size()]);
	for (int k = 0; k < values.size(); k++){
		result << values[k][0] <<"\t"<< sum[k] << endl;
	}
	cout << y << endl;
	return 0;
}