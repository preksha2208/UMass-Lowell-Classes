#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// these are the globals shared by all threads including main
#define RANGE 1000000000
#define BASE 500000000
int rICrange = RANGE;
int rICbase = BASE;
int rOOCrange = RANGE;
int rOOCbase = BASE;
int wICrange = RANGE;
int wICbase = BASE;
int wOOCrange = RANGE;
int wOOCbase = BASE;
int keepgoing = 1;
int totalReaders = 0;
int totalWriters = 0;
// the global area must include semaphore declarations and declarations
// of any state variables (reader counts, total number of readers and writers

sem_t sems[5]; // need to change this number

// Use this to sleep in the threads
void threadSleep(int range, int base) {
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = (rand() % range) + base;
  nanosleep(&t, 0);
}

void *readers(void *args) {
  int id = *((int *)args);
  threadSleep(rOOCrange, rOOCbase);
  while (keepgoing) {
    // add code for each reader to enter the
    // reading area
    // the totalReaders variable must be
    // incremented just before entering the
    // reader area
    printf("Reader %d starting to read\n", id);
    threadSleep(rICrange, rICbase);
    printf("Reader %d finishing reading\n", id);
    // add code for each reader to leave the
    // reading area
    threadSleep(rOOCrange, rOOCbase);
  }
  printf("Reader %d quitting\n", id);
}

void *writers(void *args) {
  int id = *((int *)args);
  threadSleep(rOOCrange, rOOCbase);
  while (keepgoing) {
    // add code for each writer to enter
    // the writing area
    totalWriters++;
    printf("Writer %d starting to write\n", id);
    threadSleep(wICrange, wICbase);
    printf("Writer %d finishing writing\n", id);
    // add code for each writer to leave
    // the writing area
    threadSleep(wOOCrange, wOOCbase);
  }
  printf("Writer %d quitting\n", id);
}

int main(int argc, char **argv) {
  int numRThreads = 0;
  int numWThreads = 0;
  if (argc == 11) {
    rICrange = atoi(argv[1]);
    rICbase = atoi(argv[2]);
    rOOCrange = atoi(argv[3]);
    rOOCbase = atoi(argv[4]);
    wICrange = atoi(argv[5]);
    wICbase = atoi(argv[6]);
    wOOCrange = atoi(argv[7]);
    wOOCbase = atoi(argv[8]);
    numRThreads = atoi(argv[9]);
    numWThreads = atoi(argv[10]);
  } else {
    printf("Usage: %s <reader in critical section sleep range>", argv[0]);
    printf("<reader in critical section sleep base> \n\t <reader out of "
           "critical section sleep range> ");
    printf("<reader out of critical section sleepbase> \n\t <writer in "
           "critical section sleep range> ");
    printf("<writer in critical section sleep base> \n\t<writer out of "
           "critical section sleep range> ");
    printf("<writer out of critical section sleep base> \n\t <number of "
           "readers> <number of writers>\n");
    exit(-1);
  }
  // declarations for pthread arrays, one for reader threads and
  // one for writer threads
  // arrays for reader and writer thread identities. As in the
  // dining philosopher problem, arrays of int are used
  // initialize the binary semaphores used by the readers and writers
  // a for loop to create numRThread reader threads
  // a for loop to create numWThread writer threads
  // these statements wait for the user to type a character and press
  // the Enter key. Then, keepgoing will be set to 0, which will cause
  // the reader and writer threads to quit

  pthread_t pWriters[numWThreads]; // need to change this number
  pthread_t pReaders[numRthreads];

  int i;
  int writerID[5];
  int readerID[5];

  for (i = 0; i < 5; i++)
    sem_init(&[sems[i], 0, 1);  // init semaphores

  for (i = 0; i < numWThreads; i++) {  // create writer threads
      writerID[i] = i;
      if (pthread_create(&pWriters[i], 0, writers, &writerID[i]) != 0)
        perror("Pthread_create error");
  }
  for (i = 0; i < numRThreads; i++) {  // create reader threads
      readerID[i] = i;
      if (pthread_create(&pReaders[i], 0, readers, &readerID[i]) != 0)
        perror("Pthread_create error");
  }

  char buf[256];
  scanf("%s", &buf);
  keepgoing = 0;
  // two for loops to use pthread_join to wait for the reader
  // and writer threads to quit
  printf("Total number of reads: %d\nTotal number of writes: %d\n",
         totalReaders, totalWriters);

  for (i = 0; i < 5; i++) //
    pthread_join(phils[i], 0);
  return 0;
}