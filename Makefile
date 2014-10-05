
OPENCV_PREFIX = /usr/local
BOOST_PREFIX= /usr
INCLUDES = -I$(OPENCV_PREFIX)/include/opencv2 \
           -I$(OPENCV_PREFIX)/include/opencv \
           -I$(BOOST_PREFIX)/include/boost 
LIBS = -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_core \
       -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem
LIBDIRS = -L$(OPENCV_PREFIX)/lib -Wl,-rpath,$(OPENCV_PREFIX)/lib \
          -L$(BOOST_PREFIX)/lib -Wl,-rpath,$(BOOST_PREFIX)/lib \
          -L/usr/lib
CC=g++
CFLAGS=-c -Wall -O3
SOURCES= RandomFerns.cpp rcpr.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=RCPR

all: $(SOURCES) $(EXECUTABLE)
clean: 
	rm -f *.o
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
