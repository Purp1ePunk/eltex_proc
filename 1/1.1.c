#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
//Задание 1.1 (1 балл). Проверить работу системных вызовов get*.

int main(int argc, char* argv[]) {
  pid_t test_pid = getpid();

  printf("PID: %d\n", test_pid);
  printf("PPID: %d\n", getppid());
  printf("UID: %d\n", getuid());
  printf("EUID: %d\n", geteuid());
  printf("GID: %d\n", getgid());
  printf("SID: %d\n", getsid(test_pid));

  return 0;
}
