// 2.5 (1 балл). Изменить программу из п. 2.4 так, чтобы она
// игнорировала сигнал SIGINT и SIGQUIT, если в это время идет
// работа с файлом.

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


int main(int argc, char *argv[])
{

  int fd;
  int counter = 0;

  while (true) {
    if ((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
      perror("An error wile opening file!");
      exit(EXIT_FAILURE);
    }
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    char temp[sizeof(int)+1];
    int test = sprintf(temp, "%d ", counter);
    write(fd, temp, test);
    sleep(1);
    close(fd);
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    counter++;
    sleep(1);
  }

  return 0;
}
