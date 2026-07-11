// modified from Brighten.cpp by Phil Orlando for Quiz 3

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <pthread.h>
#define NUM_THREADS 4

using namespace cv; using namespace std;
double alpha=2.0;  int beta=50;  /* contrast and brightness control */

typedef struct 
{
    Mat *src;
    Mat *done;
    int startrow;
    int endrow;
    int thread_id;
}threadParams_t;

pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

void *brighten(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;



    for( int y = threadParams-> startrow; y < threadParams->endrow; y++ ) //begininng to end row
    {
        for( int x = 0; x < threadParams->src->cols; x++ ) // cols 
        { 
            for( int c = 0; c < 3; c++ ) // each color channel
                threadParams->done->at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( threadParams->src->at<Vec3b>(y,x)[c] ) + beta );
        }
    }
    return NULL;
}

// splitting images by row to do this. 
int main( int argc, char** argv )
{

    Mat image = imread( argv[1] ); // read in image file
    Mat dst = image.clone();

    int rowsperthread = image.rows / NUM_THREADS;


    for(int i=0; i<NUM_THREADS; i++)
    {
       threadParams[i].src = &image;
       threadParams[i].done = &dst;
       threadParams[i].startrow = i * rowsperthread; // starter row
       threadParams[i].endrow = (i+1) * rowsperthread; // 1 less then next starter row 
       threadParams[i].thread_id = i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      brighten, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
    }

    for(int i = 0; i<NUM_THREADS;i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    namedWindow("Original Image", 1); namedWindow("New Image", 1);
    imshow("Original Image", image); imshow("New Image", dst);
    waitKey(); return 0;
}

