// written by by Phil Orlando for the Final Project
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 1cardmatch.cpp -o 1cardmatch -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <syslog.h>
#include <string>


using namespace cv;
using namespace std;



vector<string> labels = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"}; // all the card values
string tempname = "temp.JPG";
// going to try a template of only 13 cards with the template only the corner of the card 
struct Cardtemp {
    string value;
    Mat img;
};

struct Matchresult {
    string value;
    double maxval;
    Point maxloc;
};

double cardthresh = 0.40; // fine tine with more testing

int lowthresh = 150; // from testing with canny.cpp from exercise 2 90 isolated edges of cards and lost alot of the little ones
int ratioval = 3;
int kernel_size = 3;

int maxcont = 90000;
int mincont = 80000;

Matchresult Matchcard(Mat cardcorner, vector<Cardtemp>& cards)
{
    Matchresult bestcard; // using struct to store the multiple values for each card i want
    bestcard.maxval = 0;
    bestcard.value = "";
    
    for(auto& i : cards)
    {
    	Mat result;
    	matchTemplate(cardcorner, i.img, result, TM_CCOEFF_NORMED);

    	double maxval;
        Point maxloc;
    	minMaxLoc(result, NULL, &maxval, NULL, &maxloc); // finding the max correlation and where it is on the card

    	if(maxval > bestcard.maxval)
    	{
    	    bestcard.maxval = maxval; // set new highest value 
    	    bestcard.value = i.value; // the label is now which had the highest correlation
            bestcard.maxloc = maxloc;
    	}
    }
    if(bestcard.maxval < cardthresh)
    {
    	printf("card recognized is below threshold\n");
    	bestcard.value = ""; // set the stored card value to nothing 
    }
    return bestcard; 
}


// better loading for all templates in the struct with the value and label 

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

struct sortY {
    bool operator() (Point pt1, Point pt2) {return (pt1.y< pt2.y); }
} mySortY;

struct sortX {
    bool operator() (Point pt1, Point pt2) {return (pt1.x < pt2.x); }
} mySortX;


vector<Point2f> cornerorder(vector<Point>& pts)
{
    sort(pts.begin(), pts.end(), mySortY); // sorting into upper card corner and bottom
    sort(pts.begin(), pts.begin()+2, mySortX); // sorting top points left to right
    sort(pts.begin()+2, pts.end(), mySortX); // sorting bottom points left and right

    vector<Point2f> ordered; 

    for(auto& p : pts)
    {
        ordered.push_back(Point2f(p));// put them in ordered list now
    }
    return ordered; //return the ordered list
}



int main(int argc, char** argv)
{
    // syslog for timing check to get initial wcet
    openlog("BlackJack_cv", LOG_PID, LOG_USER);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    
    // adding in all templates for later testing
    vector<Cardtemp> cardtemplates = loadtemp(labels,tempname);
    
    Mat tablecolor = imread("table1.jpg", IMREAD_COLOR); // table file for testing using upright images of cards
    Mat table = imread("table3.jpg", IMREAD_GRAYSCALE); 

    if(table.empty())
    {
        printf("error loading test table image");
        return -1;
    }
    Mat blurtable, cannyedge;
    vector<vector<Point>> contours;
    vector<vector<Point>> cardcontours;
    vector<vector<Point>> cardcorners;
    vector<Vec4i> hierarchy;
    
    // for later visual
    Mat cornerdisp=tablecolor.clone();

    // blur table and canny edge detect to find the cards

    blur(table, blurtable, Size(3,3));
    Canny(blurtable, cannyedge, lowthresh, lowthresh*ratioval, kernel_size);
    

    findContours(cannyedge, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //CHAIN_APPROX_SIMPLE keeps only end points of contourstraight lines so faster
    // was finding doubles for the cards and external should fix it 
    // testing contour area to get min and max contour
    /*
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);
        printf("contour %zu area is %f\n",i, area); // issue with printing i so need zu
        
    }
    */
    // contour filter to keep only contours about the size of the cards 
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);
        if(area < mincont || area > maxcont) continue; // skip contours outside the size

        cardcontours.push_back(contours[i]); // make new list of the cards contours only
    }

    // added visual
    Mat contourdisp=tablecolor.clone();
    drawContours(contourdisp, cardcontours, -1, Scalar(0,0,255), 1);
    imshow("Contours of cards", contourdisp);
    waitKey(0);



    // get contours to 4 corner points 
    for(size_t j = 0; j < cardcontours.size();j++)
    {
        double perimeter = arcLength(cardcontours[j], true); // finding the arclength of the contours that are closed
        vector<Point> corners;
        approxPolyDP(cardcontours[j], corners, 0.02 *perimeter, true); // adjsut 0.2 

        if(corners.size() != 4) continue;

        cardcorners.push_back(corners); // list of card 4 corners 
        printf("card corners %zu\n", cardcorners.size());
        
    }

    

    int numcards = cardcorners.size();

    // order the corner points by top l top r bottom l bottom r

    vector<Mat> cardsisolated;
    vector<Mat> TLofcards;
  
    for(int i = 0; i < numcards;i++)
    {
        // perspectiive transform to straighten cards out for reading
        vector<Point2f> srcpts = cornerorder(cardcorners[i]);
        // making sure order is right

        // added visual
        
        for (int i = 0; i < numcards; i++) {
            vector<Point2f> srcpts = cornerorder(cardcorners[i]);
            circle(cornerdisp, srcpts[0], 6, Scalar(0,0,255), -1);   // TL = red
            circle(cornerdisp, srcpts[1], 6, Scalar(0,255,0), -1);   // TR = green
            circle(cornerdisp, srcpts[2], 6, Scalar(255,0,0), -1);   // BL = blue
            circle(cornerdisp, srcpts[3], 6, Scalar(0,255,255), -1); // BR = yellow
        }
        

        printf(" top left (%f, %f) top right (%f, %f) bottom left (%f, %f) bottom right (%f, %f)\n",
                srcpts[0].x, srcpts[0].y,
                srcpts[1].x, srcpts[1].y,
                srcpts[2].x, srcpts[2].y,
                srcpts[3].x, srcpts[3].y
            );
        vector<Point2f> dstpts = { Point2f(0,0), Point2f(200,0), Point2f(0,300), Point2f(200,300) };

        Mat M = getPerspectiveTransform(srcpts,dstpts); // finding difference from the cards points to a perfect upgright image

        Mat warped;
        warpPerspective(table, warped, M, Size(200,300)); // take table and extract a straightened card from it

        cardsisolated.push_back(warped);
    }
    
    imshow("Corner order check", cornerdisp);
    waitKey(0);

    // added visual
    for (int i = 0; i < numcards; i++) {
        imshow("Isolated card " + to_string(i), cardsisolated[i]);
    }
    waitKey(0);

    for(size_t i =0; i < cardsisolated.size(); i++)// isolate corner of the card to run into matching
    {
        Rect cornercard(0, 0, cardsisolated[i].cols * .2, cardsisolated[i].rows * .2); // test and adjust if getting errors
        Mat corner = cardsisolated[i](cornercard);
        TLofcards.push_back(corner);
    }

    // added visual
    for (size_t i = 0; i < TLofcards.size(); i++) {
        imshow("Corner crop " + to_string(i), TLofcards[i]);
    }
    waitKey(0);
    
    // matching put here with TLofCards and cardtemp. 
    vector<Matchresult> foundcards;
    for(size_t i =0; i <TLofcards.size();i++)
    {
        
        Matchresult thiscard = Matchcard(TLofcards[i], cardtemplates);

        printf("card value %s, correlation, %f, location (%d, %d)\n", 
                thiscard.value.c_str(),
                thiscard.maxval,
                thiscard.maxloc.x, thiscard.maxloc.y
            );

        
        if(thiscard.value == "") thiscard.value = "Value not found"; 

        foundcards.push_back(thiscard);


        rectangle(tablecolor, cardcorners[i][0], cardcorners[i][3], Scalar(0,255,0), 2); // draw rectangle around the cards
        putText(tablecolor, thiscard.value, Point(cardcorners[i][0].x + 20, cardcorners[i][0].y), FONT_HERSHEY_SIMPLEX, 2.0, Scalar(0, 255, 0), 2); // draw which value
        // helps will testing of reliability where each frame will be labeled and found 
        
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double dt = (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1e6;
    syslog(LOG_INFO, "frame process time %.3f ms", dt);

    imshow("Blackjack table with found cards", tablecolor);


    /* to do
    variable clean up put in header to clear the main function up
    add the foundcards processing for black jack
    once can handle players hand then using the cardcorners locations to differentiate dealer and player
    add play reccomendations via put text and maybe syslog
    add counter for when the value of card is not found and syslog for it
    read video so i can actually test framerate
    
    */
    closelog();
    waitKey(0);
    return 0;
    
}
