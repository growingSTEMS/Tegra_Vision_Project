#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

using namespace std;
using namespace cv;

void error(const char *msg);

int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //recieve number of images
    unsigned char numImagesPacket[sizeof(int)];
    int dataRead = read(sockfd, numImagesPacket, sizeof(int));
    if (dataRead != sizeof(int))
      error("ERROR: numImages couldn't be interpereted");
    union
    {
      //used for converting bytes to an int
      int intVal;
      unsigned char bytes[sizeof(int)];
    };
    for (int i = 0; i < sizeof(int); i++)
      bytes[i] = numImagesPacket[i];
    int numImages = intVal;
    printf("Number of images to recieve: %i\n", numImages);
    //recieve data
    for (int i = 0; i < numImages; i++)
    {
      //recieve header packet
      unsigned char header[2 * sizeof(int)];
      dataRead = read(sockfd, header, 2 * sizeof(int));
      if (dataRead != 2 * sizeof(int))
        error("ERROR: data could not be interpreted");
      for (int i = 0; i < 2 * sizeof(int); i++)
      {
        printf("header[%i]: %i\n", i, header[i]);
      }
      //reinterpret the header information into values
      printf("Reached here\n");
      vector<unsigned char> headerVect;
      for (int i = 0; i < 2 * sizeof(int); i++)
      {
        headerVect.push_back(header[i]);
      }
      //get key
      for (int i = 0; i < sizeof(int); i++)
        bytes[i] = headerVect[i];
      headerVect.erase(headerVect.begin(), headerVect.begin() + 4);
      int key = intVal;
      printf("key: %i\n", key);
      if (headerVect.size() < sizeof(int))
        error("off by one error, remove \"-1\"");
      //get length
      for (int i = 0; i < sizeof(int); i++)
        bytes[i] = headerVect[i];
      headerVect.erase(headerVect.begin(), headerVect.begin() + 4);
      printf("Final vector size: %i\n", (int)headerVect.size());
      int length = intVal;
      printf("length: %i\n", length);

      //recieve image
      char* imageBuffer = new char[length];
      dataRead = read(sockfd, imageBuffer, length);
      printf("dataRead: %i\n", dataRead);
      if (dataRead != length)
        error("ERROR reading image");
      headerVect.clear(); //ensure there is no extra data left over in vector
      for (int i = 0; i < length; i++)
        headerVect.push_back(imageBuffer[i]);
      printf("image vector size: %i\n", (int)headerVect.size());
      Mat recievedImage = imdecode(headerVect, CV_LOAD_IMAGE_COLOR);
      imshow("Recieved image 2", recievedImage);
      waitKey();
      string str = "Recieved image";
      write(sockfd, str.c_str(), str.length());
    }
    waitKey();
    close(sockfd);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
