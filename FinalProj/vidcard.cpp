// written by by Phil Orlando for the Final Project
//compiled with g++ -O0 -g -fopenmp -I/usr/local/include/opencv4 vidcard.cpp -o vidcard -L/usr/local/lib `pkg-config --libs opencv4`


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <syslog.h>
#include <string>
#include <omp.h>
#include <algorithm> // for the max element

using namespace cv;
using namespace std;

// constants
string tablefile = "tablevid1_720p.mp4";
double scalefactor = 0.75; // testing to make sure it is still finding cards 
double cardthresh = 0.50; // fine tune with more testing
int lowthresh = 150; // from testing with canny.cpp from exercise 2 90 isolated edges of cards and lost alot of the little ones
int ratioval = 3;
int kernel_size = 3;
int maxcont = 60000;
int mincont = 20000; // lowered for compressing table
vector<string> labels = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"}; // all the card values

struct Cardtemp {
    string value;
    vector<Mat> imgs;
};

struct Matchresult {
    string value;
    double maxval;
    Point maxloc;
};

// putting thresh in main to not redo it a bunch
Matchresult matchcard(Mat cornerbinary, const Cardtemp& cards)
{
    vector<double> scores;
    for(auto& samp : cards.imgs)
    {
        //same matching idea as before and removed location
        Mat result;
        matchTemplate(cornerbinary, samp, result, TM_CCOEFF_NORMED);

        double maxval;
        minMaxLoc(result, NULL, &maxval, NULL, NULL);
        scores.push_back(maxval);
    }

    Matchresult bestcard;
    bestcard.value = cards.value;
    bestcard.maxval = scores.empty() ? 0 : *max_element(scores.begin(), scores.end());
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
    setNumThreads(6);
    // syslog for timing check to get initial wcet
    openlog("BlackJack_cv", LOG_PID, LOG_USER);
    struct timespec start, end, framestart, frameend, contourstart, contourend, matchstart, matchend, initstart, initend, loadstart, loadend;
    
    clock_gettime(CLOCK_MONOTONIC, &initstart);
    
    // adding in all templates
    vector<Cardtemp> cardtemplates = loadtemp(labels);
    
    Mat tablecolor;
    string inputfile;
    if(argc < 2)
    {
    	inputfile = tablefile;
    	printf("no file passed going with %s\n",inputfile.c_str());
    }
    else
    {
    	inputfile = argv[1];
    	printf("using input file %s\n",inputfile.c_str());
    }

    VideoCapture cap(inputfile);
    
    if(!cap.isOpened())
    {
        printf("error loading test table video");
        return -1;
    }

    long framecnt = 0;
    // make for loop with the read, gray then go into each frame stuff
    clock_gettime(CLOCK_MONOTONIC, &initend);
    clock_gettime(CLOCK_MONOTONIC, &start);

    double dtinit = (initend.tv_sec - initstart.tv_sec) * 1000.0 + (initend.tv_nsec - initstart.tv_nsec) / 1e6;
    syslog(LOG_INFO, "Template loading time %.3f ms", dtinit);

    while(cap.read(tablecolor))
    {
        Mat blurtable, cannyedge;
        vector<vector<Point>> contours;
        vector<vector<Point>> cardcontours;
        vector<vector<Point>> cardcorners;
        vector<Vec4i> hierarchy;
        
        clock_gettime(CLOCK_MONOTONIC, &framestart);
        clock_gettime(CLOCK_MONOTONIC, &loadstart);

        Mat table, tablesmall;
        cvtColor(tablecolor, table, COLOR_BGR2GRAY);

        clock_gettime(CLOCK_MONOTONIC, &loadend);
        double dtload = (loadend.tv_sec - loadstart.tv_sec) * 1000.0 + (loadend.tv_nsec - loadstart.tv_nsec) / 1e6;
        syslog(LOG_INFO, "image loading time %.3f ms", dtload);
        clock_gettime(CLOCK_MONOTONIC, &contourstart);
        //compress table for contour, blur then canny
        resize(table, tablesmall, Size(), scalefactor, scalefactor, INTER_AREA);
        blur(tablesmall, blurtable, Size(3, 3)); // changed to table small for compression
        Canny(blurtable, cannyedge, lowthresh, lowthresh * ratioval, kernel_size);
        findContours(cannyedge, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // CHAIN_APPROX_SIMPLE keeps only end points of contourstraight lines so faster

        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            // for compression and mult twice for squared size
            int minscaled = mincont * scalefactor * scalefactor;
            int maxscaled = maxcont * scalefactor * scalefactor;
            // for testing contour size kept
            //printf("min contour kept %i, max kept %i\n", minscaled, maxscaled);
            if (area < minscaled || area > maxscaled)
                continue;                        // skip contours outside the size
            cardcontours.push_back(contours[i]); // make new list of the cards contours only
        }
        // contour filter to keep only contours about the size of the cards
        for (size_t j = 0; j < cardcontours.size(); j++)
        {
            double perimeter = arcLength(cardcontours[j], true); // finding the arclength of the contours that are closed
            vector<Point> corners;
            approxPolyDP(cardcontours[j], corners, 0.02 * perimeter, true); // 0.2 working at full resolution

            if (corners.size() != 4)
                continue;

            // for mapping compressed corners back to original image
            for (auto &i : corners)
            {
                i.x = static_cast<int>(i.x / scalefactor);
                i.y = static_cast<int>(i.y / scalefactor);
            }
            cardcorners.push_back(corners); // list of card 4 corners
            // printf("card corners %zu\n", cardcorners.size());
        }
        int numcards = cardcorners.size();
        vector<Mat> cardsisolated;
        vector<Mat> TLofcards;

        // order the corner points by top l top r bottom l bottom r
        for (int i = 0; i < numcards; i++)
        {
            // perspective transform to straighten cards out for reading
            vector<Point2f> srcpts = cornerorder(cardcorners[i]);
            vector<Point2f> dstpts = {Point2f(0, 0), Point2f(200, 0), Point2f(0, 300), Point2f(200, 300)};
            Mat M = getPerspectiveTransform(srcpts, dstpts); // finding difference from the cards points to a perfect upgright image
            Mat warped;
            warpPerspective(table, warped, M, Size(200, 300)); // take table and extract a straightened card from it
            cardsisolated.push_back(warped);
        }
        // isolate corner of the card to run into matching
        for (size_t i = 0; i < cardsisolated.size(); i++) 
        {
            Rect cornercard(0, 0, cardsisolated[i].cols * .26, cardsisolated[i].rows * .32); // sizing for cards double each dimension of templates
            Mat corner = cardsisolated[i](cornercard);
            Mat binarycorner;
            threshold(corner, binarycorner, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
            TLofcards.push_back(binarycorner);
        }

        clock_gettime(CLOCK_MONOTONIC, &contourend);
        double dtcontour = (contourend.tv_sec - contourstart.tv_sec) * 1000.0 + (contourend.tv_nsec - contourstart.tv_nsec) / 1e6;
        syslog(LOG_INFO, "contour process time %.3f ms", dtcontour);
        clock_gettime(CLOCK_MONOTONIC, &matchstart);

        // new matching with parallel
        int totalloops = 13 * numcards;
        vector<Matchresult> unsortresult(totalloops);
        setNumThreads(1);
        // parallel for matching
        #pragma omp parallel for
        for (int loop = 0; loop < totalloops; loop++)
        {
            int cardloop = loop / 13; // loops through the cards in table frame
            int temploop = loop % 13; // remainder so counts through the templates

            unsortresult[loop] = matchcard(TLofcards[cardloop], cardtemplates[temploop]);
        }
        vector<Matchresult> foundcards(numcards);
        for (int eachcard = 0; eachcard < numcards; eachcard++)
        {
            Matchresult bestguess;
            bestguess.maxval = 0;
            bestguess.value = "";
            for (int temps = 0; temps < 13; temps++)
            {
                Matchresult maybe = unsortresult[eachcard * 13 + temps];
                if (maybe.maxval > bestguess.maxval)
                    bestguess = maybe;
            }
            if (bestguess.maxval < cardthresh)
                bestguess.value = "Value not found";
            foundcards[eachcard] = bestguess;
        }

        // draw the cards outline and value
        for (int i = 0; i < numcards; i++)
        {
            rectangle(tablecolor, cardcorners[i][0], cardcorners[i][3], Scalar(0, 255, 0), 2);                                                               // draw rectangle around the cards
            putText(tablecolor, foundcards[i].value, Point(cardcorners[i][0].x + 20, cardcorners[i][0].y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 0), 2); // draw which value
        }
        clock_gettime(CLOCK_MONOTONIC, &matchend);
        double dtmatch = (matchend.tv_sec - matchstart.tv_sec)*1000.0 + (matchend.tv_nsec - matchstart.tv_nsec)/1e6;
        syslog(LOG_INFO, "match process time %.3f ms", dtmatch);

        clock_gettime(CLOCK_MONOTONIC, &frameend);
        double dtframe = (frameend.tv_sec - framestart.tv_sec)*1000.0 + (frameend.tv_nsec - framestart.tv_nsec)/1e6;
        syslog(LOG_INFO, "frame process time %.3f ms, FPS %.3f", dtframe, (1000/dtframe));

        framecnt++;
        //show results

        imshow("Blackjack table with found cards", tablecolor);
        waitKey(1);

    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // timing math and syslog
    double dt = (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1e6;
    double avgfps = framecnt / (dt / 1000.0);
    syslog(LOG_INFO, "Total video process time %.3f ms, AVG FPS %.3f", dt, avgfps);
    printf("Total video process time %.3f ms, AVG FPS %.3f\n", dt, avgfps)
    
    closelog();
    return 0;
    
}
