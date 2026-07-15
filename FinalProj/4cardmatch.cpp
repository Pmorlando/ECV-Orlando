// written by by Phil Orlando for finalproj
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 4cardmatch.cpp -o 4cardmatch -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>


using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
    Mat temp8 = imread("8temp.jpg", IMREAD_GRAYSCALE); // template file of cropped 8 corner
    Mat temp9 = imread("9temp.jpg", IMREAD_GRAYSCALE); // template file of cropped 9 corner
    Mat temp10 = imread("10temp.jpg", IMREAD_GRAYSCALE); // template file of cropped 10 corner
    Mat tempQ = imread("Qtemp.jpg", IMREAD_GRAYSCALE); // template file of cropped Queen corner
    Mat table = imread("table.jpg", IMREAD_GRAYSCALE); // table file for testing using upright images of cards

    if(temp8.empty() || temp9.empty() || temp10.empty() || tempQ.empty() || table.empty())
    {
        printf("error loading stuff");
        return -1;
    }
    
    Mat result8, result9, result10, resultQ;
    result8 = table.clone();
    result9 = table.clone();
    result10 = table.clone();
    resultQ = table.clone();

    int method = TM_CCOEFF_NORMED;

    matchTemplate(table, temp8,  result8, method);
    matchTemplate(table, temp9,  result9, method);
    matchTemplate(table, temp10,  result10, method);
    matchTemplate(table, tempQ,  resultQ, method);

    double maxval8, maxval9, maxval10, maxvalQ;

    Point maxloc8, maxloc9, maxloc10, maxlocQ;

    minMaxLoc(result8, NULL, &maxval8, NULL, &maxloc8);
    minMaxLoc(result9, NULL, &maxval9, NULL, &maxloc9);
    minMaxLoc(result10, NULL, &maxval10, NULL, &maxloc10);
    minMaxLoc(resultQ, NULL, &maxvalQ, NULL, &maxlocQ);

    rectangle(table, maxloc8, Point(maxloc8.x + temp8.cols, maxloc8.y + temp8.rows), Scalar(0, 255, 0), 2);
    rectangle(table, maxloc9, Point(maxloc9.x + temp9.cols, maxloc9.y + temp9.rows), Scalar(0, 255, 0), 2);
    rectangle(table, maxloc10, Point(maxloc10.x + temp10.cols, maxloc10.y + temp10.rows), Scalar(0, 255, 0), 2);
    rectangle(table, maxlocQ, Point(maxlocQ.x + tempQ.cols, maxlocQ.y + tempQ.rows), Scalar(0, 255, 0), 2);

    imshow("Blackjack Table", table);
    imshow("8 Result", result8);
    imshow("9 Result", result9);
    imshow("10 Result", result10);
    imshow("Q Result", resultQ);


    waitkey(0);

    return 0;
}
