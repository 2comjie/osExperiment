/**
 * 哲学家就餐问题
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM 10

sem_t chopsticks[NUM];

void* eat(void* arg) {
  int idx = *((int*)arg);
  while (1) {
    if (idx % 2) {
      sem_wait(&chopsticks[idx]);
      sem_wait(&chopsticks[(idx + 1) % NUM]);
    } else {
      sem_wait(&chopsticks[(idx + 1) % NUM]);
      sem_wait(&chopsticks[idx]);
    }

    printf("哲学家 %d 正在吃饭 \n", idx);
    sleep(rand() % 10);
    printf("哲学家 %d 吃完饭了 \n", idx);

    sem_post(&chopsticks[(idx + 1) % NUM]);
    sem_post(&chopsticks[idx]);

    sleep(rand() % 10);
  }

  return NULL;
}

int main(void) {
  for (int i = 0; i < NUM; i++) sem_init(&chopsticks[i], 0, 1);
  pthread_t philosophers[NUM];
  int idx[NUM];

  for (int i = 0; i < NUM; i++) {
    idx[i] = i;
    pthread_create(&philosophers[i], NULL, eat, &idx[i]);
    pthread_detach(philosophers[i]);
  }

  while (1);
  return 0;
}
