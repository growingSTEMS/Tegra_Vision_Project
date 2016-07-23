#include <cstdio>
#include <thread>
#include <atomic>

std::atomic<int> talkingStick(-1);
void callFromThread(int tid)
{
  //function is called from a thread
  while (tid != talkingStick);
  printf("Launched by thread: %i\n", tid);
  talkingStick++;
}

static const int NUM_THREADS = 10;

int main()
{
  std::thread t[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++)
  {
    t[i] = std::thread(callFromThread, i);
  }

  printf("Launched from the main\n");
  talkingStick++;

  for (int i = 0; i < NUM_THREADS; i++)
    t[i].join();
  return 0;
}
