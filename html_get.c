#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>


#define PORT 80
#define MAX_BUF 2*1024*1024
int urlparser(char * url, char * url_protocol, char * url_host, char * url_link){

    char tmp_url[256];
    strcpy(tmp_url,url);
    char * split = strstr(tmp_url,"://");
    tmp_url[split-tmp_url] = '\0';
    strcpy(url_protocol,tmp_url);
    strcpy(tmp_url,url);
    char * tmp_host = strtok(tmp_url+strlen(url_protocol)+3,"/");//strlen("://") equals 3

    strcpy(url_host,tmp_host);
    /*Set Link*/
    strcpy(url_link,url+strlen(url_protocol)+strlen(tmp_host)+3);
    if(strcmp(url_protocol,"http")!=0){
        return 0 ;
    }
    return 1;

}




int gethtml(char * buf, char * url) {
    
    /*Url parser*/
    char url_protocol[6];
    char url_host[64];
    char url_link[256];
    urlparser(url,url_protocol,url_host,url_link);
    //Only allow websites in NJUPT and in Inner Network
    if( (NULL==strstr(url_host,"njupt"))&&(NULL==strstr(url_host,"10."))&&(NULL==strstr(url_host,"202.119."))){
    return 1;
    }
    //printf("%s\n%s\n%s\n",url_protocol,url_host,url_link);
    char * response_buf=buf;
    int buf_index=0;
    int current_length=0;
    /*Socket*/
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
        /*Set Timeout*/
    struct timeval timeo = {5,0};//time for timeout
    socklen_t len = sizeof(timeo);
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, len);
        /*Set Socket*/ 
    struct sockaddr_in address;
    address.sin_family = AF_INET;
            /*Get IP by host*/
        char url_host_ip[64];
        struct hostent * host = gethostbyname(url_host);
	if ( host == NULL) {
	 return 1;
	}
       	sprintf(url_host_ip,"%s",inet_ntoa(*((struct in_addr *)host->h_addr)));
    
    address.sin_addr.s_addr = inet_addr(url_host_ip);
    address.sin_port = htons(PORT);
        /*Open Socket*/
    int result = connect(sockfd , (struct sockaddr *)&address,sizeof(address));
    if (result == -1){
        if(errno == EINPROGRESS){
            fprintf(stderr,"timeout\n");
            exit(1);
        }
        perror("connect faild: ");
        exit(1);
    }
        /*Set Http Haeder*/
    char request[256];
    sprintf(request,"GET %s HTTP/1.1 \r\nHost: %s \r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.4 (KHTML, like Gecko) Chrome/22.0.1229.94 Safari/537.4\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: deflate\r\nAccept-Language: en-US,en;q=0.8\r\nConnection: close\r\n\r\n",url_link,url_host);
   //printf("%s\n",request);
        /*Send Data*/
    send(sockfd,request,strlen(request),0);
        /*Get Response*/
    while(1){
        current_length=recv(sockfd,response_buf+buf_index,sizeof(response_buf),0);
        buf_index+=current_length;
        if (current_length==0){
            break;
        }
        response_buf[buf_index]='\0';
    }
    /*Socket CLsoe*/
    close(sockfd);
	return 0;
}

int html_parser(char * buf, char * host){
    char * h_buf=buf;
    char * href_cursor=h_buf;
    char  uurl[256];
    while(1){
    char * href_begin=strstr(href_cursor,"href=\"");
    if (NULL==href_begin){
    		break;
    	}
    char * href_end=strstr(href_begin+6,"\"");
    if ((href_end-href_begin)>256){
        href_cursor=href_end+1;
    	continue;//It is too long to handle.
    }
    sprintf(uurl,"%.*s\n",(href_end-href_begin-6),href_begin+6);
    if(uurl[0]=='/'){
    	printf("http://%s%s",host,uurl);
    	}
     else if (uurl[0]=='H') {
    	printf("%s",uurl);
    	}
    href_cursor=href_end+1;
    }

    char * src_cursor=h_buf;
    while(1){
    char * src_begin=strstr(src_cursor,"src=\"");
    if (NULL==src_begin){
    		break;
    	}
    char * src_end=strstr(src_begin+5,"\"");
    if ((src_end-src_begin)>256){
        src_cursor=src_end+1;
    	continue;//It is too long to handle.
    }
    sprintf(uurl,"%.*s\n",(src_end-src_begin-5),src_begin+5);
    if(uurl[0]=='/'){
    	printf("http://%s%s",host,uurl);
    	}
    else if (uurl[0]=='H'){
    	printf("%s",uurl);
    	}
    src_cursor=src_end+1;
    }
    return 0 ;
}



int main() {
    char html_buf[MAX_BUF];
    char url[256];
    strcpy(url,"http://www.njupt.edu.cn/");
    if(0==gethtml(html_buf,url)){
   // printf("%s",html_buf);
    html_parser(html_buf,"www.njupt.edu.cn");
    }
   return 0;
}
