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

#define ESCAPE_KEY (27)

using namespace cv;
using namespace std;

char difftext[20];

int main( int argc, char** argv )
{
    Mat mat_frame, mat_diff, mat_frame_prev, mat_small, mat_diff_small;
    Size display_size(640, 480);
    VideoCapture cam0("Dark-Room-Laser-Spot-with-Clutter.mpeg");
    unsigned int diffsum, maxdiff;
    double percent_diff;

    if(!cam0.open("Dark-Room-Laser-Spot-with-Clutter.mpeg")) 
    {
        cout << "Error opening file" << endl;
        return -1;
    }
    else
    {
	   cout << "Opened file" << endl;
    }


    while(!cam0.read(mat_frame)) {
	cout << "No frame" << endl;
	waitKey(33);
	return -1;
    }
    resize(mat_frame, mat_small, display_size); 

    mat_diff = mat_small.clone();
    mat_frame_prev = mat_small.clone();

    maxdiff = (mat_diff.cols)*(mat_diff.rows)*255;

    while(1)
    {
	if(!cam0.read(mat_frame)) {
		cout << "No frame" << endl;
		waitKey();
		break;
	}
	resize(mat_frame, mat_small, display_size);

	absdiff(mat_frame_prev, mat_small, mat_diff);

	// worst case sum is resolution * 255
	diffsum = (unsigned int)cv::sum(mat_diff)[0]; // single channel sum

	percent_diff = ((double)diffsum / (double)maxdiff)*100.0;

        printf("percent diff=%lf\n", percent_diff);
        sprintf(difftext, "%8d",  diffsum);

	if(percent_diff > 0.5) putText(mat_diff, difftext, Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1, LINE_AA);

	imshow("Example", mat_small);
	imshow("Previous", mat_frame_prev);
	imshow("Diff", mat_diff);


        if(waitKey(10) == ESCAPE_KEY) break;

	mat_frame_prev = mat_small.clone();
    }

};
