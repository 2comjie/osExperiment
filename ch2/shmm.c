#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_SIZE 1024  // 共享内存大小

int main() {
  int shmid;
  key_t key = 5678;  // 共享内存的键，任意选择，只需确保唯一
  char *data;
  pid_t pid;

  // 创建共享内存
  if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  // 将共享内存段附加到父进程的地址空间
  data = shmat(shmid, NULL, 0);
  if (data == (char *)(-1)) {
    perror("shmat");
    exit(1);
  }

  // 创建一个子进程
  while ((pid = fork()) == -1);

  if (pid < 0) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {  // 子进程
    printf("子进程正在读取共享内存数据...\n");

    // 将共享内存段附加到子进程的地址空间
    data = shmat(shmid, NULL, 0);
    if (data == (char *)(-1)) {
      perror("shmat");
      exit(1);
    }

    printf("读取的数据: %s\n", data);  // 读取父进程写入的数据

    // 将共享内存段从子进程的地址空间分离
    if (shmdt(data) == -1) {
      perror("shmdt");
      exit(1);
    }

    exit(0);
  } else {  // 父进程
    // 父进程写数据到共享内存
    printf("父进程正在写数据到共享内存...\n");
    strcpy(data, "Hello, World!");

    // 等待子进程完成
    wait(NULL);

    // 将共享内存段从父进程的地址空间分离
    if (shmdt(data) == -1) {
      perror("shmdt");
      exit(1);
    }

    // 删除共享内存段
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
      perror("shmctl");
      exit(1);
    }
  }

  return 0;
}