// Written by Sam Siewert and modified by Phil Orlando
// for exercise 3
// compiled with g++ -O0 -g -I/usr/local/include/opencv4 houghcam.cpp -o houghcam -L/usr/local/lib `pkg-config --libs opencv4`



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;


// See www.asciitable.com
#define ESCAPE_KEY (27)


int main(int argc, char** argv)
{
    VideoCapture cam0(0);
   namedWindow("video_display");
   char winInput;
   Mat *displayframe;
   
   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);
   
   cam0.read(frame)
    
    // Declare the output variables
    Mat dst, cdst, cdstP;

    // Edge detection
    
    Canny(frame, dst, 80, 240, 3);
    // Copy edges to the images that will display the results in BGR
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdstP = cdst.clone();

    // Standard Hough Line Transform
    vector<Vec2f> lines; // will hold the results of the detection

    //HoughLines(dst, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
    HoughLines(dst, lines, 10, CV_PI/180, 200, 200, 0 ); // runs the actual detection

    // Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
    }
    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI/180, 50, 50, 10 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    // Show results
    imshow("Source", frame);
    //imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
    imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
    // Wait and Exit
    waitKey();
    return 0;
}

int main( int argc, char** argv )
{
   VideoCapture cam0(0);
   namedWindow("video_display");
   char winInput;
   struct timespec curr_t;
   double start, end;
   Mat *displayframe;


   if (!cam0.isOpened())
   {
       exit(SYSTEM_ERROR);
   }

   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);

   namedWindow( window_name, WINDOW_AUTOSIZE );


   while (1)
   {
      clock_gettime(CLOCK_MONOTONIC, &curr_t);
      start = (double)curr_t.tv_sec + ((double)curr_t.tv_nsec) / 1000000000.0;

      cam0.read(frame);

      char fps[20];

      sprintf(fps,  "FPS: %.2f", 1.0/(end-start));
      putText(*displayframe, fps, Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250),1,LINE_AA);
      imshow(window_name, *displayframe);
   }

   destroyWindow("video_display");

}
