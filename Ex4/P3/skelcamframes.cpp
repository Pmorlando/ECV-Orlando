// modified from previous examples by Phil orlando and the OpenCV skeletal.cpp file
// compiled with ##


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

int main(int argc, char **argv)
{
    VideoCapture cam0(0);
    char skeletalframe[50];
    Mat frame, gray, binary, mfblur;
    const char* window_name = "vidout";
    int framecnt = 0;

    if (!cam0.isOpened())
    {
        exit(-1);
    }

    
    cam0.set(CAP_PROP_FRAME_WIDTH, 640);
    cam0.set(CAP_PROP_FRAME_HEIGHT, 480);


    namedWindow(window_name, WINDOW_AUTOSIZE);

    while (true)
    {

        cam0.read(frame);
        if(frame.empty()) break;

        cvtColor(frame,gray,COLOR_BGR2GRAY);

        // using 150 for hand

        threshold(gray, binary, 150, 255, THRESH_BINARY);
        binary = 255 - binary;

        medianBlur(binary, mfblur, 1);

        Mat skel(mfblur.size(), CV_8UC1, Scalar(0));
        Mat temp, eroded;
        Mat element = getStructuringElement(MORPH_CROSS, Size(3,3));
        bool done;
        int interations = 0;

        do
        {
            erode(mfblur, eroded, element);
            dilate(eroded, temp, element);
            subtract(mfblur, temp, temp);
            bitwise_or(skel, temp, skel);
            eroded.copyTo(mfblur);

            done = (countNonZero(mfblur) == 0);
            iterations++
        } while (!done && (interations < 100));


        sprintf(skeletalframe, "skelframe%04d.pgm", framecnt);
        imwrite(skeletalframe, skel);

        framecnt++;

        imshow("Cam",frame);
        imshow("Skeletal", skel)

        if(waitKey(10)== ESCAPE_KEY) break;
    }
    return 0;
}
