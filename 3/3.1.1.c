 // Задание 1.11 (2 балла). Сделать две программы: первая передает в именованный
// канал случайные числа, а вторая – считывает данные и выводит на экран. Что будет
// если запустить несколько экземпляров каждой программы?

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  srand(time(NULL));

  if (argc < 2) {
    printf("Необходимо передать аргументом количество передаваемых чисел\n");
    exit(EXIT_FAILURE);
  }

  int fd_fifo;
  int count = strtol(argv[1], NULL, 10); //10 - система счисления
  int arr[count];
  char *filename = "3.1.1.c";
  key_t key;
  int semid;
  struct sembuf sem1, sem2;

  key = ftok(filename, 0);
  if (key == -1) {
      printf("Error: cannot generate key\n");
      exit(-1);
  }

  semid = semget(key, 2, 0666 | IPC_CREAT | IPC_EXCL);
  if (semid == -1) {
  	if (errno != EEXIST) {
  		printf("Error: cannot create semaphores\n");
  		exit(-1);
  	} else {
  		semid = semget(key, 2, 0);
  		if (semid == -1) {
  			printf("Error: cannot find semaphores\n");
  			exit(-1);
  		}
  	}
  }

  if ((mkfifo("1.fifo", 0666)) == -1)
  	if (errno != EEXIST) {
    	printf("Error: cannot create FIFO\n");
    	exit(EXIT_FAILURE);
	}

  if ((fd_fifo=open("1.fifo", O_WRONLY)) == -1) {
    printf("Error: cannot open FIFO\n");
    exit(0);
  }

  for (int i = 0; i < count; i++) {arr[i] = rand()%10; printf("[ЗАПИСЬ]: %d \n", arr[i]);}
  int size = write(fd_fifo, &arr, sizeof(arr));

  if (size < sizeof(arr)) {
    printf("Can\'t write string\n");
    exit(-1);
  }

  sem1.sem_op = 1;
  sem1.sem_flg = 0;
  sem1.sem_num = 0;

  sem2.sem_op = -1;
  sem2.sem_flg = 0;
  sem2.sem_num = 1;

  if (semop(semid, &sem1, 1) == -1) {
      printf("Error: cannot wait for condition\n");
      exit(-1);
  }

  if (semop(semid, &sem2, 1) == -1) {
      printf("Error: cannot wait for condition\n");
      exit(-1);
  }

  unlink("1.fifo");

  if (semctl(semid,	0, IPC_RMID, 0) == -1) {
    printf("Error: cannot remove semaphores\n");
	exit(-1);
  }

  return 0;
}
