#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

//used for sockets transmission
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//used for openCV code
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//forward declarations
unsigned char* vectorToBuffer(const vector<unsigned char> &packet, int& sizeOut);
void error(const char* msg);

//template functions
template <typename T> vector<unsigned char> convertToBytes(T toConvert)
{
  /* This function will convert any type T to its underlying bytes.
   * The function does not take into account the endianess of the system.
   * The function utilizes union for conversion. The union will have two
   * elements: an unsigned character array to represent the bytes and a
   * value of type T. The function will return a vector of unsigned chars
   * because of the internal size member vectors poses.
   */
  vector<unsigned char> bytesVect; //return vector of bytes
  union
  {
    //union used for conversion to the underlying bytes
    T value;
    unsigned char bytes[sizeof(T)];
  };

  value = toConvert; //store paramater into the union
  //add bytes to the return vector
  for (int i = 0; i < sizeof(T); i++)
    bytesVect.push_back(bytes[i]);
  return bytesVect;
}

int main(int argc, char** argv)
{
  //ensure the code was called correctly
  if (argc < 4)
  {
    fprintf(stderr, "syntax, %s <portno> <image directory> <number of images to transmit\n", argv[0]);
    exit(1);
  }

  //load images into the server
  int numOfImages = atoi(argv[3]);
  vector<Mat> images;
  for (int i = 0; i < numOfImages; i++)
  {
    //create the name of the image to load
    string imageName = argv[2];
    imageName.append(to_string(i));
    imageName.append(".jpg");
    
    //read image
    images.push_back(imread(imageName));
    if (!images[i].data)
      error("failed to load image");
  }

  /*//load images into server
  vector<Mat> images;
  for (int i = 0; i < imageNames.size(); i++)
  {
    printf("Loading Image: %i\n", i);
    images.push_back(imread(imageNames[i]));
    if (!images[i].data)
      error("One of the timages couldn't be loaded");
  }*/

  //create the server to transmit the data
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  //init serv_addr to zeros
  bzero((char*) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
  if (newsockfd < 0)
    error("ERROR on accept");

  /*imencode(const string& ext, InputArray img, vector<uchar>& buf)
   * ext: file extension that defines the output format
   * img: image to be written
   * buf: output buffer resized to fit the compressed image
   */
  vector<vector<unsigned char>> packets; //images to encode
  vector<int> keys; //enum (eventually) for the type of data transmitted
  vector<int> lengths; //length (in bytes) of the data to be transmitted
  //create packets and header information
  for (int i = 0; i < images.size(); i++)
  {
    keys.push_back(1); //generate key (will eventually matter)
    vector<unsigned char> currentPacket; //create temporary vector for image
    imencode(".jpeg", images[i], currentPacket); //encode image into tempVect
    packets.push_back(currentPacket); //add tempVect to the packets vector
    lengths.push_back(currentPacket.size()); //add length of current packet
  }
  
  //construct header packets
  vector<vector<unsigned char>> headerPackets;
  for (int i = 0; i < packets.size(); i++)
  {
    //will be added to the end of headerPackets
    vector<unsigned char> headerVect;
    //used to contain the bytes that will be added to the end of headerVect
    vector<unsigned char> bytesToAdd;
    
    //add key[i] bytes to headerVect
    bytesToAdd = convertToBytes(keys[i]);
    headerVect.insert(headerVect.end(), bytesToAdd.begin(), bytesToAdd.end());
    //add lengths[i] bytes to headerVect
    bytesToAdd = convertToBytes(lengths[i]);
    headerVect.insert(headerVect.end(), bytesToAdd.begin(), bytesToAdd.end());
    
    //add headerVect to the vector of headerPackets
    headerPackets.push_back(headerVect);
  }

  //transmit the number of images
  vector<unsigned char> numImagesVect; //temp container because resizable
  numImagesVect = convertToBytes(numOfImages);
  int numImagesLen;
  unsigned char* numImagesPacket = vectorToBuffer(numImagesVect,numImagesLen);
  write(newsockfd, numImagesPacket, numImagesLen);
  delete[] numImagesPacket;
  
  //transmit the images
  for (int i = 0; i < packets.size(); i++)
  {
    //this part gets parallelized
    int headerLen;
    unsigned char* header = vectorToBuffer(headerPackets[i], headerLen);
    write(newsockfd, header, headerLen);
    delete[] header;
    int packetLen;
    unsigned char* packet = vectorToBuffer(packets[i], packetLen);
    write(newsockfd, packet, packetLen);
    delete[] packet;
    //wait for client to respond
    unsigned char recieved[255];
    int dataRead = read(newsockfd, recieved, 255);
  }

  //close the network
  close(newsockfd);
  close(sockfd);
  return 0;
}

unsigned char* vectorToBuffer(const vector<unsigned char> &packet, int& sizeOut)
{
  sizeOut = packet.size();
  unsigned char* buffer = new unsigned char[sizeOut];
  for (int i = 0; i < sizeOut; i++)
    buffer[i] = packet[i];
  return buffer;
}

void error(const char* msg)
{
  perror(msg);
  exit(1);
}
