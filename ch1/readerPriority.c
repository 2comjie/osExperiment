/**
 * 读者优先
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ_COUNT 5   // 读者数量
#define WRITE_COUNT 3  // 写者数量

sem_t read_mutex;
sem_t write_mutex;
int count;

// 写者线程
void* write_thread(void* arg) {
  int write_idx = *((int*)arg);
  while (1) {
    sem_wait(&write_mutex);  // 读者写者互斥、写者写者互斥

    printf("writer %d is writing \n", write_idx);
    sleep(rand() % 10);  // 模拟写的过程
    printf("writer %d writeing down \n", write_idx);

    sem_post(&write_mutex);  // 写完退出，释放锁

    sleep(rand() % 10);  // 到下一次写书的等待时间
  }

  return NULL;
}

// 读者线程
void* read_thread(void* arg) {
  int read_idx = *((int*)arg);
  while (1) {
    sem_wait(&read_mutex);  // 读者读者对 count 互斥
    if (count == 0)
      sem_wait(&write_mutex);  // 读者写者互斥，如果当前是第一个读者，获取锁
    count++;
    sem_post(&read_mutex);  // 读者读者对 count 互斥

    printf("reader %d is reading \n", read_idx);
    sleep(rand() % 10);  // 模拟读的过程
    printf("reader %d reading down \n", read_idx);

    sem_wait(&read_mutex);
    count--;
    if (count == 0)
      sem_post(&write_mutex);  // 读者写者互斥，如果当前是最后一个读者，释放锁
    sem_post(&read_mutex);

    sleep(rand() % 10);  // 到下一次读书的时间
  }
}

int main(void) {
  count = 0;
  sem_init(&read_mutex, 0, 1);
  sem_init(&write_mutex, 0, 1);

  pthread_t readers[READ_COUNT];
  pthread_t writers[WRITE_COUNT];
  int read_idx[READ_COUNT];
  int write_idx[WRITE_COUNT];

  for (int i = 0; i < WRITE_COUNT; i++) {
    write_idx[i] = i + 1;
    pthread_create(&writers[i], NULL, write_thread, &write_idx[i]);
    pthread_detach(writers[i]);
  }

  for (int i = 0; i < READ_COUNT; i++) {
    read_idx[i] = i + 1;
    pthread_create(&readers[i], NULL, read_thread, &read_idx[i]);
    pthread_detach(readers[i]);
  }

  while (1);

  return 0;
}