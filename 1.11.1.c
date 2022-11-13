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

  if (argc < 2) {
    printf("Необходимо передать аргументом количество считываемых чисел\n");
    exit(EXIT_FAILURE);
  }

  int fd_fifo;
  int ready = 0;
  int count = strtol(argv[1], NULL, 10); //10 - система счисления
  int arr[count];

  fd_fifo=open("1.fifo", O_RDONLY);

  if (fd_fifo == -1) {
    printf("Невозможно открыть fifo\n");
    exit(0);
  }

  printf("Введите любое число для чтения:");
  scanf("%d", &ready);
  int size = read(fd_fifo, arr, sizeof(arr));

  if (size < 0) {
  /* Если прочитать не смогли, сообщаем об ошибке и завершаем работу */
    printf("Can\'t read string\n");
    exit(-1);
  }

  for (int i = 0; i < count; i++) {printf("[ЧТЕНИЕ]: %d \n", arr[i]);}
  return 0;
}
