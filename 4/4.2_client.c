#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

int sockfd;

void sig_handler(int sig)
{
  shutdown(sockfd, SHUT_WR);
  close(sockfd);
}

int main(int argc, char **argv)
{
    signal(SIGINT, sig_handler);
    int n, len; /* Переменные для различных длин и количества символов */
    char sendline[1000], recvline[1000]; /* Массивы для отсылаемой и принятой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */
    /* Сначала проверяем наличие ip и порта в
    командной строке. При их отсутствии прекращаем работу */
    if(argc != 3)
    {
        printf("Usage: client.out <IP address> <serv_port num>\n");
        exit(1);
    }
    /*Читаем номера портов*/
    uint16_t server_port = strtol(argv[2], NULL, 10);

    /* Создаем UDP сокет */
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(1);
    }
    /* Заполняем структуру для адреса клиента */
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &cliaddr,
    sizeof(cliaddr)) < 0)
    {
        perror(NULL);
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(1);
    }
    /* Заполняем структуру для адреса сервера */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(1);
    }
    while(1)
    {
        /* Вводим строку, которую отошлем серверу */
        printf("String => ");
        fgets(sendline, 1000, stdin);
        printf("\n");
        /* Отсылаем датаграмму */
        if(sendto(sockfd, sendline, strlen(sendline)+1,
        0, (struct sockaddr *) &servaddr,
        sizeof(servaddr)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        /* Ожидаем ответа и читаем его. Максимальная
        допустимая длина датаграммы – 1000 символов,
        адрес отправителя нам не нужен */
        if((n = recvfrom(sockfd, recvline, 1000, 0,
        (struct sockaddr *) NULL, NULL)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        /* Печатаем пришедший ответ*/
        printf("%s\n", recvline);
    }
    return 0;
}
