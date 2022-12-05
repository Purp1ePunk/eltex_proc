// 2.1 (2 балла). Написать программу, бесконечно
// выводящую в файл 1 раз в секунду значение внутреннего
// счетчика (1 2 3 ...). Запустить ее в фоновом режиме
// (myprogram &). Узнать идентификатор процесса и
// протестировать команды управления (SIGINT, SIGQUIT,
// SIGABRT, SIGKILL, SIGTERM, SIGTSTP, SIGSTOP, SIGCONT).

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
  int counter = 0;
  int fd;

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
