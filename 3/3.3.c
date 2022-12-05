// 3.3 (2 балла). Скорректировать программу 3.2 так, чтобы
// несколько процессов (ограниченное количество) могли
// читать из файла. Запись в файл возможна, когда он никем
// не читается.

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[])
{
  int semid; /* IPC дескриптор для массива IPC семафоров */
  key_t key; /* IPC ключ */

  struct sembuf decr = {0, -1, 0};
  struct sembuf incr = {0, 3, 0};
  struct sembuf sync = {0, 0, 0};

  if (argc < 2) {
    printf("Недостаточно аргументов: передайте кол-во чисел.");
    exit(EXIT_FAILURE);
  }

  if ((key = ftok("3.3", 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get semid\n");
    exit(-1);
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
  pid_t t = fork();


  if (p == -1) {printf("ERROR!"); exit(EXIT_FAILURE);}
  if (t == -1) {printf("ERROR!"); exit(EXIT_FAILURE);}

  if ((p != 0) && (t != 0)) {
    close(fd[0]); //Только передаем

    for (int i = 0; i < count; i++) {

      if (semop(semid, &incr, 1) < 0) {
        printf("Can\'t wait for a condition!\n");
        exit(-1);
      }

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
      if (semop(semid, &sync, 1) < 0) {
        printf("Can\'t wait for an condition!\n");
        exit(-1);
      }
    }
    close(fd[1]);
  }

  else if ((p != 0) && (t == 0)) {
    int parent_num;
    close(fd[1]); //Только принимаем.
    for (int i = 0; i < count; i++) {
      read(fd[0], &parent_num, sizeof(int));
      printf("[PARENT]:%d \n", parent_num);
      (void)umask(0);
      if (semop(semid, &decr, 1) < 0) {
        printf("Can\'t wait for condition!!!\n");
        exit(-1);
      }
      //kill(p, SIGUSR1);


      size_t filewr = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666);

      if (filewr == -1) {
        printf("Невозможно открыть файл.");
        exit(EXIT_FAILURE);
      }

      char wrt[sizeof(int)+1];
      int test = sprintf(wrt, "%d ", parent_num);
      write(filewr, wrt, test);
    }
    close(fd[0]);
  }

  else if ((p == 0) && ((t == 0) || (t != 0))) {
    for (int i = 0; i < count; i++) {
      if (semop(semid, &decr, 1) < 0) {
        printf("Can\'t wait for condition!!!\n");
        exit(-1);
      }
      FILE* child_read = fopen(argv[1], "a+");
      if (t == 0) {printf("[PROC4]: ");}
      else {printf("[PROC3]: ");}
      while ((fscanf(child_read, "%d", &temp)) != EOF) {printf("%d ", temp);}
      printf("\n");
      fclose(child_read);
    }
  }
  return 0;
}
