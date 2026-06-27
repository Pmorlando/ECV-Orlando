/* Written by Phil Orlando for exercise 3 
compiled with g++ -O0 -g -I/usr/local/include/opencv4 threshtrack.cpp -o threshtrack -L/usr/local/lib `pkg-config --libs opencv4`
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
    Mat mat_frame, big_frame;
    VideoCapture cam0;
    char laserframe[50];
    
    
    if(!cam0.open("encodelaser.mpeg")) // open video file
    {
        cout << "Error opening file" << endl;
        return -1;
    }
    else
    {
	cout << "Opened file" << endl;
    }
    
    if(!cam0.read(big_frame)) { // read frame from video
	cout << "No frame" << endl;
	return -1;
    }
    
    int framecnt = 0;
    
    resize(big_frame, mat_frame, Size(640, 480); 
    
    while(1)
    {
    	
    	int xpix = mat_frame.cols;
    	int ypix = mat_frame.rows; 
    	int i = 0;
    	int j = 0;
    	int xmin = mat_frame.cols;
    	int xmax = 0;
    	int ymin = mat_frame.rows;
    	int ymax = 0;
    	char trackdot[50];
    	
    	for(int i = 0, i < mat_frame.cols, i++)
    	{
    	    for(int j = 0, j < mat_frame.rows, j++)
    	    {
    	    	//if pixel at i,j <60 make 0, else find the left and right most pixel idx with any value??
    	    	if(mat_frame.at<uchar>(i,j) < 60)
    	    	{
    	    	    mat_frame.at<uchar>(i,j) = 0;
    	    	}   	    	
    	    }
    	}
    	
    	   	
    	// find extent of the dot
    	for(int i = 0, i < mat_frame.cols, i++)
    	{
    	    for(int j = 0, j < mat_frame.rows, j++)
    	    {
    	    	if(mat_frame.at<uchar>(i,j) >0)
    	    	{
    	    	    if(i < xmin) xmin = i;//starts as right most pixel and will find minimum
    	    	    if(i > xmax) xmax = i;// reverse
    	    	    if(j < ymin) ymin = j;// same for y
    	    	    if(j > ymax) ymax = j;// reverse same
    	    	}
    	    }
    	}
    	int xbar = (xmax - xmin)/2;
    	int ybar = (ymax - ymin)/2;
    	
    	
    	// crosshair drawing
    	if(xmax>xmin && ymax>ymin)
    	{
    	    line(mat_frame, Point(xbar,0), Point(xbar, mat_frame.rows), Scalar(255),1);
    	    line(mat_frame, Point(0, ybar), Point(mat_frame.cols, ybar), Scalar(255),1);
    	}
    	else
    	{
    	    cout << "Frame " << framecnt << " has no dot" << endl;
    	}
    	
    	
    	 //save to images, will ffmpeg to video again
    	
    	
    	sprintf(trackdot, "trackedot%04d.pgm", framecnt);// naming string for each frame
    
    	imwrite(trackdot, mat_frame);//save each frame with name string
    	
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
