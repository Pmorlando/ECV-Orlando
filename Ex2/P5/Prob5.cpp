/*
 *
 *  Example by Sam Siewert 
 *
 *  Created for OpenCV 4.x for Jetson Nano 2g, based upon
 *  https://docs.opencv.org/4.1.1
 *
 *  Tested with JetPack 4.6 which installs OpenCV 4.1.1
 *  (https://developer.nvidia.com/embedded/jetpack)
 *
 *  Based upon earlier simpler-capture examples created
 *  for OpenCV 2.x and 3.x (C and C++ mixed API) which show
 *  how to use OpenCV instead of lower level V4L2 API for the
 *  Linux UVC driver.
 *
 *  Verify your hardware and OS configuration with:
 *  1) lsusb
 *  2) ls -l /dev/video*
 *  3) dmesg | grep UVC
 *
 *  Note that OpenCV 4.x only supports the C++ API
 *  Modified by Phil Orlando for exercise 2
 *  compiled using 
 *  g++ -O0 -g -I/usr/local/include/opencv4 capture.cpp -o capture -L/usr/local/lib `pkg-config --  
 *   libs opencv4`
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

// See www.asciitable.com
#define ESCAPE_KEY (27)
#define SYSTEM_ERROR (-1)

int main()
{
   VideoCapture cam0(0);
   namedWindow("video_display");
   char winInput;

   if (!cam0.isOpened())
   {
       exit(SYSTEM_ERROR);
   }

   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);
   
   struct timespec start, end;
   unsigned int frame_count = 0; // need to count frames to get FPS
   clock_gettime(CLOCK_MONOTONIC, &start);//start before any capture

   while (1)
   {
      Mat frame;
      cam0.read(frame);
      imshow("video_display", frame);
      frame_count++;//increase frames each capture

      if ((winInput = waitKey(1)) == ESCAPE_KEY) // adjusted down to 1ms to get better idea of FPS
      //if ((winInput = waitKey(0)) == ESCAPE_KEY)
      {
          clock_gettime(CLOCK_MONOTONIC, &end);
          break;
      }
      else if(winInput == 'n')
      {
	  cout << "input " << winInput << " ignored" << endl;
      }
      
   }
   
   
   double runtime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1000000000.0;
   
   cout << "Frames: " << frame_count << ", AVG FPS: " <<  (frame_count/runtime) << endl;
   

   destroyWindow("video_display"); 
};
