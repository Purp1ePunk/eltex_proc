// 1.9 (2 балла). Скорректировать решение задачи 1.8 так, чтобы использовался
// двунаправленный обмен: процесс-родитель отправляет дочернему процессу
// ответное сообщение. В ответе - число, умноженное на 2.

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
	exit(-1);
  }

  int fd[2], send_fd[2];
  int count = strtol(argv[1], NULL, 10); //10 - система счисления

  printf("COUNT: %d\n", count);
  if (count < 0) {
  	printf("Ошибка: некорректное количество чисел\n");
	exit(-1);
  }

  srand(time(NULL));
  int arr[count];

  if ((pipe(fd) < 0) || (pipe(send_fd))) {
    printf("Невозможно создать pipe\n");
    exit(-1);
  }

  pid_t p = fork();

  if (p == -1) {printf("ERROR!"); exit(EXIT_FAILURE);}
  if (p == 0) {
    //Child
    close(fd[0]); //Только передаем
    close(send_fd[1]);
    for (int i = 0; i < count; i++) {arr[i] = rand()%10; printf("[CHILD]: %d \n", arr[i]);}

    if (write(fd[1], arr, sizeof(arr)) != count*sizeof(int)) {
      close(fd[1]); printf("Некорректная запись: %d\n", count);
      exit(EXIT_FAILURE);
    }
    else close(fd[1]);

    read(send_fd[0], arr, sizeof(arr));
    for (int i = 0; i < count; i++) {
      printf("[CHILD] Квадраты чисел: %d \n", arr[i]);
    }
  }

  else {
    int arr[count];
    close(fd[1]); //Только принимаем.
    close(send_fd[0]);
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
      int s_size = sprintf(temp, "%d ", arr[i]);
      write(filewr, temp, s_size);
      arr[i] = arr[i] * 2;
    }

    if (write(send_fd[1], arr, sizeof(arr)) != count*sizeof(int)) {
      close(send_fd[1]); printf("Некорректная запись. %d", count);
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
