/*
 * RandomFerns.cpp
 *
 *  Created on: 15 Mar 2014
 *      Author: hy
 */

#include "RandomFerns.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace cv;
using namespace std;
void reprojectpose(cv::Mat pose1, cv::Mat& pose2, cv::Rect bbox){
	float sx = bbox.width/2.0;
	float sy = bbox.height/2.0;
	float ctrx = bbox.x + sx;
	float ctry = bbox.y + sy;
	pose2 = pose1.clone();
	int nc = pose1.cols;
	pose2.colRange(0,nc/2) *= sx;
	pose2.colRange(0,nc/2) += ctrx;
	pose2.colRange(nc/2,nc) *= sy;
	pose2.colRange(nc/2,nc) += ctry;
}
void projectpose(cv::Mat pose1, cv::Mat& pose2, cv::Rect bbox){
	float sx = bbox.width/2.0;
	float sy = bbox.height/2.0;
	float ctrx = bbox.x + sx;
	float ctry = bbox.y + sy;
	pose2 = pose1.clone();
	int nc = pose1.cols;
	pose2.colRange(0,nc/2) -= ctrx;
	pose2.colRange(0,nc/2) /= sx;
	pose2.colRange(nc/2,nc) -= ctry;
	pose2.colRange(nc/2,nc) /= sy;
}
int calculate_fernid(cv::Mat feats,std::vector<l1l2f1> fernfuncs){
	int id = 0;
	for(int k = 0; k< fernfuncs.size(); k++){
		id *= 2;
		float dif = feats.at<float>(fernfuncs[k].id1-1,0) - feats.at<float>(fernfuncs[k].id2-1,0);
		if(dif < fernfuncs[k].t1)
			id += 1;
	}
	return id;
}
RandomFerns::RandomFerns() {
	// TODO Auto-generated constructor stub
	depth = 5;
	num_leafs = 32;
	posdim = 136;
	num_levels = 100;
	num_ferns_pl = 50;
	num_fea_locs = 400;
	allferns.resize(num_levels);
}

bool RandomFerns::Load_Ferns(std::string filename) {

	for (unsigned int k = 0; k < num_levels; k++){
		std::cout << k << std::endl;
		allferns[k].ferns.resize(num_ferns_pl);
		allferns[k].line_pids.resize(num_fea_locs);
		char c[10];
		sprintf(c,"%d",k);
		std::string file_name = filename +c+".txt";
		ifstream filereader;
		filereader.open(file_name.c_str());
		for (unsigned int f =0; f < num_fea_locs; f++){
			filereader >> allferns[k].line_pids[f].id1;
			filereader >> allferns[k].line_pids[f].id2;
			filereader >> allferns[k].line_pids[f].t1;
		}
		for (unsigned int n=0; n < num_ferns_pl; n++){
			allferns[k].ferns[n].fernfuncs.resize(depth);
			allferns[k].ferns[n].leafVecs = cv::Mat(num_leafs,posdim,CV_32FC1);
			for (unsigned int r = 0; r < num_leafs; r++)
				for (unsigned int c =0; c < posdim; c++){
					filereader >> allferns[k].ferns[n].leafVecs.at<float>(r,c);
				}
			for(unsigned int d = 0; d < depth; d++){
				filereader >> allferns[k].ferns[n].fernfuncs[d].id1;
				filereader >> allferns[k].ferns[n].fernfuncs[d].id2;
				filereader >> allferns[k].ferns[n].fernfuncs[d].t1;
			}
		}
		filereader.close();
	}
	return true;
}

bool RandomFerns::Apply_Ferns(cv::Mat img,cv::Rect bbox, cv::Mat& init_pose){
	//convert image into double with range [0,1]
	cv::Mat initpose2;
	reprojectpose(init_pose,initpose2,bbox);
//	cv::Mat temp;
//	img.convertTo(temp,CV_8UC1,255.);
//	for (int p = 0; p < 136/2; p++){
//		int x = init_pose.at<float>(0,p);
//		int y = init_pose.at<float>(0,p+68);
//		cv::circle(temp, cv::Point_<int>(x,y),2, cv::Scalar(0, 0, 255, 0),2);
//	}
//	cv::imshow("tmp",temp);
//	cv::waitKey(0);
//	cv:: destroyAllWindows();
	for (int t = 0; t < allferns.size(); t++){
		cv::Mat feats;
	    get_linepoint(img,initpose2,allferns[t].line_pids,feats);
		cv::Mat pDel = cv::Mat(1,posdim,CV_32FC1);
		pDel.setTo(0.0);
		cv::Mat posetmp;
		projectpose(initpose2, posetmp,bbox);
		for (int k = 0; k < allferns[t].ferns.size(); k++){
			int id = calculate_fernid(feats,allferns[t].ferns[k].fernfuncs);
			pDel += allferns[t].ferns[k].leafVecs.row(id);
		}
		posetmp += pDel;
		reprojectpose(posetmp,initpose2,bbox);
		posetmp.release();
	}
	init_pose = initpose2.clone();

}
void RandomFerns::get_linepoint(cv::Mat img,cv::Mat init_pose,std::vector<l1l2f1> line_pids, cv::Mat& feats){
	int cs = init_pose.cols;
	int w = img.cols;
	int h = img.rows;
	assert(cs==posdim);
	assert(line_pids.size()==num_fea_locs);
	feats = cv::Mat(num_fea_locs,1,CV_32FC1);
	cv::Mat xs = init_pose.colRange(0,cs/2).clone();
	cv::Mat ys = init_pose.colRange(cs/2,cs).clone();
	float mx = cv::mean(xs).val[0];
	float my = cv::mean(ys).val[0];
	xs -= mx;
	ys -= my;
	for (int k =0; k < line_pids.size(); k++){
		float p1x = xs.at<float>(0,line_pids[k].id1-1);
		float p1y = ys.at<float>(0,line_pids[k].id1-1);
		float p2x = xs.at<float>(0,line_pids[k].id2-1);
		float p2y = ys.at<float>(0,line_pids[k].id2-1);
		float a = (p2y-p1y)/(p2x-p1x);
		float b = p1y - a*p1x;
		float distx = (p2x-p1x)/2;
		float ctrx = p1x + distx;
		float cs1 = ctrx+line_pids[k].t1*distx;
		float rs1 = a*cs1+b;
		int cs = (int)(cs1+mx);
		int rs = (int)(rs1+my);
//		std::cout<< rs  << " " << cs << std::endl;
		feats.at<float>(k,0) = img.at<float>(max(0,min(h-1,rs)),max(0,min(w-1,cs)));
//		std::cout << feats.at<float>(k,0) << std::endl;
	}

}


RandomFerns::~RandomFerns() {
	// TODO Auto-generated destructor stub
}

