// Modified from rgb2siftcam by Phil Orlando for exercise 4
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 orbtry.cpp -o orbtry -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>


using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
    Mat book = imread("book.pgm", IMREAD_COLOR);
    Mat scene = imread("scene.pgm", IMREAD_COLOR);

    if(book.empty() || scene.empty())
    {
        printf("error loading stuff");
        return -1;
    }
    
    Mat graybook, grayscene;
    cvtColor(book, graybook, COLOR_BGR2GRAY);
    cvtColor(scene, grayscene, COLOR_BGR2GRAY);

    Ptr<ORB> orb = ORB::create(2000);

    vector<KeyPoint> kpsBook, kpsScene;
    Mat desBook, desScene;
    orb->detectAndCompute(graybook, noArray(), kpsBook, desBook);
    orb->detectAndCompute(grayscene, noArray(), kpsScene, desScene);

    BFMatcher matcher(NORM_HAMMING, false);
    vector<vector<DMatch>> knn;
    matcher.knnMatch(desBook, desScene, knn, 2);

    vector<DMatch> good;
    for(const auto& m : knn)
    {
        if (m.size() == 2 && m[0].distance < 0.75f * m[1].distance)
        {
            good.push_back(m[0]);
        }
    }

    Mat imgmatch;
    for(good.size() >= 4)
    {
        vector<Point2f> ptsBook, ptsScene;
        for(const auto& a : good)
        {
            ptsBook.push_back(kpsBook[mqueryIdx].pt);
            ptsScene.push_back(kpsScene[mqueryIdx].pt);
        }
        Mat H = findHomography(ptsBook, ptsScene, RANSAC);

        vector<Points2f> corners(4). sceneCorners(4);
        corners[0] = Point2f(0, 0);
        corners[1] = Point2f((float)book.cols, 0);
        corners[2] = Point2f((float)book.cols, (float)book.rows);
        corners[3] = Point2f(0, (float)book.rows);
        perspectiveTransform(corners, sceneCorners, H);

        drawMatches(book, kpsBook, scene, kpsScene, good, imgmatch, Scalar::all(-1), Scalar(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        Point2f offset((float)book.cols, 0);
        line(imgmatch, sceneCorners[0] + offset, sceneCorners[1] + offset, Scalar(0,255,0), 3);
        line(imgmatch, sceneCorners[1] + offset, sceneCorners[2] + offset, Scalar(0,255,0), 3);
        line(imgmatch, sceneCorners[2] + offset, sceneCorners[3] + offset, Scalar(0,255,0), 3);
        line(imgmatch, sceneCorners[3] + offset, sceneCorners[0] + offset, Scalar(0,255,0), 3);
    }
    imshow("SIFT match", imgmatch);
    imwrite("siftmatches.png", imgmatch);
    
    return 0;
}
