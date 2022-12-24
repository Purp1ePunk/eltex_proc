#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// прототип функции, обслуживающей
// подключившихся пользователей
void dostuff(int);

// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// глобальная переменная – количество
// активных пользователей
int nclients = 0;

// макрос для печати количества активных
// пользователей
void printusers()
{
	if(nclients)
	{printf("%d user on-line\n",nclients);}
	else {printf("No User on line\n");}
}

int sum(int a, int b)
{
	return a + b;
}
int prod(int a, int b)
{
	return a * b;
}
int diff(int a, int b)
{
	return a - b;
}
int dv(int a, int b)
{
	return a / b;
}



int main(int argc, char *argv[])
{
    char buff[1024];
	printf("TCP SERVER \n");

	int sockfd, newsockfd; // дескрипторы сокетов
	int portno; // номер порта
	int pid; // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента

	// ошибка в случае если мы не указали порт
    if (argc < 2)
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // Шаг 1 - создание сокета
	// AF_INET     - сокет Интернета
    // SOCK_STREAM  - потоковый сокет (с
    //      установкой соединения)
    // 0 - по умолчанию выбирается TCP протокол
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ошибка при создании сокета
	if (sockfd < 0)
       error("ERROR opening socket");

	// Шаг 2 - связывание сокета с локальным адресом
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = strtol(argv[1], NULL, 10);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    // вызываем bind для связывания
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");
    // Шаг 3 - ожидание подключений, размер очереди - 5
	listen(sockfd,5);
    clilen = sizeof(cli_addr);

	// Шаг 4 - извлекаем сообщение из очереди
	// цикл извлечения запросов на подключение из очереди
	while (1)
	{
         newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
	nclients++; // увеличиваем счетчик
              // подключившихся клиентов

		// вывод сведений о клиенте
		// struct hostent *hst;
		// hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
		// printf("+%s [%s] new connect!\n",
		// (hst) ? hst -> h_name : "Unknown host",
		// (char*)inet_ntoa(cli_addr.sin_addr));
	    printusers();

         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)
		 {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

void dostuff (int sock)
{
	//int n,a,b;
	int bytes_recv; // размер принятого сообщения
	int a,b; // переменные для myfunc
    char c;
	char buff[20 * 1024];
	#define str1 "1\r\n"
	#define str2 "Enter 2 parametr\r\n"
    #define str3 "Enter the operation (s for sum, p for production, d for diff, any string for division)\r\n"

	// отправляем клиенту сообщение
   	 write(sock, str1, sizeof(str1));

   	// обработка первого параметра
	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	a = strtol(buff, NULL, 10); // преобразование первого параметра в int

	// отправляем клиенту сообщение
	write(sock,str2,sizeof(str2));

	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	b = strtol(buff, NULL, 10); // преобразование второго параметра в int
    // отправляем клиенту сообщение
	write(sock,str3,sizeof(str3));

	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
    printf("%c\n", buff[0]);
    c = buff[0];

    if(c == 's')
	    a = sum(a,b); // вызов пользовательской функции

    else if(c == 'p')
        a = prod(a,b);

    else if(c == 'd')
        a = diff(a,b);

    else
        a = dv(a,b);

	sprintf(buff,"%d", a); // преобразование результата выполнения в строку
	buff[strlen(buff)] = '\n'; // добавление к сообщению символа конца строки

	// отправляем клиенту результат
	write(sock,&buff[0],sizeof(buff));

	nclients--; // уменьшаем счетчик активных клиентов
    	printf("-disconnect\n");
	printusers();
	return;
}
