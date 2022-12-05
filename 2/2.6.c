// 2.6 (3 балла). Изменить программу из п. 1.8 так, чтобы
// дочерний процесс выводил информацию из файла. Если
// родительский процесс собирается изменить файл, то он
// отправляет сигнал SIGUSR1 (блокировка доступа к файлу).
// Когда родительский процесс завершил модификацию файла,
// он отправляет сигнал SIGUSR2 (разрешение доступа к файлу).
// Дочерний процесс отправляет новое число родительскому
// после того, как прочитал файл.

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>

int start = 1;

void start_send(int signum)
{
  start = 1;
  //printf("!START!\n");
}

void stop_send(int signum)
{
  //printf("!STOP!\n");
  start = 0;
}

int main(int argc, char *argv[])
{
  signal(SIGUSR1, start_send);
  signal(SIGUSR2, stop_send);

  if (argc < 2) {
    printf("Недостаточно аргументов: передайте кол-во чисел.");
    exit(EXIT_FAILURE);
  }

  int fd[2];
  int count = strtol(argv[2], NULL, 10); //10 - система счисления
  printf("COUNT: %d\n", count);
  srand(time(NULL));
  int arr[count];

  if (pipe(fd) < 0) {
    printf("Невозможно создать pipe\n");
    exit(-1);
  }

  int temp; //Используется в программах для считывания/генерации чисел.
  pid_t p = fork();

  if (p == -1) {printf("ERROR!"); exit(EXIT_FAILURE);}
  if (p == 0) {
    //Child
    close(fd[0]); //Только передаем

    for (int i = 0; i < count; i++) {
      while (start == 0) {}
      FILE* child_read = fopen(argv[1], "a+");

      printf("[CHILD] Read: ");
      while ((fscanf(child_read, "%d", &temp)) != EOF) {printf("%d ", temp);}
      printf("\n");
      fclose(child_read);

      int num = rand()%100;
      printf("[CHILD]: %d \n", num);
      if (write(fd[1], &num, sizeof(int)) != sizeof(int)) {
        close(fd[1]);
        printf("Некорректная запись.\n %d", count);
        exit(EXIT_FAILURE);
      }
    }
    close(fd[1]);
  }

  else {
    int parent_num;
    close(fd[1]); //Только принимаем.
    for (int i = 0; i < count; i++) {
      read(fd[0], &parent_num, sizeof(int));
      printf("[PARENT]:%d \n", parent_num);

      (void)umask(0);
      kill(p, SIGUSR1);
      size_t filewr = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666);

      if (filewr == -1) {
        printf("Невозможно открыть файл.");
        exit(EXIT_FAILURE);
      }

      char wrt[sizeof(int)+1];
      int test = sprintf(wrt, "%d ", parent_num);
      write(filewr, wrt, test);
      kill(p, SIGUSR2);
    }
    close(fd[0]);
  }
  return 0;
}
