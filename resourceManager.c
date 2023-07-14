#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_RESOURCES 10
#define MAX_RESOURCES_PER_THREAD 5 // less than MAX_RESOURCES
#define THREAD_COUNT 6
#define MAX_THREAD_EXECUTION_COUNT 4 // Execution for each decrease then increase
#define SLEEP_INTERVAL_IN_MICROSECONDS 1000

int available_resources = MAX_RESOURCES;
pthread_mutex_t resourcesLock = PTHREAD_MUTEX_INITIALIZER;

const char *colorCodes[] = {
    "\033[0;31m", // Red
    "\033[0;32m", // Green
    "\033[0;33m", // Yellow
    "\033[0;34m", // Blue
    "\033[0;35m", // Magenta
    "\033[0;36m"  // Cyan
    "\033[0;96m"  // Bright Cyan
    "\033[0;91m"  // Bright Red
    "\033[0;92m"  // Bright Green
    "\033[0;93m"  // Bright Yellow
    "\033[0;94m"  // Bright Blue
    "\033[0;95m"  // Bright Magenta
};

// When a process wishes to obtain a number of resources, it
// invokes the decrease count() function:
// decrease available_resources by count resources
// return 0 if sufficient resources available,
// otherwise return -1
int decrease_count(int count, int threadId, const char *colorCode)

{

  if (available_resources < count)
  {
    return -1;
  }
  else
  {
    if (pthread_mutex_lock(&resourcesLock) != 0)
    {
      return -1;
    }

    printf("%sT%ld: Count before decrease by %d: %d\033[0m\n", colorCode, threadId, count, available_resources);

    available_resources -= count;
    printf("%sT%ld: Count after decrease: %d\033[0m\n", colorCode, threadId, available_resources);

    pthread_mutex_unlock(&resourcesLock);
    return 0;
  }
}

// When a process wants to return a number of resources, it
// calls the increase count() function:
// increase available_resources by count
int increase_count(int count, int threadId, const char *colorCode)
{
  if (pthread_mutex_lock(&resourcesLock) != 0)
  {
    return -1;
  }

  printf("%sT%ld: Count before increase by %d: %d\033[0m\n", colorCode, threadId, count, available_resources);

  available_resources += count;
  printf("%sT%ld: Count after increase: %d\033[0m\n", colorCode, threadId, available_resources);

  pthread_mutex_unlock(&resourcesLock);

  return 0;
}

// Function executed by each thread
void *threadFunction(void *threadId)
{
  long tid = (long)threadId;

  // Set a unique seed for each thread based on thread ID
  unsigned int seed = (unsigned int)(tid + 1);

  int execution_count = rand_r(&seed) % MAX_THREAD_EXECUTION_COUNT;
  const char *colorCode = colorCodes[tid % (sizeof(colorCodes) / sizeof(colorCodes[0]))];

  for (size_t i = 0; i < execution_count; i++)
  {
    int requestingCount = (rand_r(&seed) % MAX_RESOURCES_PER_THREAD) + 1;
    if (decrease_count(requestingCount, tid, colorCode) == 0)
    {

      int duration = rand_r(&seed) % SLEEP_INTERVAL_IN_MICROSECONDS;
      usleep(duration);

      increase_count(requestingCount, tid, colorCode);

      duration = rand_r(&seed) % SLEEP_INTERVAL_IN_MICROSECONDS;
      usleep(duration);
      continue;
    }

    printf("%sT%ld: Decrease count failed\033[0m\n", colorCode, tid);
  }

  pthread_exit(NULL);
}

int main()
{

  pthread_t threads[THREAD_COUNT];
  int rc;
  long t;

  for (t = 0; t < THREAD_COUNT; t++)
  {
    printf("Creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, threadFunction, (void *)t);

    if (rc)
    {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      return 1;
    }
  }

  // Wait for all threads to complete
  for (t = 0; t < THREAD_COUNT; t++)
  {
    pthread_join(threads[t], NULL);
  }

  printf("All threads have completed.\n");
  return 0;
}
