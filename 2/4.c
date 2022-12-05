// 2.4 (1 балл). Изменить программу из п. 2.1 так, чтобы
// она печатала сообщение о получении сигнала SIGINT или
// SIGQUIT. Использовать одну функцию-обработчик
// сигнала.

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

void sign_handler(int sig) {
  if (sig == SIGQUIT) {printf("SIGQUIT!\n");}
  if (sig == SIGINT) {printf("SIGINT\n");}
}

int main(int argc, char *argv[])
{
  signal(SIGINT, sign_handler);
  signal(SIGQUIT, sign_handler);

  int fd;
  int counter = 0;

  if ((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
    perror("An error wile opening file!");
    exit(EXIT_FAILURE);
  }

  while (true) {
    char temp[sizeof(int)+1];
    int test = sprintf(temp, "%d ", counter);
    write(fd, temp, test);
    counter++;
    sleep(1);
  }
  close(fd);
  return 0;
}
