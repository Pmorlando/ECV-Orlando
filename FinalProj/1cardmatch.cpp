// written by by Phil Orlando for the Final Project
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 1cardmatch.cpp -o 1cardmatch -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>


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

double cardthresh = 0.80; // fine tine with more testing

int lowthresh = 150; // from testing with canny.cpp from exercise 2 90 isolated edges of cards and lost alot of the little ones
int ratio = 3;
int kernel_size = 3;

int maxcont = 10;
int mincont = 9;

string Matchcard(Mat cardcorner, vector<Cardtemp>& cards)
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
    return bestcard;  // change to card, max val, and max loc 
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
    // adding in all templates for later testing
    vector<Cardtemp> cardtemplates = loadtemp(labels,tempname);

    Mat tablecolor = imread("table1.jpg", IMREAD_COLOR); // table file for testing using upright images of cards
    Mat table = imread("table1.jpg", IMREAD_GRAYSCALE); 

    if(table.empty())
    {
        printf("error loading test table image");
        return -1;
    }
    Mat blur, cannyedge;
    vector<vector<Point>> contours;
    vector<vector<Point>> cardcontours;
    vector<vector<Point>> cardcorners;
    vector<Vec4i> hierarchy;

    // blur table and canny edge detect to find the cards

    blur(table, cannyedge, Size(3,3));
    Canny(cannyedge, cannyedge, lowthresh, lowthresh*ratio, kernel_size);

    findContours(cannyedge, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //CHAIN_APPROX_SIMPLE keeps only end points of contourstraight lines so faster
    
    // testing contour area to get min and max contour
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);
        printf("contour %zu area is %f\n",i, area); // issue with printing i so need zu
        
    }

    // contour filter to keep only contours about the size of the cards 
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);
        if(area < mincont || area > maxcont) continue; // skip contours outside the size

        cardcontours.push_back(contours[i]); // make new list of the cards contours only
    }
    // get contours to 4 corner points 
    for(size_t j = 0; j < cardcontours.size();j++)
    {
        double perimeter = arcLength(cardcontours[j], true); // finding the arclength of the contours that are closed
        vector<Point> corners;
        approxPolyDP(cardcontours[j], corners, 0.02 *perimeter, true); 

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

    for(size_t i =0; i < cardsisolated.size(); i++)// isolate corner of the card to run into matching
    {
        Rect cornercard(0, 0, cardsisolated[i].cols * .25, cardsisolated[i].rows * .25); // test and adjust if getting errors
        Mat corner = cardsisolated[i](cornercard);
        TLofcards.push_back(corner);
    }
    
    // matching put here with TLofCards and cardtemp. 
    vector<Matchresult> foundcards;
    for(size_t i =0; i <TLofcards.size();i++)
    {
        Matchresult thiscard = Matchcard(TLofcards[i], cardtemplates);

        printf("card value %s, correlation, %f, location (%f, %f)\n", 
                thiscard.value.c_str(),
                thiscard.maxval,
                thiscard.maxloc.x, thiscard.maxloc.y
            );

        
        if(thiscard.value == "") thiscard.value = "Value not found"; 

        foundcards.push_back(thiscard);


        rectangle(tablecolor, cardcorners[i][0], cardcorners[i][3], Scalar(0,255,0), 2); // draw rectangle around the cards
        putText(tablecolor, thiscard.value, Point(cardcorners[i][0].x + 20, cardcorners[i][0].y + 20), FONT_HERSHEY_SIMPLEX, 2.0, Scalar(0, 255, 0), 2); // draw which value
        // helps will testing of reliability where each frame will be labeled and found 
        
    }
    imshow("Blackjack table with found cards", tablecolor);


    /* to do
    variable clean up put in header to clear the main function up
    timing to see how long each frame will roughly take
    add the foundcards processing for black jack
    once can handle players hand then using the cardcorners locations to differentiate dealer and player
    add play reccomendations via put text and maybe syslog
    add counter for when the value of card is not found and syslog for it
    read video so i can actually test framerate
    
    */

    waitKey(0);
    return 0;
}
