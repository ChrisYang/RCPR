/*
 * RandomFerns.h
 *
 *  Created on: 15 Mar 2014
 *      Author: hy
 */
#include "opencv2/opencv.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "opencvseralization.hpp"

#ifndef RANDOMFERNS_H_
#define RANDOMFERNS_H_

struct l1l2f1{//define one point on a line given two points and the ratio
	int id1;
	int id2;
	float t1;
	friend class boost::serialization::access;
	  template<class Archive>
	  void serialize(Archive & ar, const unsigned int version) {
		  ar & id1;
		  ar & id2;
		  ar & t1;
	  }
};
struct fern{
	std::vector<l1l2f1> fernfuncs;
	cv::Mat leafVecs;
	friend class boost::serialization::access;
	  template<class Archive>
	  void serialize(Archive & ar, const unsigned int version) {
		  ar & fernfuncs;
		  ar & leafVecs;
	  }
};

struct fernlevel{
	int num_ferns;
	int num_locs;
	std::vector<l1l2f1> line_pids;
	std::vector<fern> ferns;
	friend class boost::serialization::access;
	  template<class Archive>
	  void serialize(Archive & ar, const unsigned int version) {
		  ar & num_ferns;
		  ar & num_locs;
		  ar & line_pids;
		  ar & ferns;
	  }
};
class RandomFerns {
public:
	RandomFerns();
	bool Load_Ferns(std::string filename);
	bool Apply_Ferns(cv::Mat img, cv::Rect bbox, cv::Mat& init_pose);
	void get_linepoint(cv::Mat img,cv::Mat init_pose,std::vector<l1l2f1> line_pids, cv::Mat& locs);
	virtual ~RandomFerns();

public:
	int depth;
	int num_leafs;
	int posdim;
	int num_levels;
	int num_ferns_pl;
	int num_fea_locs; // for each image, 400 locations
	std::vector<fernlevel> allferns;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & depth;
		ar & num_leafs;
		ar & posdim;
		ar & num_levels;
		ar & num_ferns_pl;
		ar & num_fea_locs;
		ar & allferns;
	}
};

#endif /* RANDOMFERNS_H_ */
