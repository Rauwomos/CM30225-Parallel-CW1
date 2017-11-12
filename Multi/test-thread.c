#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void testFunc(int* id) {
  // int* id = (int*) args;
  printf("Thread: %d\n", *id);
}

int main()
{

unsigned int threadCount = 3;

unsigned int* ids = malloc(threadCount*sizeof(int));

for(unsigned int i=0; i<threadCount; i++) {
  ids[i] = i;
}

/* this variable is our reference to the second thread */
pthread_t inc_x_thread[threadCount];

for(unsigned int i=0; i<threadCount-1; i++) {
  /* create a second thread which executes inc_x(&x) */
  if(pthread_create(&inc_x_thread[i], NULL, (void*(*)(void*))testFunc, &ids[i])) {

  fprintf(stderr, "Error creating thread\n");
return 1;
}

}

printf("Thread: %d\n", ids[threadCount-1]);

for(unsigned int i=0; i<threadCount-1; i++) {
  /* wait for the second thread to finish */
  if(pthread_join(inc_x_thread[i], NULL)) {

  fprintf(stderr, "Error joining thread\n");
  return 2;

  }
}

return 0;

}