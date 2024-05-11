// /**
//  * 管道通信
//  * int pipe(int fd[2])
//  * fd[0]读数据
//  * fd[1]写数据
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <unistd.h>

// #define MAXLINE 1024

// int main(void) {
//   int n;
//   int fd[2];
//   pid_t pid;

//   if (pipe(fd) < 0) {
//     perror("pipe()");
//     exit(1);
//   }

//   while ((pid = fork()) == -1);

//   if (pid > 0) {   // 父进程
//     close(fd[0]);  // 关闭读数据的管道
//     printf("父进程正在给子进程发送消息... \n");
//     sleep(3);
//     char* buf = "hello world";
//     write(fd[1], buf, strlen(buf));
//     close(fd[1]);
//   } else {
//     close(fd[1]);  // 关闭写数据的管道
//     char buf[MAXLINE];
//     int len = read(fd[0], buf, MAXLINE);
//     buf[len] = 0;
//     printf("子进程收到父进程的消息: %s \n", buf);
//     close(fd[0]);
//   }
//   return 0;
// }

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define BUF_SIZE 1024
int main(void) {
  int fds1[2], fds2[2];
  char str1[] = "Who are you?";
  char str2[] = "我是你爸爸";
  char buf[BUF_SIZE];
  pid_t pid;
  pipe(fds1), pipe(fds2);
  while ((pid = fork()) == -1);
  if (pid == 0) {
    read(fds2[0], buf, BUF_SIZE);
    printf("子进程说: %s \n", buf);
    write(fds1[1], str2, sizeof(str2));
  } else {
    write(fds2[1], str1, sizeof(str1));
    read(fds1[0], buf, BUF_SIZE);
    printf("父进程说: %s \n", buf);
  }
  return 0;
}