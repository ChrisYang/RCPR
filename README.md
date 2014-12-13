RCPR
====

C++ code for Robust Cascaded Pose Regression using Random Ferns 


The original RCPR code was written in matlab which limits its run-time speed performance. I transformed the testing code into C++ and using a python interface. 

Requirements:
--OpenCV 2.4
--boost 1.55 
--Tested on Ubuntu 12.04 

------------------For creating the model file-------------------------------------

Dowload the matlab code from here: http://www.vision.caltech.edu/xpburgos/ICCV13/
First, I transform the trained model [regModel.mat] into many .txt file (might be not optimal but it works) and store them in a directory called ferns. A cpp program is used to serialize the txt files into a fern class object file, called fern.dat, which is the model file. You can directly use the fern.dat and ignore the following steps. 

1. load regModel

2. mkdir ferns

3. run save_regmodel in matlab (it takes a few minutes) 

4. make -f Makefile_Serialize 

5. $./RCPRSerializer 

-----------------------------------------------------------------------------------

-----------------------------------------------------------------------------------
For running the model. 

$ make

RCPR will be created 

Then run 

$./RCRP Model/fern.dat yourfilelist.txt 
e.g.: ./RCPR Model/fern.dat img.txt 
it will save the result to result_img.txt when it finishes

