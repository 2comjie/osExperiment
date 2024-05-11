// 某寺庙，有小，老和尚若干，有一个水缸，有小和尚打水入缸供老和尚取水饮用。
// 水缸可以放10桶水。水桶总数为3个。
// 每次入、取缸水只能是1桶，且不可以同时进行。
// 小和尚从一个井里面打水。水井狭窄，每次只能容纳一个桶取水。
// 试用信号量和P，V操作给出小和尚打水入缸和老和尚从缸里取水的算法描述

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define YOUNG 20
#define OLD 20

sem_t mutex1;
sem_t mutex2;
sem_t empty;
sem_t full;
sem_t bucket;

int water;

void* young_thread(void* arg) {
  int idx = *((int*)arg);
  while (1) {
    sem_wait(&empty);
    sem_wait(&bucket);
    sem_wait(&mutex1);
    printf("小和尚 %d 正在从水井里面打水 \n", idx);
    sleep(rand() % 10);
    sem_post(&mutex1);
    sem_wait(&mutex2);
    printf("小和尚 %d 正在往水缸里倒水，水缸有 %d 的水 \n", idx, ++water);
    sleep(rand() % 10);
    sem_post(&mutex2);
    sem_post(&bucket);  // 还桶
    sem_post(&full);    // 通知老和尚
  }

  return NULL;
}

void* old_thread(void* arg) {
  int idx = *((int*)arg);
  while (1) {
    sem_wait(&full);
    sem_wait(&bucket);
    sem_wait(&mutex2);
    printf("老和尚 %d 正在从水缸里打水，水缸有 %d 的水 \n", idx, --water);
    sleep(rand() % 10);
    sem_post(&mutex2);
    sem_post(&bucket);  // 还桶
    sem_post(&empty);   // 通知小和尚
  }
}

int main(void) {
  water = 0;
  sem_init(&empty, 0, 10);
  sem_init(&full, 0, 0);
  sem_init(&mutex1, 0, 1);
  sem_init(&mutex2, 0, 1);
  sem_init(&bucket, 0, 3);

  pthread_t young[YOUNG];
  pthread_t old[OLD];

  int youn_idx[YOUNG];
  int old_idx[OLD];

  for (int i = 0; i < YOUNG; i++) {
    youn_idx[i] = i;
    pthread_create(&young[i], NULL, young_thread, &youn_idx[i]);
    pthread_detach(young[i]);
  }

  for (int i = 0; i < OLD; i++) {
    old_idx[i] = i;
    pthread_create(&old[i], NULL, old_thread, &old_idx[i]);
    pthread_detach(old[i]);
  }

  while (1);

  return 0;
}