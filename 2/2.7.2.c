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
	key_t key; 
	int msqid;

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
	
	buf.mtype = 1;
	while (buf.mtype != LAST_MESSAGE) {
		memset(buf.mtext, sizeof(buf.mtext), '\0');
		if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
			printf("Error: cannot receive message for queue\n");
			msgctl(msqid, IPC_RMID, NULL);
			exit(-1);
		}
		if (buf.mtype != LAST_MESSAGE)
			printf("Message type: %ld, message text: %s", buf.mtype, buf.mtext);
		else 	
			msgctl(msqid, IPC_RMID, NULL);
	}

	printf("\nMessages received\n");

	return 0;
}
