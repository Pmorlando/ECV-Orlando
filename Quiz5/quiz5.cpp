// Written by Phil Orlando 
/*
OpenCV imread the bears.ppm

cvtColor of bears to Grayscale using COLOR_BGR2gray

resize the image to 320 x 240 

imwrite to bears320.pgm

2 psuedo code

imread the bears320.pgm file

resize to 640x480

imwrite to bears640.pgm

3 psuedo code

imread bears640.pgm

resize bears640.pgm to 320x240

imwrite bears320down.pgm
*/
// compiled with g++ -O0 -g -fopenmp -I/usr/local/include/opencv4 quiz5.cpp -o quiz5 -L/usr/local/lib `pkg-config --libs opencv4`
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


int main()
{
    // 1
    Mat orig = imread("Bears.ppm");
    Mat grayorig;
    Mat bears320;
    cvtColor(orig, grayorig, COLOR_BGR2GRAY);
    resize(grayorig, bears320, Size(320,240), 0, 0, INTER_LINEAR);

    imwrite("bears320.pgm", bears320);
    imshow("bears320", bears320);
    // 2 
    Mat bigbears;

    resize(bears320,bigbears,Size(640,480), 0, 0, INTER_LINEAR);

    imwrite("bears640.pgm", bigbears);
    imshow("Upscale bears", bigbears);

    // 3 

    Mat bears320down;
    resize(bigbears, bears320down, Size(320,240), 0, 0, INTER_LINEAR);

    imwrite("bears320down.pgm", bears320down);
    imshow("bears downscaled", bears320down);

    // compare
    double maxdiff = (bears320.cols)*(bears320.rows)*255;

    Mat bearsdiff;
    absdiff(bears320,bears320down, bearsdiff);

    double diffsum = (unsigned int)sum(bearsdiff)[0]; // single channel sum

    double percent_diff = ((double)diffsum / (double)maxdiff)*100.0;

    printf("percent diff=%lf\n", percent_diff);

    imshow("bears 320 difference", bearsdiff);

    waitKey(0);

}
