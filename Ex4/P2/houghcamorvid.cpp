// Taken from OpenCV example
// Modified by Phil Orlando for Exercise 4
// compiled with g++ -O0 -g -I/usr/local/include/opencv4 houghcamorvid.cpp -o houghcamorvid -L/usr/local/lib `pkg-config --libs opencv4`

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    // Declare the output variables
    
    const char* default_file = "CUdrive.mp4";
    VideoCapture cap;

    // Loads an image

    if(argc >= 2)//read from default
    {
        //read from cam
        cap.open(0);
    }
    else
    {
        //read file
        cap.open(default_file);
    }
    
    // Check if image is loaded fine
    if(!cap.isOpened()){
        printf(" Error opening video or cam\n");
        return -1;
    }

    Mat frame, gray, dst, cdst, cdstP;
    while(true)
    {
        cap >> frame;
        if(frame.empty()) break; //for end of file 

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Edge detection
        Canny(gray, dst, 100, 215, 3); //changed to 100 lower thresh  and 215 upper to reduce lines 
        // Copy edges to the images that will display the results in BGR
        cvtColor(dst, cdst, COLOR_GRAY2BGR);
        cdstP = cdst.clone();
        // Standard Hough Line Transform
        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(dst, lines, 1, CV_PI/180, 200, 0, 0 ); // runs the actual detection
        // increased to 200 to reduce lines
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
        HoughLinesP(dst, linesP, 1, CV_PI/180, 50, 150, 10 ); // runs the actual detection
        // increase min line length 
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        }


        imshow("Source", frame);
        imshow("Detected Lines - Standard Hough Line Transform", cdst);
        imshow("Detected Lines - Probabilistic Line Transform", cdstP);

        int key = waitKey(10);
        if(key == 'q' || key == 27) break;

    }
    return 0;
}
