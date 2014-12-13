/*
 * rcpr.cpp
 *
 *  Created on: 13 Apr 2014
 *      Author: hy300@eecs.qmul.ac.uk
 */




#include <opencv2/opencv.hpp>
#include "RandomFerns.h"
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace cv;
using namespace std;

bool compareRect(cv::Rect r1, cv::Rect r2) { return r1.height < r2.height; }

bool load_annotations(std::vector<std::string>& names, std::vector<cv::Rect>& rects, std::string url) {
  if (boost::filesystem::exists(url.c_str())) {
	std::string filename(url.c_str());
	boost::iostreams::stream < boost::iostreams::file_source > file(
		filename.c_str());
	std::string line;
	names.clear();
	rects.clear();
	while (std::getline(file, line)) {
	  std::vector < std::string > strs;
	  boost::split(strs, line, boost::is_any_of(" "));
	  names.push_back(strs[0]);
	  cv::Rect rect;
	  rect.x = std::atoi(strs[1].c_str());
	  rect.y = std::atoi(strs[2].c_str());
	  rect.width = std::atoi(strs[3].c_str());
	  rect.height = std::atoi(strs[4].c_str());
	  rects.push_back(rect);
	}
	return true;
  }
  return false;
}
void load_trainposes(cv::Mat& train_poses,std::string filename){
	ifstream filereader;
	filereader.open(filename.c_str(),std::ifstream::in);
	train_poses = cv::Mat(300,136,CV_32FC1);
	if (filereader.is_open()){
	for (int k1 = 0; k1< 300; k1++)
		for (int k2 =0; k2< 136; k2++){
			float a = 0;
			filereader >> a;
//			std::cout << a;
			train_poses.at<float>(k1,k2) = a;
		}
	filereader.close();
	}
}
float CalcMedianValue(vector<float> scores)
{
  float median;
  size_t size = scores.size();
  sort(scores.begin(), scores.end());

  if (size  % 2 == 0)
  {
	  median = (scores[size / 2 - 1] + scores[size / 2]) / 2;
  }
  else
  {
	  median = scores[size / 2];
  }

  return median;
}
int main( int argc, char** argv )
{
	cv::Mat trainposes;
	cv::Rect facebb;
	int key = 0;
	int num_inits = 7;
	string winname("RCPR");
	cv::namedWindow(winname,false);
	if (argc > 3)
		num_inits = atoi(argv[3]);
	// std::cout << num_inits << std::endl;
	std::string imgsfilename = argv[2];
	std::string modelname = argv[1];
	//std::string path = argv[2];
	std::string filename = "result_"+imgsfilename;
	ofstream file(filename.c_str());
	std::vector<std::string> names;
	std::vector<cv::Rect> rects;

	load_annotations(names,rects,imgsfilename);
	std::cout << "here" << std::endl;
//	std::string imgname("photos/test.jpg");
	load_trainposes(trainposes,"Model/normalizedpose.txt");
	// string faceDetectionModel("Model/haarcascade_frontalface_alt2.xml");
	RandomFerns ferns;
	std::ifstream ifs(modelname.c_str());
	if (!ifs) {
		std::cout << "Model not found "  << std::endl;
		return false;
	}
	  //load model
	try {
		boost::archive::binary_iarchive ia(ifs);
		ia >> ferns;
	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	} catch (int e) {
		std::cout  << "EXCEPTION " << e << std::endl;

	}
	ifs.close();
	std::cout << ferns.depth << std::endl;
	int framenum = 0;

	for (int k = 0; k < names.size(); k++)
	{
		std::cout << names[k] << std::endl;
		cv::Mat frame = cv::imread(names[k]);
		if(frame.empty())
			break;
		cv::Mat gray;
		cv::Mat gray_float;
		cv::cvtColor(frame,gray,CV_BGR2GRAY);
		framenum ++;

		std::cout << framenum << std::endl;
		if(!frame.empty()){
			gray.convertTo(gray_float,CV_32FC1,1./255.);
			facebb = rects[k];
			cv::rectangle(frame,cv::Point(facebb.x, facebb.y), cv::Point(facebb.x+facebb.width,facebb.y+facebb.height), cv::Scalar(255,0,0));
			std::vector<cv::Mat> inits;
			inits.clear();
			for (int ii = 0; ii< num_inits; ii++)
				inits.push_back(trainposes.row(ii).clone());
			for (int n_i = 0; n_i < num_inits; n_i++)
				ferns.Apply_Ferns(gray_float,facebb,inits[n_i]);
			for (int p = 0; p < 136/2; p++){
				std::vector<float> xs;std::vector<float> ys;
				for (int ii=0; ii< num_inits; ii++){
					xs.push_back(inits[ii].at<float>(0,p));// xs.push_back(inits[1].at<float>(0,p));xs.push_back(inits[2].at<float>(0,p));
					ys.push_back(inits[ii].at<float>(0,p+68));// ys.push_back(inits[1].at<float>(0,p+68));ys.push_back(inits[2].at<float>(0,p+68));
				}
				float x = CalcMedianValue(xs);
				float y = CalcMedianValue(ys);
				file << x <<" " << y << " ";
				cv::circle(frame, cv::Point_<int>(x,y),int(1./150.*facebb.width)+1, cv::Scalar(0, 0, 255, 0),int(1./150.*facebb.width)+1);
			}


			cv::imshow(winname,frame);
			key = cv::waitKey(1);
			file <<std::endl;
		}
	}
	file.close();
	return 0;
}
