#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[])
{

  if (argc < 2) {
    printf("Необходимо передать аргументом количество считываемых чисел\n");
    exit(EXIT_FAILURE);
  }

  char *filename = "3.1.1.c";
  int fd_fifo;
  int count = strtol(argv[1], NULL, 10); //10 - система счисления
  int arr[count];
  sem_t *sem1, *sem2;

  if ((mkfifo("1.fifo", 0666)) == -1)
  	if (errno != EEXIST) {
    	printf("Error: cannot create FIFO\n");
    	exit(EXIT_FAILURE);
	}

  sem1 = sem_open("1.sem", O_CREAT, 0666, 0);
  if (sem1 == SEM_FAILED) {
  	printf("Error: cannot open semaphore\n");
	exit(-1);
  }

  sem2 = sem_open("2.sem", O_CREAT, 0666, 0);
  if (sem2 == SEM_FAILED) {
  	printf("Error: cannot open semaphore\n");
	exit(-1);
  }

  if ((fd_fifo=open("1.fifo", O_RDONLY)) == -1) {
    printf("Error: cannot open FIFO\n");
    exit(0);
  }

  if (sem_wait(sem1) == -1) {
  	printf("Error: cannot wait for condition\n");
	sem_close(sem1);
	sem_close(sem2);
	exit(-1);
  };

  int size = read(fd_fifo, arr, sizeof(arr));

  if (size < 0) {
  /* Если прочитать не смогли, сообщаем об ошибке и завершаем работу */
    printf("Can\'t read string\n");
    exit(-1);
  }

  for (int i = 0; i < count; i++) {printf("[ЧТЕНИЕ]: %d \n", arr[i]);}

  if (sem_post(sem2) == -1) {
  	printf("Error: cannot continue execution\n");
	sem_close(sem1);
	sem_close(sem2);
	exit(-1);
  };

  sem_close(sem1);
  sem_close(sem2);

  return 0;
}
