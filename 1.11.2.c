 // Задание 1.11 (2 балла). Сделать две программы: первая передает в именованный
// канал случайные числа, а вторая – считывает данные и выводит на экран. Что будет
// если запустить несколько экземпляров каждой программы?

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
  srand(time(NULL));

  if (argc < 2) {
    printf("Необходимо передать аргументом количество передаваемых чисел\n");
    exit(EXIT_FAILURE);
  }

  int ready = 0;
  int fd_fifo;
  int count = strtol(argv[1], NULL, 10); //10 - система счисления
  int arr[count];

  unlink("1.fifo");

  if ((mkfifo("1.fifo", 0666)) == -1) {
    printf("Невозможно создать FIFO.");
    exit(EXIT_FAILURE);
  }

  if ((fd_fifo=open("1.fifo", O_WRONLY)) == -1) {
    printf("Невозможно открыть fifo\n");
    exit(0);
  }

  printf("Введите любое число для записи:");
  scanf("%d", &ready);

  for (int i = 0; i < count; i++) {arr[i] = rand()%10; printf("[ЗАПИСЬ]: %d \n", arr[i]);}
  int size = write(fd_fifo, &arr, sizeof(arr));

  if (size < sizeof(arr)) {
    printf("Can\'t write string\n");
    exit(-1);
  }

  return 0;
}
