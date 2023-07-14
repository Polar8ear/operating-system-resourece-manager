#define MAX_RESOURCES 5

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

int available_resources = MAX_RESOURCES;

/*When a process wishes to obtain a number of resources, it
invokes the decrease count() function: /* decrease available_resources by count
resources */

/* return 0 if sufficient resources available, */

/* otherwise return -1 */

int decrease_count(int count)

{

  if (available_resources < count)

    return -1;

  else
  {

    // available_resources -= count;

    return 0;
  }
}

// When a process wants to return a number of resources, it
// calls the increase count() function:

/* increase available_resources by count */

int increase_count(int count)
{

  available_resources += count;

  return 0;
}

// Function executed by each thread
void *threadFunction(void *threadId)
{
  long tid = (long)threadId;
  printf("Thread ID: %ld\n", tid);
  printf("Hello from thread %ld!\n", tid);
  pthread_exit(NULL);
}

int main()
{
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;

  for (t = 0; t < NUM_THREADS; t++)
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
  for (t = 0; t < NUM_THREADS; t++)
  {
    pthread_join(threads[t], NULL);
  }

  printf("All threads have completed.\n");

  return 0;
}
