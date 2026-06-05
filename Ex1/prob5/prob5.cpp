// created from Github ECV-ECEE-5763/computer_vision_cv4_tested/simpler-capture-4/capture.cpp from Sam Siewert and modified by Phil Orlando
// Used Claude to assist in creating the Makefile to actually find my libraries which i could not get to compile 
// cv.line() info and examples taken from https://docs.opencv.org/4.13.0/dc/da5/tutorial_py_drawing_functions.html#autotoc_md1293

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp> // needed for line according to opencv docs

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
   int width = 320; // width and height set to meet problem specs
   int height = 240;
   cam0.set(CAP_PROP_FRAME_WIDTH, width); 
   cam0.set(CAP_PROP_FRAME_HEIGHT, height); 

   while (1)
   {
      Mat frame;
      cam0.read(frame);
      // adding 2 lines for crosshair
      line(frame, Point(width/2, 0), Point(width/2, height), Scalar(0,255,255), 1);
      line(frame, Point(0, height/2), Point(width, height/2), Scalar(0,255,255), 1);
      // Draw 4 pixel thick rectangle for border
      rectangle(frame,Point(0,0), Point(width, height), Scalar(0,0,255), 4); //went with red for border
      // after this commment is unchanged from Sam Siewert Github file noted above
      imshow("video_display", frame);

      if ((winInput = waitKey(10)) == ESCAPE_KEY)
      //if ((winInput = waitKey(0)) == ESCAPE_KEY)
      {
          break;
      }
      else if(winInput == 'n')
      {
	  cout << "input " << winInput << " ignored" << endl;
      }
      
   }

   destroyWindow("video_display"); 
};
