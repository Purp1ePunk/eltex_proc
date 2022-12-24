// 3.8 (3 балла). Скорректировать решение задачи 3.7 так,
// чтобы порождались дополнительные дочерние процессы,
// находящие минимум, сумму и среднее значение элементов.

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

  if ((key = ftok("3.8.c", 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((semid = semget(key, 4, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get semid\n");
    exit(-1);
  }
  //Создаем массив из 4 семафоров: каждый отвечает за следующие действие:
  //Равен двум, если дочерний процесс не начал запрос на выделение памяти.
  //Равен одному, если дочерний процесс завершил работу с в общей памятью
  //Равен нулю, если процесс закончил работу и ожидает синхронизации с остальными.

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

  union semun {
   int val;
   struct semid_ds *buf;
   unsigned short *array;
   struct seminfo *__buf;
  };

  union semun arg;
  arg.val = 0;

  for (int i = 1; i < 5; i++) semctl(semid, i, SETVAL, arg);

  //В текущей реализации не существует явного предела максимального количества подстыкованных сегментов
  //разделяемой памяти на процесс: (SHMSEG)
  int proc_id = 0;

//  printf("proc %d\n", proc_id);

  pid_t p, t, r;

  p = fork();
  t = fork();
  r = 0;

  //p t r - Родитель
  //0 t 0- Дочерний
  //p 0 0- Второй дочерний
  //0 0 0- Третий дочерний
  //p t 0 - Четвертый дочерний

  if (t < 0) {printf("ERROR!"); exit(EXIT_FAILURE);}
  if (p < 0) {printf("ERROR!"); exit(EXIT_FAILURE);}

  if ((p != 0) && (t != 0)) {
    if ((r = fork()) < 0) {printf("ERROR r!\n"); exit(EXIT_FAILURE);}
  }

  if ((p != 0) && (t != 0) && (r != 0)) {
    //Основной процесс
    while (flag == 1) {
      sleep(1);

      for (int i = 0; i < 4; i++) {
        sync.sem_num = 0;
        if (semop(semid, &sync, 1) == -1) {
          //perror("syn ");
          printf("syn error %d \n", i+1);
          exit(-1);
        }
      }

      //printf("start!\n");

      if ((shm_ptr = (int *)shmat(shmid, NULL, 0)) == (int*)-1) {
        printf("Can't attach memory\n");
        exit(EXIT_FAILURE);
      }

      //[0] - Количество элементов
      //[arr_size+1] - поле для передачи минимума
      //[arr_size+2] - поле для передачи максимума
      //[arr_size+3] - поле для передачи суммы
      //[arr_size+4] - поле для передачи среднего значения

      if (counter > 0) {
        int arr_size = shm_ptr[0];
        printf("MIN: %d, MAX: %d\n", shm_ptr[arr_size+1], shm_ptr[arr_size+2]);
        printf("SUM: %d, AVG: %d\n", shm_ptr[arr_size+3], shm_ptr[arr_size+4]);
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

      for (int i = 0; i < 4; i++) {
        incr.sem_num = i;
        if (semop(semid, &incr, 1) == -1) {
          perror("inc: ");
          exit(-1);
        }
      }
    counter++;
    }
  }
  else {

    if ((p == 0) && (t != 0)) {proc_id = 1;}
    else if ((p != 0) && (t == 0)) {proc_id = 2;}
    else if ((p == 0) && (t == 0)) {proc_id = 3;}
    else proc_id = 4;


    decr.sem_num = proc_id-1;
    sync.sem_num = proc_id-2;

    while (flag == 1) {
      //printf("proc_id: %d\n", proc_id);

      //Заставляем процессы работать с общей памятью последоватлеьно.
      if (proc_id != 1) {
        if (semop(semid, &sync, 1) == -1) {
          //printf("proc_id: %d", proc_id);
          exit(-1);
        }
      }

      //Общая часть
      if (semop(semid, &decr, 1) == -1) {
        //printf("proc_id: %d", proc_id);
        exit(-1);
      }

      if ((shm_ptr = (int *)shmat(shmid, NULL, 0)) == (int*)(-1)) {
        printf("Can't attach memory\n");
        exit(EXIT_FAILURE);
      }

      //Дочерний
      if (proc_id == 1) {
        int max = 0;
        int arr_size = shm_ptr[0];

        //printf("NUMBERS: ");
        for (int i = 1; i < arr_size+1; i++) {
          if (shm_ptr[i] > max) max = shm_ptr[i];
          //printf("%d ", shm_ptr[i]);
        }
        shm_ptr[arr_size+2] = max;
      }

      //Второй дочерний
      if (proc_id == 2) {
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
      }

      //Третий дочерний
      if (proc_id == 3) {
        int arr_size = shm_ptr[0];
        int sum = 0;
        //printf("NUMBERS: ");
        for (int i = 1; i < arr_size+1; i++) {
          sum += shm_ptr[i];
          //printf("%d ", shm_ptr[i]);
        }
        shm_ptr[arr_size+3] = sum;
      }

      //Четвертый дочерний
      if (proc_id == 4) {
        int arr_size = shm_ptr[0];
        int sum = 0;
        //printf("NUMBERS: ");
        for (int i = 1; i < arr_size+1; i++) {
          sum += shm_ptr[i];
          //printf("%d ", shm_ptr[i]);
        }
        shm_ptr[arr_size+4] = sum/arr_size;
      }

      //Общая часть
      if(shmdt(shm_ptr) == -1) {
        printf("Can't detach memory!\n");
        exit(EXIT_FAILURE);
      }

      //Общая часть
      if (semop(semid, &decr, 1) == -1) {
        exit(-1);
      }

    counter++;
    }

    printf("[CHILD]Обработано: %d\n", counter);
  }

  return 0;
}
