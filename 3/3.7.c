
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

  if ((key = ftok("3.7.c", 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get semid\n");
    exit(-1);
  }

  //Выделяем 16 байт, так как ограничимся размерностью [1, 100]
  if ((shmid = shmget(key, 16, 0666 |IPC_CREAT|IPC_EXCL)) < 0) {
    if (errno != EEXIST) {
      printf("Can\'t create shared memory\n");
      exit(-1);
    }
    else {
      //Если память уже создана, то просто пытается получить дескриптор
      if ((shmid = shmget(key, 16, 0)) < 0) {
        printf("Can\'t find shared memory\n");
        exit(-1);
      }
      printf("Attached to existed memory\n");
    }
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

      if ((shm_ptr = (int *)shmat(shmid, NULL, 0)) == (int*)-1) {
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

      if (shmdt(shm_ptr) == -1) {
        printf("Can't detach memory!\n");
        exit(EXIT_FAILURE);
      }

      if (semop(semid, &incr, 1) < 0) {
        printf("Can\'t wait for an condition!\n");
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
        printf("Can\'t wait for an condition!\n");
        exit(-1);
      }

      if ((shm_ptr = (int *)shmat(shmid, NULL, 0)) == (int*)(-1)) {
        printf("Can't attach memory\n");
        exit(EXIT_FAILURE);
      }
      //
      // if (semop(semid, &decr, 1) < 0) {
      //   printf("Can\'t wait for condition!!\n");
      //   exit(-1);
      // }

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

      if(shmdt(shm_ptr) == -1) {
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
