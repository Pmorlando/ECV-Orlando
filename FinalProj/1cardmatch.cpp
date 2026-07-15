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

// need to add
// extract each card then use that in matchtemplate
// read number of cardsb and then iterate through that 
// assign location of the cards to dealer or player based on top or bottom of screen 
// after extract card, correct it normal card orientation for the template match 


string Matchcard(Mat cardcorner)
{
    double highest = 0;
    string highestlabel = "";
    for(auto& i : cards)
    {
    	Mat result;
    	matchTemplate(cardcorner, i.img, result, TM_CCOEFF_METHOD);
    	double maxval;
    	minMaxLoc(result, NULL, &maxval, NULL, NULL);
    	if(maxval > highest)
    	{
    	    highest = maxval; // set new highest value 
    	    highestlabel = i.label // the label is now which had the highest correlation
    	}
    }
    if(highest < threshold)
    {
    	printf("card recognized is below threshold");
    	break;
    }
    else return highestlabel;  // change to card, max val, and max loc 
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


int main(int argc, char** argv)
{
    // addinhg in all templates for later testing
    cards = loadtemp(labels,tempname);

    Mat table = imread("table1.jpg", IMREAD_GRAYSCALE); // table file for testing using upright images of cards

    if(temp8.empty() || temp9.empty() || temp10.empty() || tempQ.empty() || table.empty())
    {
        printf("error loading stuff");
        return -1;
    }
    
    // need card isolator to find number of cards on screen
    
    // then for loop for number of card and take the pixels from that card, straighten them
    // and then run each card through the match
    // need to adjust to output maxloc from the match so I can block the card value and where for testing reliability
    
    
    
    for(int i : Numcards)
    {
    	// card1 card2 card3 string initilize
    	
    }
    
    
    Point maxloc8, maxloc9, maxloc10, maxlocQ; // just for location of where the box is drawn
    // need from the match function


    printf("8 max %f, 9 max %f, 10 max %f, Queen max %f\n", maxval8, maxval9, maxval10, maxvalQ); // get rid of 

    Mat fin8, fin9, fin10, finQ; // for drawing the box around what it found 
    fin8 = table.clone();
    fin9 = table.clone();
    fin10 = table.clone();
    finQ = table.clone();

    // drawing put in for loop for num of cards 
    rectangle(fin8, maxloc8, Point(maxloc8.x + temp8.cols, maxloc8.y + temp8.rows), Scalar(0, 255, 0), 2);
    rectangle(fin9, maxloc9, Point(maxloc9.x + temp9.cols, maxloc9.y + temp9.rows), Scalar(0, 255, 0), 2);
    rectangle(fin10, maxloc10, Point(maxloc10.x + temp10.cols, maxloc10.y + temp10.rows), Scalar(0, 255, 0), 2);
    rectangle(finQ, maxlocQ, Point(maxlocQ.x + tempQ.cols, maxlocQ.y + tempQ.rows), Scalar(0, 255, 0), 2);

    // imshow("Blackjack Table", table);
    imshow("8 Result", fin8);
    imshow("9 Result", fin9);
    imshow("10 Result", fin10);
    imshow("Q Result", finQ);


    waitKey(0);

    return 0;
}
