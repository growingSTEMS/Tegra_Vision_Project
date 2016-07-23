/* NOTES:
 * Most likely will either make a secondary function for processing the
 * config file information or do the processing straight in the readFile
 * function. If it is the latter then configPair_t will need to be
 * changed to a std::pair<std::string, int>
 */
#include <cstdio>  //consle input and output
#include <string>  //strings
#include <fstream> //file operations
#include <vector>  //vectors
#include <utility> //pair

//used for opencv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
int main(int argc, char** argv)
{
  int saCameraNumber = 1;
  VideoCapture camera(saCameraNumber); //open camera one
  while (!(waitKey(30) >= 0))
  {
    Mat frame;
    Mat edges;
    camera >> frame;
    cvtColor(frame, edges, CV_BGR2GRAY);
    GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);
    imshow("image", frame);
    imshow("edges", edges);
  }
  return 0;
}
