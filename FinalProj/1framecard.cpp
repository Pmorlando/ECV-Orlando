// written by by Phil Orlando for the Final Project
//compiled with g++ -O0 -g -I/usr/local/include/opencv4 1framecard.cpp -o 1framecard -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <syslog.h>
#include <string>


using namespace cv;
using namespace std;

// constants
string tablefile = "table7.jpg";
double scalefactor = 0.5; // testing to make sure it is still finding cards 
double cardthresh = 0.20; // fine tine with more testing
int lowthresh = 150; // from testing with canny.cpp from exercise 2 90 isolated edges of cards and lost alot of the little ones
int ratioval = 3;
int kernel_size = 3;
int maxcont = 110000;
int mincont = 80000;

vector<string> labels = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"}; // all the card values
// going to try a template of only 13 cards with the template only the corner of the card 
// now trying 4 cards per template

struct Cardtemp {
    string value;
    vector<Mat> imgs;
};

struct Matchresult {
    string value;
    double maxval;
    Point maxloc;
};


Matchresult Matchcard(Mat cardcorner, vector<Cardtemp>& cards)
{
    Mat binarycorner;
    threshold(cardcorner, binarycorner, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
    
    
    Matchresult bestcard; // using struct to store the multiple values for each card i want
    bestcard.maxval = 0;
    bestcard.value = "";
    
    for(auto& i : cards)
    {
    	for(auto& samp : i.imgs)
    	{
    	    Mat result;
    	    matchTemplate(binarycorner, samp, result, TM_CCOEFF_NORMED);
    	    
    	    
    	    double maxval;
    	    Point maxloc;
    	    minMaxLoc(result, NULL, &maxval, NULL, &maxloc);
    	    
    	    if(maxval > bestcard.maxval)
    	    {
    	    	bestcard.maxval = maxval; // set new highest value 
    	    	bestcard.value = i.value; // the label is now which had the highest correlation
    	    	bestcard.maxloc = maxloc;
    	    }
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

vector<Cardtemp> loadtemp(vector<string> labels) 
{
    vector<Cardtemp> out;
    for(auto& val : labels) 
    {
        Cardtemp templates;
        templates.value = val;
        
        for(int s =0; s< 4; s++)// hardcoded 4 samples for each
        {
            string filename = val + "/" + val + "samp" + to_string(s+1) + ".jpg"; //samp1-4
            
            Mat img = imread(filename, IMREAD_GRAYSCALE);
            if(img.empty()) 
            { 
                printf("failed to load %s\n", val.c_str()); 
                continue;             
            }
            
            Mat binarytemp;
            threshold(img, binarytemp, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
            templates.imgs.push_back(binarytemp);
        }
        
        out.push_back(templates);

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
    struct timespec start, end, contourstart, contourend, matchstart, matchend;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    Mat table, tablecolor, tablesmall;
    
    if(argc < 2)
    {
    	tablecolor = imread(tablefile, IMREAD_COLOR); // table file for testing using upright images of cards
    	table = imread(tablefile, IMREAD_GRAYSCALE);
    	printf("no file passed going with %s\n",tablefile.c_str());
    }
    else
    {
    	string inputfile = argv[1];
    	tablecolor = imread(inputfile, IMREAD_COLOR); // table file for testing using upright images of cards
    	table = imread(inputfile, IMREAD_GRAYSCALE);
    	printf("using input file %s\n",inputfile.c_str());
    }
    
    // adding in all templates for later testing
    vector<Cardtemp> cardtemplates = loadtemp(labels);
    
 

    if(table.empty())
    {
        printf("error loading test table image");
        return -1;
    }
    Mat blurtable, cannyedge, cornerdisp, contourdisp;
    vector<vector<Point>> contours;
    vector<vector<Point>> cardcontours;
    vector<vector<Point>> cardcorners;
    vector<Vec4i> hierarchy;
    
    // adding timing for frame manipulation
    clock_gettime(CLOCK_MONOTONIC, &contourstart);
    
    // for later visual
    cornerdisp=tablecolor.clone();

    //compress table for card finding
    resize(table, tablesmall, Size(), scalefactor, scalefactor, INTER_AREA);

    // blur table and canny edge detect to find the cards
    blur(tablesmall, blurtable, Size(3,3)); // changed to table small for compression
    Canny(blurtable, cannyedge, lowthresh, lowthresh*ratioval, kernel_size);
    
    // added visual
    //imshow("canny edge",cannyedge);
    //waitKey(0);
    

    findContours(cannyedge, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //CHAIN_APPROX_SIMPLE keeps only end points of contourstraight lines so faster
    // was finding doubles for the cards and external should fix it 
    // testing contour area to get min and max contour if messing with new images 
    /*
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);
        //printf("contour %zu area is %f\n",i, area); // issue with printing i so need zu
        
    }
    */
    // contour filter to keep only contours about the size of the cards 
    for(size_t i = 0; i < contours.size();i++)
    {
        double area = contourArea(contours[i]);

        // for compression
        int minscaled = mincont * scalefactor;
        int maxscaled = maxcont * scalefactor;
        if(area < minscaled || area > maxscaled) continue; // skip contours outside the size

        cardcontours.push_back(contours[i]); // make new list of the cards contours only
    }

    // added visual
    contourdisp=tablecolor.clone();
    drawContours(contourdisp, cardcontours, -1, Scalar(0,255,0), 1);
    //imshow("Contours of cards", contourdisp);
    //waitKey(0);



    // get contours to 4 corner points 
    for(size_t j = 0; j < cardcontours.size();j++)
    {
        double perimeter = arcLength(cardcontours[j], true); // finding the arclength of the contours that are closed
        vector<Point> corners;
        approxPolyDP(cardcontours[j], corners, 0.02 *perimeter, true); // 0.2 working

        if(corners.size() != 4) continue;

        // for mapping compressed corners back to original image
        for(auto& i : corners)
        {
            i.x = static_cast<int>(i.x/scalefactor);
            i.y = static_cast<int>(i.y/scalefactor);
        }

        cardcorners.push_back(corners); // list of card 4 corners 
        // printf("card corners %zu\n", cardcorners.size());
        
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
        

        /* printf(" top left (%f, %f) top right (%f, %f) bottom left (%f, %f) bottom right (%f, %f)\n",
                srcpts[0].x, srcpts[0].y,
                srcpts[1].x, srcpts[1].y,
                srcpts[2].x, srcpts[2].y,
                srcpts[3].x, srcpts[3].y
            );
        */
        vector<Point2f> dstpts = { Point2f(0,0), Point2f(200,0), Point2f(0,300), Point2f(200,300) };

        Mat M = getPerspectiveTransform(srcpts,dstpts); // finding difference from the cards points to a perfect upgright image

        Mat warped;
        warpPerspective(table, warped, M, Size(200,300)); // take table and extract a straightened card from it

        cardsisolated.push_back(warped);
    }
    
    //imshow("Corner order check", cornerdisp);
    //waitKey(0);

    // added visual
    /*
    for (int i = 0; i < numcards; i++) {
        //imshow("Isolated card " + to_string(i), cardsisolated[i]);
    }
    waitKey(0);
    */

    for(size_t i =0; i < cardsisolated.size(); i++)// isolate corner of the card to run into matching
    {
        Rect cornercard(0, 0, cardsisolated[i].cols * .26, cardsisolated[i].rows * .32); // test and adjust if getting errors
        Mat corner = cardsisolated[i](cornercard);
        TLofcards.push_back(corner);
    }

    /* added visual
    for (size_t i = 0; i < TLofcards.size(); i++) {
        //imshow("Corner crop " + to_string(i), TLofcards[i]);
    }
    //waitKey(0);
    */
    clock_gettime(CLOCK_MONOTONIC, &contourend);
    clock_gettime(CLOCK_MONOTONIC, &matchstart);

    // matching  
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
        putText(tablecolor, thiscard.value, Point(cardcorners[i][0].x + 20, cardcorners[i][0].y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 0), 2); // draw which value
        // +20 puts the label above the box and not over the corner
        
    }
    clock_gettime(CLOCK_MONOTONIC, &matchend);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double dt = (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1e6;
    syslog(LOG_INFO, "frame process time %.3f ms", dt);
    
    double dtcontour = (contourend.tv_sec - contourstart.tv_sec)*1000.0 + (contourend.tv_nsec - contourstart.tv_nsec)/1e6;
    syslog(LOG_INFO, "contour process time %.3f ms", dtcontour);
    
    double dtmatch = (matchend.tv_sec - matchstart.tv_sec)*1000.0 + (matchend.tv_nsec - matchstart.tv_nsec)/1e6;
    syslog(LOG_INFO, "match process time %.3f ms", dtmatch);
    

    imshow("Blackjack table with found cards", tablecolor);

    /*
    to think about:
    how to best thread the 6 cores, maybe 1 core per sample in the templates 
    multi core the blur, canny and contouring stuff 
    so i guess 2 sections of threading and multicore stuff, need to see size of the resized table to see how to best multi core it 

    maybe put functions to the blur, canny and other stuff to keep main cleaner and easier to follow 
    maybe 6 core where 5 get 2 cards and 1 gets 3 cards

    check table15 for 4 card speed 

    setNumThreads(6) at begining of main, and setNumThreads(1) before matching and mutithread that 

    
    */ 



    closelog();
    waitKey(0);
    return 0;
    
}
