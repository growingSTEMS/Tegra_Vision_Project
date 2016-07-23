/* NOTES:
 * Most likely will either make a secondary function for processing the
 * config file information or do the processing straight in the readFile
 * function. If it is the latter then configPair_t will need to be
 * changed to a std::pair<std::string, int>
 */
#include <cstdio>  //consle input and output
#include <string>  //strings
#include <vector>  //vectors

//used for opencv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv; //used for opencv
int main(int argc, char** argv)
{
  //syntax check
  if (argc < 3)
  {
    printf("Syntax error: %s <camera number> <camera number 2>\n", argv[0]);
    exit(-1);
  }
  
  //open camera one
  int cameraNumber = atoi(argv[1]); //number equates to the /dev/video number of the camera
  VideoCapture camera;
  camera.open(0);
  if (!camera.isOpened())
  {
    printf("problem opening camera\n");
    exit(-1);
  }
  /* LIST OF CAMERA SETTINS:
   * CV_CAP_PROP_POS_MSEC: pos of video file in (ms) or capture's timestamp
   * CV_CAP_PROP_POS_FRAMES: 0-based index of the frame to be decoded next
   * CV_CAP_PROP_POS_AVI_RATIO: relative pos of video file
   *                            0: start of film
   *                            1: end of film
   * CV_CAP_PROP_FRAME_WIDTH: width of the frames in the video stream
   * CV_CAP_PROP_FRAME_HEIGHT: height of the frames in the video stream
   * CV_CAP_PROP_FPS: frame rate
   * CV_CAP_PROP_FOURCC: 4 character code of codec
   *    CV_FOURCC('M', 'J', 'P', 'G'): motion-jpeg
   * CV_CAP_PROP_FRAME_COUNT: number of frames in the video
   * CV_CAP_PROP_FORMAT: format of the Mat objects returned by retrieve()
   * CV_CAP_PROP_MODE: backend-specific value indicating current cap mode
   * CV_CAP_PROP_BRIGHTNESS: brightness of the image (only for cameras)
   * CV_CAP_PROP_CONTRAST: contrast of the image (only for cameras)
   * CV_CAP_PROP_SATURATION: saturation of the image (only for cameras)
   * CV_CAP_PROP_HUE: hue of the image (cameras only)
   * CV_CAP_PROP_GAIN: gain of the image (cameras only)
   * CV_CAP_PROP_EXPOSURE: exposure (cameras only)
   */
  double fourcc, fps, width, height;
  fourcc = camera.get(CV_CAP_PROP_FOURCC/*, CV_FOURCC('M', 'J', 'P', 'G')*/);
  fps = camera.get(CV_CAP_PROP_FPS/*, 0*/);
  width = camera.get(CV_CAP_PROP_FRAME_WIDTH/*, 320*/);
  height = camera.get(CV_CAP_PROP_FRAME_HEIGHT/*, 240*/);
  printf("fourcc: %f\n", fourcc);
  printf("fps: %f\n", fps);
  printf("width: %f\n", width);
  printf("height: %f\n", height);
  while (!(waitKey(30) >= 0))
  {
    Mat frame;
    camera.read(frame);
    //cvtcolor(frame, edges, cv_bgr2gray);
    //gaussianblur(edges, edges, size(7,7), 1.5, 1.5);
    //canny(edges, edges, 0, 30, 3);
    imshow("edges", frame);
  }
  return 0;
}
