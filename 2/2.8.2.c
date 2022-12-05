#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_MSG_LEN 1024
#define LAST_MESSAGE 255 

int main(int argc, char *argv[]) {

	char *path = "2.8.1.c";
	key_t key; 
	int msqid;
	char msg[MAX_MSG_LEN] = {0};

	struct msgbuf
	{
		long mtype;
		char mtext[MAX_MSG_LEN];
	} buf;

	if ((key = ftok(path, 0)) == -1) {
		printf("Error: cannot generate new key\n");
		exit(-1);
	}

	if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
		printf("Error: cannot get message id\n");
		exit(-1);
	}

	buf.mtype = 2;
	while ((buf.mtype != LAST_MESSAGE)) {
		if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, IPC_NOWAIT) == -1)
			if (errno != ENOMSG) {
				printf("Error: cannot receive message for queue\n");
				msgctl(msqid, IPC_RMID, NULL);
				exit(-1);
			} 

		if ((errno != ENOMSG) && (buf.mtype != LAST_MESSAGE)) {
			if (buf.mtype != 2)
				printf("<Client1>: %s", buf.mtext);
			else 
				if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
					printf("Error: cannot send message to queue\n");
					msgctl(msqid, IPC_RMID, NULL);
					exit(-1);
				}
		}

		if (buf.mtype != LAST_MESSAGE) {
			printf("Enter your message: ");
			fgets(msg, sizeof(msg), stdin);

			if (msg[0] != '\n') {
				buf.mtype = 2;
				strcpy(buf.mtext, msg);
			}
			else buf.mtype = LAST_MESSAGE;

			if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
				printf("Error: cannot send message to queue\n");
				msgctl(msqid, IPC_RMID, NULL);
				exit(-1);
			}

			errno = 0;
		}

		if (buf.mtype == LAST_MESSAGE) 
			printf("End of the chat\n");
	}

	return 0;
}
