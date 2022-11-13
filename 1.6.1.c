#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER 256
// Задание 1.6 (1 балл). Написать 2 программы: первая
// дописывает в файл строку, введенную пользователем. Вторая
// выводит содержимое файла. Имя файла указывается в
// параметрах запуска приложения. Использовать
// небуферизованный ввод-вывод.

int main(int argc, char *argv[]) {
  if (argc < 2) {printf("There's no enough arguments. Next time put the file name.\n"); exit(-1);}
  int fd;
  char inp_str[BUFFER];
  fgets(inp_str, BUFFER, stdin);
  size_t len_str = strlen(inp_str);

  (void)umask(0);
  if ((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
  printf("Can't open file\n");
  exit(-1);}


  if (write(fd, inp_str, len_str) == len_str) {printf("Correct!");}
  if (close(fd) != 0) {printf("Close error!");}
  exit(EXIT_SUCCESS);
}
