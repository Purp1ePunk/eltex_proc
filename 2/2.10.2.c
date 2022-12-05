/* Прием сообщения из очереди */
#include <stdio.h>
#include <mqueue.h>
#define QUEUE_NAME "/q_posix"
#define PRIORITY 1
#define SIZE 256
int main(){
  mqd_t ds;
  char new_text[SIZE];
  struct mq_attr attr, old_attr;
  int prio;

  if ((ds = mq_open(QUEUE_NAME, O_RDWR | O_NONBLOCK, 0644, NULL)) == (mqd_t)-1){
    perror("Creating queue error");
    return -1;
  }

  attr.mq_flags = 0; /* set !O_NONBLOCK */
  if (mq_setattr(ds, &attr, NULL)){
    perror("mq_setattr");
    return -1;
  }

  if (mq_getattr(ds, &old_attr)) {
    perror("mq_getattr");
    return -1;
  }

  if (!(old_attr.mq_flags & O_NONBLOCK)) printf("O_NONBLOCK not set\n");

  if (mq_receive(ds, new_text, SIZE, &prio) == -1){
    perror("cannot receive");
    return -1;
  }

  printf("Message: %s, prio = %d\n", new_text, prio);

  if (mq_close(ds) == -1) perror("Closing queue error");
  if (mq_unlink(QUEUE_NAME) == -1) perror("Removing queue error");

  return 0;
}
