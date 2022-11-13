#include <stdio.h>
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

  int fd;
  char inp_str[BUFFER] = {0};
  size_t size;
  char *token;

  if (argc < 2) {
	printf("Error: invalid number of arguments\n");
	exit(-1);
  }

  (void)umask(0); //Обнуление маски для соответствия вызову open
  if ((fd = open(argv[1], O_RDONLY, 0666)) == -1) {
  /* Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу */
  printf("Can\'t open file\n");
  exit(-1);
  }

  do {
		size = read(fd, inp_str, sizeof(inp_str));
		if (size < 0) {
			printf("Error: cannot read string\n");
			exit(-1);
		}

		if (size != 0) {
			token = strtok(inp_str, "\0");
			while (token != NULL) {
				printf("User string:\n");
				printf("%s\n", token);
				token = strtok(NULL, "\0");
			}
		}
  } while (size != 0);
  close(fd);

  printf("Correct!\n");
  exit(EXIT_SUCCESS);
}
