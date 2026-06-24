// Written by Sam Siewert and from OpenCV documentation then modified by Phil Orlando
// for exercise 3
// compiled with g++ -O0 -g -I/usr/local/include/opencv4 houghcirccam.cpp -o houghcirccam -L/usr/local/lib `pkg-config --libs opencv4`

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
    Mat frame, gray;
    const char* window_name = "Huff Circ"

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

        // notation and syntax taken from OpenCV samples/cpp/tutorial_code/ImgTrans/houghcircles.cpp
        cvtColor(frame, gray, COLOR_GRAY2BGR);

        medianBlur(gray, gray, 5);

        vector<Vec3f> circles; 

        
        HoughCircles(gray, circles, HOUGH_GRADIENT, 1, 
            gray.rows/16, // change to detect circles with diff distance to eachother
            100, 30, 1, 30); // change last 2 for min and max circle radius
    

        // Draw the circles
        for (size_t i = 0; i < circles.size(); i++)
        {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // circle center
            circle(frame, center, 1, Scalar(0,100,100), 3, LINE_AA);

            // circle outline 
            circle(frame, center, radius, Scalar(255,0,255), 3, LINE_AA);
        }

        // Show results

        imshow(window_name, frame);
        // Wait and Exit
        if(waitKey(10)== ESCAPE_KEY) break;

    }
    return 0;
}
