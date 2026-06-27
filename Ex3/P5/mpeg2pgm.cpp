/* Written by Phil Orlando for exercise 3 
compiled with g++ -O0 -g -I/usr/local/include/opencv4 mpeg2pgm.cpp -o mpeg2pgm -L/usr/local/lib `pkg-config --libs opencv4`
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


/* mpeg video extract red, 
save frames to .pgm
ffmpeg reencode .pgm into mpeg4 */

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
    Mat mat_frame;
    VideoCapture cam0;
    char laserframe[50];
    
    
    if(!cam0.open("Dark-Room-Laser-Spot.mpeg")) // open video file
    {
        cout << "Error opening file" << endl;
        return -1;
    }
    else
    {
	cout << "Opened file" << endl;
    }
    
    if(!cam0.read(mat_frame)) { // read frame from video
	cout << "No frame" << endl;
	return -1;
    }
    
    int framecnt = 0;
    
    while(1)
    {
    	
    	vector<Mat> channels;
    	split(mat_frame, channels);//split to each color channel B R G
    	
    	sprintf(laserframe, "laserframe%04d.pgm", framecnt);// naming string for each frame
    
    	imwrite(laserframe, channels[2]);//save each frame with name string
    	
    	framecnt++;
    	
    	if(!cam0.read(mat_frame))
    	{
    	    cout << "No Frame" << endl;
    	    break;// break out if no more frames
    	}
    	
    }
    cout << "Save " << framecnt << " frames" << endl;
    return 0;
    	   
}

