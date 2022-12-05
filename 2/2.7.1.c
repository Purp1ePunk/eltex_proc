#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MSG_LEN 1024
#define LAST_MESSAGE 255 

int main(int argc, char *argv[]) {

	char *path = "2.7.1.c";
	int N;
	key_t key; 
	int msqid;
	int i;
	char msg[MAX_MSG_LEN] = {0};

	if (argc < 2) {
		printf("Error: invalid number of arguments\n");
		printf("Enter the count of sending numbers\n");
		exit(-1);
	}

	N = atoi(argv[1]);

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

	for (i = 0; i < N; i++) {
		sprintf(msg, "This is %d message\n", i);
		buf.mtype = 1;
		strcpy(buf.mtext, msg);

		if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
			printf("Error: cannot send message to queue\n");
			msgctl(msqid, IPC_RMID, NULL);
			exit(-1);
		}
	}

	buf.mtype = LAST_MESSAGE;
	if (msgsnd(msqid, &buf, 0, 0) == -1) {
		printf("Error: cannot send message to queue\n");
		msgctl(msqid, IPC_RMID, NULL);
		exit(-1);
	}

	printf("Messages sent\n");

	return 0;
}
