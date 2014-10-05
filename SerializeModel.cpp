#include <opencv2/opencv.hpp>
#include "RandomFerns.h"
#include <iostream>
#include <fstream>
#include <time.h>
using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
//	cv::Mat trainposes;
//	cv::Rect facebb;
////	int key = 0;
////	string winname("CoolFace");
//	std::string videoname = argv[1];
//    std::string filename = videoname+".txt";
//	ofstream file(filename.c_str());
////	std::string imgname("photos/test.jpg");
//	load_trainposes(trainposes,"Model/normalizedpose.txt");
//	string faceDetectionModel("Model/haarcascade_frontalface_alt2.xml");
    RandomFerns ferns;

    //	  Loading model from text file
      if(!ferns.Load_Ferns("./ferns/ferns_level"))
          std::cout <<"Ferns loading fails" << std::endl;
      std::ofstream ofs("ferns.dat", std::ios::binary);//save to binary file
      boost::archive::binary_oarchive oa(ofs);
      oa << ferns;
    std::ifstream ifs("ferns.dat");
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

}
