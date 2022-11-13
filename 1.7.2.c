#include <stdio.h>
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

  char inp_str[BUFFER];
  size_t size;

  if (argc < 2) {
	printf("Error: invalid number of arguments\n");
	exit(-1);
  }

  FILE* file = fopen(argv[1], "r");
  	while (fgets(inp_str, sizeof(inp_str), file) != NULL)
		printf("User input --> %s", inp_str);
  fclose(file);

  printf("Correct!\n");
  exit(EXIT_SUCCESS);
}
