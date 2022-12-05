#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER 256
// Задание 1.7 (1 балл). Написать 2 программы: первая
// дописывает в файл строку, введенную пользователем. Вторая
// выводит содержимое файла. Имя файла указывается в
// параметрах запуска приложения. Использовать
// небуферизованный ввод-вывод.

int main(int argc, char *argv[]) {

  if (argc < 2) {
	printf("Error: invalid number of arguments\n");
	exit(-1);
  }

  char inp_str[BUFFER]; //Вводимая строка
  printf("Enter the string: ");
  fgets(inp_str, BUFFER, stdin);
  size_t len_str = strlen(inp_str);

  FILE* file = fopen(argv[1], "a+");

  if (file) {
    fputs(inp_str, file);
    fclose (file);
    printf("Success!\n");
  }
  else {
    printf("Error! Can\'t open file to write");
    exit(-1);
  }

  exit(EXIT_SUCCESS);
}
