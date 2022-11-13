// 1.8 (2 балла). Написать программу, порождающую дочерний процесс и
// использующую однонаправленный обмен данными. Процесс-потомок
// генерирует случайные числа и отправляет родителю. Родитель выводит числа
// на экран и в файл. Количество чисел задается в параметрах запуска
// приложения.

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Недостаточно аргументов: передайте кол-во чисел.\n");
    exit(EXIT_FAILURE);
  }

  int fd[2];
  int count = strtol(argv[1], NULL, 10); //10 - система счисления

  printf("COUNT: %d\n", count);
  if (count < 0) {
  	printf("Ошибка: некорректное количество чисел\n");
	exit(-1);
  }
 
  srand(time(NULL));
  int arr[count];

  if (pipe(fd) < 0) {
    printf("Невозможно создать pipe\n");
    exit(-1);
  }

  pid_t p = fork();

  if (p == -1) {printf("ERROR!"); exit(EXIT_FAILURE);}
  if (p == 0) {
    //Child
    close(fd[0]); //Только передаем
    for (int i = 0; i < count; i++) {arr[i] = rand()%1000; printf("[CHILD]: %d \n", arr[i]);}

    if (write(fd[1], arr, sizeof(arr)) != count*sizeof(int)) {
      close(fd[1]); printf("Некорректная запись.\n %d", count);
      exit(EXIT_FAILURE);
    }
    else close(fd[1]);
  }

  else {
    int arr[count];
    close(fd[1]); //Только принимаем.
    read(fd[0], arr, sizeof(arr));
    for (int i = 0; i < count; i++) printf("[PARENT]:%d \n", arr[i]);
    close(fd[0]);

    (void)umask(0);
    size_t filewr = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);

    if (filewr == -1) {
      printf("Невозможно открыть файл.");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
      char temp[sizeof(int)+1];
      int test = sprintf(temp, "%d ", arr[i]);
      write(filewr, temp, test);
      }
    }

  return 0;
}
