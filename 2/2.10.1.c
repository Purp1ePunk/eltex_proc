// 2.10 (4 балла). Написать две программы, использующие
// очередь сообщений: первая отправляет строки в очередь,
// а вторая – считывает и выводит на экран. Отправляемые
// строки можно читать из файла или генерировать в
// программе (например, случайные числа). Для завершения
// обмена данными первая программа должна отправить
// сообщение с типом 255.

/* Отправка сообщения в очередь */
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define QUEUE_NAME "/q_posix"
#define PRIORITY 1
#define SIZE 256
int main(int argc, char *argv[]){
  mqd_t ds;
  int fd;
  char text[SIZE];
  struct mq_attr queue_attr;
  queue_attr.mq_maxmsg = 10;
  queue_attr.mq_msgsize = SIZE;
  if ((ds = mq_open("/q_posix",
  O_CREAT | O_RDWR , 0644,
  &queue_attr)) == (mqd_t)-1){
    perror("Creating queue error");
    return -1;
  }

  if ((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
  perror("Can't open file\n");
  exit(-1);
  }

  printf("%d", read(fd, text, SIZE));
  //while ( {printf("1!");}

  if (mq_send(ds, text, strlen(text), PRIORITY) == -1){
    perror("Sending message error");
    return -1;
  }
  if (mq_close(ds) == -1) perror("Closing queue error");
  return 0;
}
