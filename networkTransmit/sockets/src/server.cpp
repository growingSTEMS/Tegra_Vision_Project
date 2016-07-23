#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>

int readFromSocket(int sockfd, char* &buffer, int blockSize = 256, char exitChar = 0xFF);

void error(const char* msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char** argv)
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

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

  char* buffer; //create buffer for data to be read to
  printf("Reached here\n");
  char notUsed[256];
  fgets(notUsed, 255, stdin);
  int dataRead = readFromSocket(newsockfd, buffer, 10);
  //int dataRead = read(newsockfd, buffer, 255);
  printf("data read: %i\n", dataRead);
  for (int i = 0; i < dataRead; i++)
    printf("%c", buffer[i]);
  printf("\n");
  close(newsockfd);
  close(sockfd);
  return 0;
}

int readFromSocket(int sockfd, char* &buffer, int blockSize, char exitChar)
{
  /* Function will read an arbitrary ammount of data from a socket.
   * Function will return the amount of data that was read in bytes.
   * The arbitrary read will be accomplished by sequential calls to read()
   * with a predefined amount of data to be read. read() will continue
   * to be called until the amount of data that was read is less than the
   * amount it was told to read, thus signifying the end of the data stream.
   * buffer should be a pointer that has not been used to allocate memory
   * dynamically. This is due to the fact that the pointer is going to be
   * reassigned and thus it would cause a memory leak.
   * This function should only be used to read large amounts of data. It 
   * should be combined with a data scheme where you first transmit a header
   * packet to inform you of the type of data coming through.
   */
  //note to self: POINTERS ARE PASSED BY VALUE, NOT REFERENCE!!!!

  //code is getting hung up in here somewhere for some reason
  using namespace std;

  vector<char*> dataPackets; //holds the buffers returned from read() calls
  vector<int> packetSizes; //holds the size of buffers returned from read()
  bool stop = false;
  int dataRead = 0; //total amount of data read

  //read data from socket until the end has been reached
  while (!stop)
  {
    char* tempBuffer = new char[blockSize]; //holds buffer returned by read()
    int numBytesRead = read(sockfd, tempBuffer, blockSize);

    if (numBytesRead < 0)
      error("ERROR reading from socket");

    packetSizes.push_back(numBytesRead); //add amnt data read to packetSizes
    dataRead += numBytesRead; //add amnt data read to working total
    stop = numBytesRead < blockSize; //check if end of data reached
    
    //fill next element in dataPackets with the buffer returned by read()
    dataPackets.push_back(new char[numBytesRead]);
    memcpy(dataPackets.back(), tempBuffer, numBytesRead);
    delete[] tempBuffer; //prevent memory leak
    printf("dataRead:%i\n", dataRead);
    stop = numBytesRead < blockSize;
  }
  //construct return buffer
  buffer = new char[dataRead]; //construct array of appropriate size
  for (int i = 0; i < dataPackets.size(); i++)
    for (int k = 0; k < packetSizes[i]; k++)
      buffer[(i * blockSize) + k] = dataPackets[i][k];
  return dataRead;
}
