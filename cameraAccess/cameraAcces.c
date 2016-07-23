#include <stdio.h>
#include <stdlib.h>
int main (int argc, char** argv)
{
  /*if (argc < 3)
  {
    printf("syntax error: %s <res1> <res2>\n", argv[0]);
    exit(-1);
  }
  int size = atoi(argv[1]) * atoi(argv[2]);
  FILE *camera, *grab;
  camera = fopen("/dev/video1", "rb");
  grab = fopen("grab.raw", "wb");
  float data[size];
  fread(data, sizeof(data[0]), size, camera);
  fwrite(data, sizeof(data[0]), size, grab);
  fclose(camera);
  fclose(grab);
  return 0;*/
  FILE* camera;
  FILE* toWrite;
  long size;
  char* buffer;
  size_t result;

  camera = fopen("/dev/video1", "rb");
  if (camera==NULL)
  {
    fputs("File error", stderr);
    exit(-1);
  }

  toWrite = fopen("grab.raw", "wb");

  //obtain file size;
  fseek(camera, 0, SEEK_END);
  size = ftell(camera);
  printf("size: %li\n", size);
  rewind(camera);

  //allocate memory to contain the wholefile
  buffer = (char*) malloc (sizeof(char)*size);
  if (buffer == NULL)
  {
    fputs("Memory error", stderr);
    exit(-1);
  }

  //copy the file into the buffer
  result = fread(buffer, 1, size, camera);
  if (result != size)
  {
    fputs("Reading error", stderr);
    exit(1);
  }
  fwrite(buffer, 1, size, toWrite);

  fclose(camera);
  fclose(toWrite);
  free(buffer);
  return 0;
}

//possibly need to add a readData function for arbitrary amount of data
//read from fread
