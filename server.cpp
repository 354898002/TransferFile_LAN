#include <bits/stdc++.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

const int MAXBUF = 1024;

using namespace std;

int main(int argc,char **argv)
{

    if(argc != 4)
    {
        puts("Worng argument!");
        exit(EXIT_FAILURE);
    }
    
    int sockServer,sockClient;
    unsigned int myport,lisnum;
    sockaddr_in my_addr,their_addr;
    socklen_t len;    
    myport = atoi(argv[2]);
    lisnum = 5;
    
    if( (sockServer = socket(AF_INET,SOCK_STREAM,0) ) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    bzero(&my_addr,sizeof(my_addr));
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    if(bind(sockServer,(sockaddr *)&my_addr,sizeof(my_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if(listen(sockServer,lisnum) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    puts("connect me,please!");
    
    len = sizeof(sockaddr);
    
    
    if( (sockClient = accept(sockServer,(sockaddr *)&their_addr,&len) ) == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server got connection form IP : %s port : %d,socket %d\n",inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port),sockClient);
    }
    
    FILE *src;
    
    const char *path = argv[3];
    struct stat fileInfo;
    
    if(stat(path,&fileInfo) == -1)
    {
        perror("openFile");
        exit(EXIT_FAILURE);
    }
    
    if( (src = fopen(path,"r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    long total = fileInfo.st_size;
    long sum = 0,part;
    long totalRead = 0;
    
    char buffer[MAXBUF];
    
    while(feof(src) != 1)
    {
        if((part = fread(buffer,1,MAXBUF,src)) == -1)
        {
            perror("rendFile");
            exit(EXIT_FAILURE);
        }
        
        if(part)        
        {
            if((len = send(sockClient,buffer,part,0)) < 0)
            {
                perror("sendFile");
                exit(EXIT_FAILURE);
            }
        }
        
        sum += len;
        totalRead += part;
        
        printf("total = %ld send = %ld read = %ld\n",total,sum,totalRead);
    }
    
    fclose(src);
    close(sockServer);
    close(sockClient);
    
    return 0;
}
