#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define PORT 80
#define MAX_BUF 1024*1024
void gethtml(char * buf)
{
    char * response_buf=buf;
    int buf_index=0;


    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    int current_length=0;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("202.119.230.10");
    address.sin_port=htons(PORT);

    
    int result = connect(sockfd , (struct sockaddr *)&address,sizeof(address));
    if (result == -1)
    {
        perror("connect faild: ");
        exit(1);
    }


    char request[]="GET / HTTP/1.1 \r\nHost: youth.njupt.edu.cn\r\nConnection: keep-alive\r\nUser-Agent: Mozilia/5.0\r\nAccept: text/html\r\nAccept-Encoding: deflate\r\nConnection: close\r\n\r\n";


    send(sockfd,request,strlen(request),0);

    while(1)
    {
        current_length=recv(sockfd,response_buf+buf_index,sizeof(response_buf),0);
        buf_index+=current_length;
        if (current_length==0)
        {
            break;
        }
        response_buf[buf_index]='\0';
    }
    close(sockfd);

}

int main()
{
   char html_buf[MAX_BUF];
   gethtml(html_buf);
   printf("%s",html_buf);
   return 0;
}
