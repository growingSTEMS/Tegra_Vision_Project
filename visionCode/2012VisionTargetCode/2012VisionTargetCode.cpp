#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void createControlPanel(string panelName, int numOfControls, const string* controlNames, int* values, const int* maxValues);

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

  //create the control panel for filter paramaters
  cout << "reached 1" << endl;
  string filterControlPanel = "Paramaters Control Panel";
  const int numFilterControls = 4;
  string filterControls [numFilterControls] = {"Hue Min", "Hue Max", "Sat Min", "Lum Min"};
  int filterValues[numFilterControls] = {96, 146, 103, 66};
  int filterMaxVals [numFilterControls] = {255, 255, 255, 255};
  createControlPanel(filterControlPanel, numFilterControls, filterControls, filterValues, filterMaxVals);

  //create the morph control panel
  cout << "reached 2" << endl;
  string morphControlPanel = "Erode Dilate Control Panel";
  const int numMorphControls = 7;
  string morphControls [numMorphControls] = {"Times to Erode: ",
                                            "Times to Dilate: ",
                                            "Dilate first: ",
                                            "Erosions before switch: ",
                                            "Dilations before switch: ",
                                            "Erosion Type: \n 0:Rect\n1:Ellipse\n2:Cross",
                                            "Kernal Size: \n 2n+1"};
  int morphValues[numMorphControls] = {21, 23, 0, 1, 2, 0, 1};
  int morphMaxVals [numMorphControls] = {100, 100, 1, 100, 100, 2, 20};
  createControlPanel(morphControlPanel, numMorphControls, morphControls, morphValues, morphMaxVals);

  //loop until the user tells it to stop (stop key is 'x')
  cout << "reached 3" << endl;
  while ((char)waitKey(30) != 'x')
  {
    int hMin, hMax, sMin, lMin; //filter paramaters
    hMin = getTrackbarPos(filterControls[0], filterControlPanel);
    hMax = getTrackbarPos(filterControls[1], filterControlPanel);
    sMin = getTrackbarPos(filterControls[2], filterControlPanel);
    lMin = getTrackbarPos(filterControls[3], filterControlPanel);

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

    /* -------------------------------------------------------------------
     * Create a new image after performing an erosion and dilation filter
     * on the binary image produced by inRange().
     * dilate function prototype: dilate(InputArray src, OutputArray dst,
     *        InputArray kernal, Point anchor=Point(-1,-1),
     *        int iteration=1, int borderType=BORDER_CONSTANT, const
     *        Scalar &borderValue=morphologyDefaultBorderValue())
     * erode has the same function prototype
     * Input descriptions:
     *    InputArray src: the input image to be eroded/dilated
     *    OutputArray dst: the resultant image that is the same size as src
     *    InputArray kernal: structuring element used for erosion/dilation.
     *                       If the element is of a default Mat, a 3x3
     *                       rectangle is used.
     *    Point anchor: The anchor point for kernal. This point is the
     *                  pixel that is being operated on in src.
     *    int iteration: the number of times the function is run.
     *    int borderType: pixel extrapolation method. Look up 
     *                    borderInterpolate() for more details.
     *    const Scalar &borderValue: border value in case there is a
     *                               constant border in the image.
     *
     * When using eorde and dilate, use the function getStructuringElement
     * to create the kernal. 
     * getStructuringElement(int shape, Size ksize, Point anchor(-1,-1)
     * Input Descriptions:
     *    int shape: The shape of the kernal. The shape can be one of the
     *               following: MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
     *               MORPH_RECT: a rectangle of size ksize.
     *               MORPH_ELLIPSE: an ellipse that is inscribed into the
     *                              rectangle of size ksize.
     *               MORPH_CROSS: a cross shaped structuring element
     *    Size ksize: the size of the structuring element
     *    Point anchor: the anchor point of the structuring element. this
     *                  indicated the pixel that is being operated on.
     * -------------------------------------------------------------------
     */
    //create variables
    int timesToErode = getTrackbarPos(morphControls[0], morphControlPanel);
    int timesToDilate = getTrackbarPos(morphControls[1], morphControlPanel);
    bool performDilation = getTrackbarPos(morphControls[2], morphControlPanel); //dilate first
    int erosionsBeforeSwitch = getTrackbarPos(morphControls[3], morphControlPanel);
    if (erosionsBeforeSwitch == 0)
      erosionsBeforeSwitch = 1;
    int dilationsBeforeSwitch = getTrackbarPos(morphControls[4], morphControlPanel);
    if (dilationsBeforeSwitch == 0)
      dilationsBeforeSwitch = 1;
    int erosionType = MORPH_RECT;
    switch(getTrackbarPos(morphControls[5], morphControlPanel))
    {
      case 0:
        erosionType = MORPH_RECT;
        break;
      case 1:
        erosionType = MORPH_ELLIPSE;
        break;
      case 2:
        erosionType = MORPH_CROSS;
        break;
    }
    int kernalSize = getTrackbarPos(morphControls[6], morphControlPanel);

    //create morphed image container and kernal
    Mat morphedImage = filteredImage; //init to image to be morphed
    Mat kernal = getStructuringElement(erosionType,
                                      Size(2 * kernalSize + 1, 2 * kernalSize + 1),
                                      Point(kernalSize, kernalSize));
    
    //perform erosions and dilations based on control panel
    while(timesToErode > 0 || timesToDilate > 0)
    {
      int erosionCount = 0; //used to track dilations for switching
      int dilationCount = 0; //used to track erosions for switching
      if (performDilation)
      {
        //perform dilations until either timesToDilate has been achieved
        //or the alloted dilationsBeforeSwitch was reached
        while (dilationCount < dilationsBeforeSwitch && timesToDilate > 0)
        {
          dilate(morphedImage, morphedImage, kernal);
          timesToDilate--;
          dilationCount++;
        }
        performDilation = !performDilation; //negate so sister op is performed
      }
      else
      {
        //perform erosions until either timesToErode has been achieved
        //or the alloted erosionsBeforeSwitch was reached
        while (erosionCount < erosionsBeforeSwitch && timesToErode > 0)
        {
          erode(morphedImage, morphedImage, kernal);
          timesToErode--;
          erosionCount++;
        }
        performDilation = !performDilation;
      }
    }
    imshow("morphed image", morphedImage);
  }
}

void createControlPanel(string panelName, int numOfControls, const string* controlNames, int* values, const int* maxValues)
{
  //reference code
  /*namedWindow("Paramaters Control Panel", WINDOW_NORMAL);
  createTrackbar("Hue Min", "Paramaters Control Panel", 0, 255);
  createTrackbar("Hue Max", "Paramaters Control Panel", 0, 255);
  createTrackbar("Sat Min", "Paramaters Control Panel", 0, 255);
  createTrackbar("Lum Min", "Paramaters Control Panel", 0, 255);*/
  namedWindow(panelName, WINDOW_NORMAL);
  for (int i = 0; i < numOfControls; i++)
  {
    if (values != 0)
      createTrackbar(controlNames[i], panelName, &values[i], maxValues[i]);
    else
      createTrackbar(controlNames[i], panelName, 0, maxValues[i]);
  }
}
