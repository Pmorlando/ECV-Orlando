// written by by Phil Orlando for the Final Project
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 4cardmatch.cpp -o 4cardmatch -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>


using namespace cv;
using namespace std;


/*
vector<string> labels = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"}; // all the card values
string tempname = "temp.JPG";
// going to try a template of only 13 cards with the template only the corner of the card 
struct Cardtemp {
    string value;
    Mat img;
};

// need to add
// extract each card then use that in matchtemplate
// read number of cardsb and then iterate through that 
// assign location of the cards to dealer or player based on top or bottom of screen 
// after extract card, correct it normal card orientation for the template match 


string Matchcard(Mat cardcorner)
{
    //add the matching stuff here 
    //return val; // returns string 
}

*/
// better loading for all templates 
/*
vector<Cardtemp> loadtemp(vector<string> labels, string tempname) 
{
    vector<Cardtemp> out;
    for(auto& val : labels) 
    {
        Mat img = imread(val + tempname, IMREAD_GRAYSCALE);
        if(img.empty()) 
        { 
            printf("failed to load %s\n", val.c_str()); 
            continue;             
        }
        out.push_back({val, img});

    }
    return out;
}
*/

int main(int argc, char** argv)
{
    // addinhg in all templates for later testing
    //loadtemp(labels,tempname);

    


    Mat temp8 = imread("8temp.JPG", IMREAD_GRAYSCALE); // template file of cropped 8 corner
    Mat temp9 = imread("9temp.JPG", IMREAD_GRAYSCALE); // template file of cropped 9 corner
    Mat temp10 = imread("10temp.JPG", IMREAD_GRAYSCALE); // template file of cropped 10 corner
    Mat tempQ = imread("Qtemp.JPG", IMREAD_GRAYSCALE); // template file of cropped Queen corner
    // need to add better way to load all the templates
    Mat table = imread("table1.jpg", IMREAD_GRAYSCALE); // table file for testing using upright images of cards

    if(temp8.empty() || temp9.empty() || temp10.empty() || tempQ.empty() || table.empty())
    {
        printf("error loading stuff");
        return -1;
    }
    
    Mat result8, result9, result10, resultQ;
    int method = TM_CCOEFF_NORMED;

    //will be replaced by the match function
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

    printf("8 max %f, 9 max %f, 10 max %f, Queen max %f\n", maxval8, maxval9, maxval10, maxvalQ);

    Mat fin8, fin9, fin10, finQ;
    fin8 = table.clone();
    fin9 = table.clone();
    fin10 = table.clone();
    finQ = table.clone();

    // drawing just for testing now
    rectangle(fin8, maxloc8, Point(maxloc8.x + temp8.cols, maxloc8.y + temp8.rows), Scalar(0, 255, 0), 2);
    rectangle(fin9, maxloc9, Point(maxloc9.x + temp9.cols, maxloc9.y + temp9.rows), Scalar(0, 255, 0), 2);
    rectangle(fin10, maxloc10, Point(maxloc10.x + temp10.cols, maxloc10.y + temp10.rows), Scalar(0, 255, 0), 2);
    rectangle(finQ, maxlocQ, Point(maxlocQ.x + tempQ.cols, maxlocQ.y + tempQ.rows), Scalar(0, 255, 0), 2);

    imshow("Blackjack Table", table);
    imshow("8 Result", fin8);
    imshow("9 Result", fin9);
    imshow("10 Result", fin10);
    imshow("Q Result", finQ);


    waitKey(0);

    return 0;
}
