/*
 *
 *  Example by Sam Siewert 
 *
 *  Updated 12/6/18 for OpenCV 3.1
 *  Updated for Open CV 4
 *  by Phil Orlando
 * compiled with g++ -O0 -g -I/usr/local/include/opencv4 diffinteract.cpp -o diffinteract -L/usr/local/lib `pkg-config --libs opencv4`
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

char difftext[20];

int main( int argc, char** argv )
{
    Mat mat_frame, mat_gray, mat_diff, mat_gray_prev;
    VideoCapture cam0(0);
    unsigned int diffsum, maxdiff;
    double percent_diff;


    //open the video stream and make sure it's opened
    // "0" is the default video device which is normally the built-in webcam
    if(!cam0.open(0)) 
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }
    else
    {
	   cout << "Opened default camera interface" << endl;
    }

    while(!cam0.read(mat_frame)) {
	cout << "No frame" << endl;
	waitKey(33);
    }
	
    cvtColor(mat_frame, mat_gray, COLOR_BGR2GRAY);

    mat_diff = mat_gray.clone();
    mat_gray_prev = mat_gray.clone();

    maxdiff = (mat_diff.cols)*(mat_diff.rows)*255;

    while(1)
    {
	if(!cam0.read(mat_frame)) {
		cout << "No frame" << endl;
		waitKey();
	}
	
	cvtColor(mat_frame, mat_gray, COLOR_BGR2GRAY);

	absdiff(mat_gray_prev, mat_gray, mat_diff);

	// worst case sum is resolution * 255
	diffsum = (unsigned int)cv::sum(mat_diff)[0]; // single channel sum

	percent_diff = ((double)diffsum / (double)maxdiff)*100.0;

        printf("percent diff=%lf\n", percent_diff);
        sprintf(difftext, "%8d",  diffsum);

	if(percent_diff > 0.5) putText(mat_diff, difftext, Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1, LINE_AA);

	imshow("Gray Example", mat_gray);
	imshow("Gray Previous", mat_gray_prev);
	imshow("Gray Diff", mat_diff);


        char c = waitKey(33); // take this out or reduce
        if( c == 'q' ) break;

	mat_gray_prev = mat_gray.clone();
    }

};
