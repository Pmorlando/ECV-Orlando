// Written by Sam Siewert and modified by Phil Orlando
// from Cannycam.cpp
// compiled using 
// g++ -O0 -g -I/usr/local/include/opencv4 Prob5.cpp -o Prob5 -L/usr/local/lib `pkg-config --
// libs opencv4`


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;


// See www.asciitable.com
#define ESCAPE_KEY (27)
#define Key_Canny (99)
#define Key_Sobel (115)
#define Key_None (110)
#define SYSTEM_ERROR (-1)

int mode = 0;

Mat canny_frame, timg_gray, timg_grad;
Mat frame;
// Canny 
const int lowThreshold = 50;
const int highThreshold = 150; //fixed threshold values 
const int kernel_size = 3;
const char* window_name = "Edge Map";

// Sobel 
int ksize = 1;
int scale = 3;
int delta = 0;
int ddepth = CV_16S;
Mat sobel_frame, grad;
Mat grad_x, grad_y;
Mat abs_grad_x, abs_grad_y;



void CannyThreshold()
{
    cvtColor(frame, timg_gray, COLOR_BGR2GRAY);

    /// Reduce noise with a kernel 3x3
    blur( timg_gray, canny_frame, Size(3,3) );

    /// Canny detector
    Canny( canny_frame, canny_frame, lowThreshold, highThreshold, kernel_size );

    /// Using Canny's output as a mask, we display our result
    timg_grad = Scalar::all(0);

    frame.copyTo( timg_grad, canny_frame);

    //imshow( window_name, timg_grad);

}

void Sobeledge()
{
    cvtColor(frame, timg_gray, COLOR_BGR2GRAY);

    /// Reduce noise with a kernel 3x3
    blur( timg_gray, sobel_frame, Size(3,3) );

    Sobel(sobel_frame, grad_x, ddepth, 1, 0, ksize, scale, delta, BORDER_DEFAULT);
    Sobel(sobel_frame, grad_y, ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
    //imshow(window_name, grad);
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
      
      if(mode == 1)// canny mode for when c is pressed switches to canny edge until another key is pressed
      {
        CannyThreshold();
        displayframe = &timg_grad;
      }
      else if(mode == 2)
      {
        Sobeledge();
        displayframe = &grad; 
      }
      else
      {
        displayframe = &frame;
      }


      if ((winInput = waitKey(1)) == ESCAPE_KEY)
      {
          break;
      }
      else if(winInput == Key_None)
      {
        mode = 0;
        cout << "No processing to image" << endl;
      }
      else if(winInput == Key_Canny)
      {
        cout << "Canny Edge Detection" << endl;
        mode = 1;

      }
      else if(winInput == Key_Sobel)
      {
        cout << "Sobel Edge Detection" << endl;
        mode = 2;
      }

      clock_gettime(CLOCK_MONOTONIC, &curr_t);
      end = (double)curr_t.tv_sec + ((double)curr_t.tv_nsec) / 1000000000.0;
      printf("Process time=%lf msec, fps=%lf\n", (end-start)*1000.0, 1.0/(end-start));

      char fps[20];

      sprintf(fps,  "FPS: %.2f", 1.0/(end-start));
      putText(*displayframe, fps, Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250),1,LINE_AA);
      imshow(window_name, *displayframe);
   }

   destroyWindow("video_display");

}
