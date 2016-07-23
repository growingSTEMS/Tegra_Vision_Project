#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    cout << "please include an image file\n";
    return -1;
  }

  //read in the image
  string imageName = argv[1];
  Mat originalImage = imread(imageName);
  if (!originalImage.data)
  {
    cout << "image could not be read\n";
    return -1;
  }

  //display the original image
  imshow("original image", originalImage);

  //convert image to HSV
  Mat hsvImage;
  cvtColor(originalImage, hsvImage, CV_BGR2HLS);
  imshow("hlsImage", hsvImage); 

  //create the control panel for filter paramaters
  namedWindow("Paramaters Control Panel", WINDOW_NORMAL);
  createTrackbar("Hue Min", "Paramaters Control Panel", 0, 255);
  createTrackbar("Hue Max", "Paramaters Control Panel", 0, 255);
  createTrackbar("Sat Min", "Paramaters Control Panel", 0, 255);
  createTrackbar("Lum Min", "Paramaters Control Panel", 0, 255);

  //loop until the user tells it to stop (stop key is 'x')
  while ((char)waitKey(30) != 'x')
  {
    int hMin, hMax, sMin, lMin; //filter paramaters
    hMin = getTrackbarPos("Hue Min", "Paramaters Control Panel");
    hMax = getTrackbarPos("Hue Max", "Paramaters Control Panel");
    sMin = getTrackbarPos("Sat Min", "Paramaters Control Panel");
    lMin = getTrackbarPos("Lum Min", "Paramaters Control Panel");

    //filter image based on hsv values from trackbars
    /* If hMin is greater than hMax, the filter will need to be split into two parts.
     * The first of the parts will take a filter with 255 as the max and hMin as the min.
     * The second filter will take 0 as the min and hMax as the max.
     * after the seperate filters are performed, they will combined using bitwise_or().
     * this will create a single binary image that represents the proper filtering.
     */
    Mat filteredImage; //create image container first, fill it later
    if (hMin > hMax)
    {
      //double filter needed
      Mat minRange, maxRange; //intermediate image filters
      inRange(hsvImage, Scalar(0, sMin, lMin), Scalar(hMax, 255, 255), minRange); //min range filter
      inRange(hsvImage, Scalar(hMin, sMin, lMin), Scalar(255, 255, 255), maxRange); //max range filter
      bitwise_or(minRange, maxRange, filteredImage); //or combination
    }
    else
    {
      //single filter only
      inRange(hsvImage, Scalar(hMin, sMin, lMin), Scalar(hMax, 255, 255), filteredImage);
    }
    imshow("filtered image", filteredImage);

    //debug code
    /*cout << "Hue Min" << hMin << endl;
    cout << "Hue Max" << hMax << endl;
    cout << "Sat Min" << sMin << endl;
    cout << "Lum Min" << lMin << endl;*/
  }
  //filter for the individual squares
  /*Mat redOnly, greenOnly, blueOnly;
  inRange(originalImage, Scalar(0, 0, 0), Scalar(0, 0, 255), redOnly);
  inRange(originalImage, Scalar(0, 0, 0), Scalar(0, 255, 0), greenOnly);
  inRange(originalImage, Scalar(0, 0, 0), Scalar(255, 0, 0), blueOnly);

  //display filtered images
  imshow("red only image", redOnly);
  imshow("blue only image", blueOnly);
  imshow("green only image", greenOnly);*/

  //bitwise_or() testing
  /*Mat orCombinedImage;
  bitwise_or(redOnly, greenOnly, orCombinedImage);
  bitwise_or(blueOnly, orCombinedImage, orCombinedImage);
  imshow("or combination test", orCombinedImage);
  waitKey();*/
}
