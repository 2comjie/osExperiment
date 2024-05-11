/**
 * 苹果橘子问题
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t apple;
sem_t orange;
sem_t plate;
sem_t mutex;

int apple_count;
int oragne_count;
int plate_count;

void* father_thread(void* arg) {
  while (1) {
    sem_wait(&plate);
    sem_wait(&mutex);  // 盘子是临界资源
    sem_post(&apple);

    apple_count++;
    plate_count--;
    printf("爸爸放了一个苹果，盘子数量：%d，苹果数量：%d，橘子数量：%d \n",
           plate_count, apple_count, oragne_count);

    sem_post(&mutex);  // 盘子是临界资源
    sleep(rand() % 10);
  }

  return NULL;
}

void* mother_thread(void* arg) {
  while (1) {
    sem_wait(&plate);
    sem_wait(&mutex);  // 盘子是临界资源
    sem_post(&orange);

    oragne_count++;
    plate_count--;
    printf("妈妈放了一个橘子，盘子数量：%d，苹果数量：%d，橘子数量：%d \n",
           plate_count, apple_count, oragne_count);

    sem_post(&mutex);  // 盘子是临界资源
    sleep(rand() % 10);
  }

  return NULL;
}

void* son_thread(void* arg) {
  while (1) {
    sem_wait(&orange);
    sem_wait(&mutex);

    oragne_count--;
    plate_count++;
    printf("儿子拿了一个橘子，盘子数量：%d，苹果数量：%d，橘子数量：%d \n",
           plate_count, apple_count, oragne_count);

    sem_post(&mutex);
    sem_post(&plate);

    sleep(rand() % 10);
  }
  return NULL;
}

void* daughter_thread(void* arg) {
  while (1) {
    sem_wait(&apple);
    sem_wait(&mutex);

    apple_count--;
    plate_count++;
    printf("女儿拿了一个苹果，盘子数量：%d，苹果数量：%d，橘子数量：%d \n",
           plate_count, apple_count, oragne_count);

    sem_post(&mutex);
    sem_post(&plate);

    sleep(rand() % 10);
  }
  return NULL;
}

int main(void) {
  sem_init(&apple, 0, 0);
  sem_init(&orange, 0, 0);
  sem_init(&plate, 0, 2);
  sem_init(&mutex, 0, 1);
  apple_count = 0;
  oragne_count = 0;
  plate_count = 2;

  pthread_t father, mother, son, daughter;
  pthread_create(&father, NULL, father_thread, NULL);
  pthread_create(&mother, NULL, mother_thread, NULL);
  pthread_create(&son, NULL, son_thread, NULL);
  pthread_create(&daughter, NULL, daughter_thread, NULL);

  pthread_detach(father);
  pthread_detach(mother);
  pthread_detach(son);
  pthread_detach(daughter);

  while (1);

  return 0;
}
