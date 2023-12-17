#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(){

    using namespace cv;
    
    Mat img = imread("./rain.jpeg");
    if(img.empty()){
       fprintf(stderr,"Error : Could not open or read the image file");
    }
    namedWindow("Display window",WINDOW_AUTOSIZE);
    imshow("Display window",img);
    waitKey(0);

    return 0;

}
