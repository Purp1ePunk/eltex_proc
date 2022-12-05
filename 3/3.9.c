// • 3.9 – 3.10: решить задачи 3.7 – 3.8 с
// использованием разделяемой памяти POSIX.
// 3.7 (3 балла). Родительский процесс генерирует наборы из
// случайного количества случайных чисел и помещает в
// разделяемую память. Дочерний процесс находит
// максимальное и минимальное число и также помещает их в
// разделяемую память, после чего родительский процесс
// выводит найденные значения на экран. Процесс повторяется
// до получения сигнала SIGINT, после чего выводится количество
// обработанных наборов данных.


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>

#define MAX_NUMBER 1001
//Числа от 0 до 1000
int flag = 1;

void to_result(int signum) {
  flag = 0;
}

int main() {
  signal(SIGINT, to_result);
  srand(time(NULL));
  int shmid, semid; /* IPC дескриптор для области разделяемой памяти */
  key_t key;
  int* shm_ptr;
  int counter = 0;

  struct sembuf decr = {0, -1, 0};
  struct sembuf incr = {0, 2, 0};
  struct sembuf sync = {0, 0, 0};

  shmid = shm_open("3.9.c", O_CREAT | O_RDWR, 0666);

  ftruncate(shmid, 16);

  if ((key = ftok("3.9.c", 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get semid\n");
    exit(-1);
  }



  //В текущей реализации не существует явного предела максимального количества подстыкованных сегментов
  //разделяемой памяти на процесс: (SHMSEG)

  pid_t p = fork();


  if (p != 0) {
    //Основной процесс
    while (flag == 1) {
      sleep(1);

      if (semop(semid, &sync, 1) < 0) {
        printf("Can\'t wait for an condition!\n");
        exit(-1);
      }

      if ((shm_ptr = (int*)mmap(0, 16, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0)) < 0) {
        printf("Can't attach memory\n");
        exit(EXIT_FAILURE);
      }

      //[0] - Количество элементов
      //[arr_size+1] - поле для передачи максимума
      //[arr_size+2] - поле для передачи минимума

      if (counter > 0) {
        int arr_size = shm_ptr[0];
        printf("MIN: %d, MAX: %d\n", shm_ptr[arr_size+1], shm_ptr[arr_size+2]);
      }

      int arr_size = rand() % 100 + 1;
      shm_ptr[0] = arr_size;

      for (int i = 1; i < arr_size+1; i++) {
        shm_ptr[i] = rand() % MAX_NUMBER;
      }

      if (munmap(shm_ptr, 16) == -1) {
        printf("Can't detach memory!\n");
        exit(EXIT_FAILURE);
      }

      if (semop(semid, &incr, 1) < 0) {
        printf("Can\'t wait for an condition!!!\n");
        exit(-1);
      }
    counter++;
    }
    fflush(stdin);
    printf("[PARENT]Обработано: %d\n", counter);
  }

  else {
    //Дочерний
    while (flag == 1) {

      sleep(1);

      if (semop(semid, &decr, 1) < 0) {
        printf("Can\'t wait for an condition?!\n");
        exit(-1);
      }

      if ((shm_ptr = (int*)mmap(0, 16, PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0)) < 0) {
        printf("Can't attach memory\n");
        exit(EXIT_FAILURE);
      }

      int min = MAX_NUMBER, max = 0;
      int arr_size = shm_ptr[0];

      //printf("NUMBERS: ");
      for (int i = 1; i < arr_size+1; i++) {
        if (shm_ptr[i] < min) min = shm_ptr[i];
        if (shm_ptr[i] > max) max = shm_ptr[i];
        //printf("%d ", shm_ptr[i]);
      }

      shm_ptr[arr_size+1] = min;
      shm_ptr[arr_size+2] = max;
      printf("\n");

      if (munmap(shm_ptr, 16) == -1) {
        printf("Can't detach memory!\n");
        exit(EXIT_FAILURE);
      }

      if (semop(semid, &decr, 1) < 0) {
        printf("Can\'t wait for an condition!\n");
        exit(-1);
      }
    counter++;
    }

  fflush(stdin);
  printf("[CHILD]Обработано : %d\n", counter);
  }

  return 0;
}
