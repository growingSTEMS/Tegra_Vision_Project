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

using namespace cv; //used for opencv

//enums
enum class ConfigOptions
{
  physicalAddress,
  logicalAddress
};

enum class CameraJobs
{
  autonomous,
  situationAwareness,
  situationAwarenessTwo,
  noJob,
  error
};

//aliasses
//configPair will contain as its first element the configure option and
//the second element will be the line from the file
using configPair_t = std::pair<ConfigOptions, std::string>;
using configVect_t = std::vector<configPair_t>;
using settingsPair_t = std::pair<CameraJobs, int>;
using settingsVect_t = std::vector<settingsPair_t>;

//function prototypes
void readFile(std::string fileLocation, configVect_t& dictionary);
void processFile(const configVect_t& configOptions, settingsVect_t& settings);
//enum printing functions
const char* printConfigEnum(ConfigOptions toPrint);
const char* printCameraJobs(CameraJobs toPrint);

int main(int argc, char** argv)
{
  //ensure the cmd line args are correct
  if (argc < 2)
  {
    printf("syntax error: %s <camera config file>\n", argv[0]);
    exit(-1);
  }
  //process cmd line args
  std::string fileLocation = argv[1];

  //create a configVect_t to contain the config file information
  configVect_t configInfo;
  readFile(fileLocation, configInfo);

  //display the contents of the configVect_t DEBUG ONLY
  for (int i = 0; i < configInfo.size(); i++)
  {
    //display first information
    printf("Config Info Type: %s\n", printConfigEnum(configInfo[i].first));
    //display second information
    printf("Config Infor Value: %s\n", configInfo[i].second.c_str());
    printf("\n");
  }

  //create a settingsVect_t to contain the processed file information
  settingsVect_t settings;
  processFile(configInfo, settings);

  //display contents of the settingsVect_t DEBUG ONLY
  for (int i = 0; i < settings.size(); i++)
  {
    printf("Camera Job: %s\t", printCameraJobs(settings[i].first));
    printf("Camera Number: %i\n", settings[i].second);
  }
  /* List of VideoCapture member functions:
   * Constructors:
   *    default: no args
   *    (int device): camera to open
   *    (string filename): video to open
   *
   * open(string filename): video to open
   * open(int device): camera to open
   *
   * isOpened(): returns true if video capturing has been inited
   *
   * release(): closes video file or capturing device
   *
   * grab(): grabs the next frame from video file or capturing device
   *
   * retrieve(Mat image, int channel): decodes and returns the grabbed video
   *                                   frame
   * read(mat& image): grabs, decodes, and returns the next video frame
   *    also has operator>>(mat& image) overload
   */
  int autoCameraNumber = 0;
  //search through the settings vector for the camera number of the job
  for (int i = 0; i < settings.size() && i != -1; i++)
  {
    if (settings[i].first == CameraJobs::autonomous)
    {
      autoCameraNumber = settings[i].second;
      i = -2; //used to stop the loop. set to -2 because i increments before the check
    }
  }
  int saCameraNumber = 0;
  //search through the settings vector for the camera number of the job
  for (int i = 0; i < settings.size() && i != -1; i++)
  {
    if (settings[i].first == CameraJobs::situationAwareness)
    {
      saCameraNumber = settings[i].second;
      i = -2; //used to stop the loop. set to -2 because i increments before the check
    }
  }
  VideoCapture camera(saCameraNumber); //open camera one
  if (!camera.isOpened())
  {
    printf("Error openeing camera\n");
    exit(-1);
  }
  VideoCapture cameraTwo(autoCameraNumber);//open camera two
  if (!cameraTwo.isOpened())
  {
    printf("Error openeing camera\n");
    exit(-1);
  }
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

void readFile(std::string fileLocation, configVect_t& dictionary)
{
  std::string currentLine; //used for getLine()
  std::ifstream configFile(fileLocation); //open the config file

  if (configFile.is_open())
  {
    while (getline(configFile, currentLine))
    {
      /* Grab the first character of the line.
       * If the character is a '-' then the line is an physical address
       * If the character is a 'v' then the line is a logical address
       */
      char configChar = currentLine[0]; //get the first character of each line
      switch(configChar)
      {
        case '-':
          dictionary.push_back(std::make_pair(ConfigOptions::physicalAddress, currentLine));
          break;
        case 'v':
          dictionary.push_back(std::make_pair(ConfigOptions::logicalAddress, currentLine));
          break;
        default:
          printf("Error reading the configuration file\n");
          exit(-1);
          break;
      }
    }
  }
  else
  {
    printf("error reading from file\n");
    exit(-1);
  }
}

void processFile(const configVect_t& configOptions, settingsVect_t& settings)
{
  int currentCameraNumber = -1; //will be used to store the camera number
  CameraJobs currentCameraJob = CameraJobs::error; //store the camera job
  //loop through each line from the configure file
  for (int i = 0; i < configOptions.size(); i++)
  {
    /* This code will be changed to make use of a pre-loaded configuation
     * file that will contain the physical addresses and the camera's job.
     * This code makes temporary use of a special camera job called error.
     * This camera job will only be present in this code to allow for a 
     * test to ensure that both options have been reached before assigning
     * them to settings. Once both have been assigned the settings will be
     * updated and then they will be reset to unattainable values (-1 and
     * error).
     */
    switch(configOptions[i].first)
    {
      case ConfigOptions::logicalAddress:
        {
          //grab the last character of the line. this equates to openCV number
          std::string currentLine = configOptions[i].second;
          unsigned char videoNum = currentLine[currentLine.size() - 1];
          currentCameraNumber = (int)videoNum - '0';
          break;
        }
      case ConfigOptions::physicalAddress:
        /* String literals only used for this code. Real code will use
         * a predefined configuration file with links between physical
         * address and camera job.
         */
        if (configOptions[i].second == "-0_3_1_1_0")
          currentCameraJob = CameraJobs::autonomous;
        else if (configOptions[i].second == "-0_3_2_1_0")
          currentCameraJob = CameraJobs::situationAwareness;
        else if (configOptions[i].second == "-0_3_3_1_0")
          currentCameraJob = CameraJobs::situationAwarenessTwo;
        else currentCameraJob = CameraJobs::noJob;
        break;
      default:
        printf("error processing the configuration file\n");
        exit(-1);
    }
    if (currentCameraNumber != -1 && currentCameraJob != CameraJobs::error)
    {
      settings.push_back(std::make_pair(currentCameraJob, currentCameraNumber));
      //reinitialize the values to default values
      currentCameraNumber = -1;
      currentCameraJob = CameraJobs::error;
    }
  }
}

const char* printConfigEnum(ConfigOptions toPrint)
{
  switch(toPrint)
  {
    case ConfigOptions::physicalAddress:
      return "physical address";
    case ConfigOptions::logicalAddress:
      return "logical address";
    default:
      return "there has been an error printing config enum";
  }
}

const char* printCameraJobs(CameraJobs toPrint)
{
  switch(toPrint)
  {
    case CameraJobs::autonomous:
      return "autonomous";
    case CameraJobs::situationAwareness:
      return "situationAwareness";
    case CameraJobs::situationAwarenessTwo:
      return "situationAwarenessTwo";
    case CameraJobs::noJob:
      return "no job";
    case CameraJobs::error:
      return "error";
    default:
      return "there has been an error printing camera jobs enum";
  }
}
