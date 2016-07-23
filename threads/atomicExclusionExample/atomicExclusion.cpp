#include <cstdio>  //output
#include <thread>  //threads
#include <atomic>  //atomic types
#include <cstdlib> //rand, atoi
#include <ctime>   //time
#include <vector>  //vector

//aliases
template <typename T> using doubleVect = std::vector<std::vector<T>>;

std::atomic<int> sum(0);

//forward delcares
/*std::vector<int>*/void generateNumbers(int n);
void partialSum(std::vector<int>& nums);

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    printf("syntax error: %s <number of numbers> <number of threads>\n", argv[0]);
    exit(-1);
  }
  //get command line args
  const int numNums = atoi(argv[1]);
  const int numThreads = atoi(argv[2]);
  //get random numbers to use;
  //std::vector<int> nums = generateNumbers(numNums);
  //print out the numbers generated
  /*for(int num : nums)
    printf("%i, ", num);
  printf("\n");*/
  //create individual number pools
  doubleVect<int> numPools;
  for (int i = 0; i < numThreads; i++)
  {
    std::vector<int> numPool;
    numPools.push_back(numPool);
  }
  for (int i = 0; i < numThreads; i++)
  {
    int poolSize = nums.size() / numThreads; //average size of vector in numPools
    if (i == numThreads - 1)
    {
      //special case for last numPool to generate that grabs the rest of data
      //in case the number of threads does not create an even division into the 
      //number of numbers in nums
      for (int j = i * poolSize; j < nums.size(); j++)
        numPools[i].push_back(nums[j]);
    }
    else 
    {
      for (int j = i * poolSize; j < (i+1) * poolSize; j++)
        numPools[i].push_back(nums[j]);
    }
  }
  //print the number pools
  /*for (int i = 0; i < numPools.size(); i++)
  {
    printf("Displaying numbers in numPools %i:\n", i);
    for (int j = 0; j < numPools[i].size(); j++)
    {
      printf("%i, ", numPools[i][j]);
    }
    printf("\n");
  }*/
  //create threads
  std::vector<std::thread> threads;
  for (int i = 0; i < numThreads; i++)
    threads.push_back(generateNumbers(10000));
  return 0;
}

/*std::vector<int>*/void generateNumbers(int n)
{
  std::vector<int> nums; //vector to contain the numbers
  //seed the PRNG
  srand(time(0));
  //generate the random numbers
  for (int i = 0; i < n; i++)
    nums.push_back(rand() % 1000);
  //return nums;
}

void partialSum(std::vector<int> nums)
{
  int partialSum = 0;
  for (int i = 0; i < nums.size(); i++)
    partialSum += nums[i];
  sum += partialSum;
}
