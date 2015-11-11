#include <sys/stat.h>
#include <bits/stdc++.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int MAXBUF = 1024;

using namespace std;

void getpath(char *path,const char *prefix)
{    
    struct stat testfile;
 
    for(int i = 1;; ++i)
    {
        if(strcmp(prefix,"") == 0)
            sprintf(path,"%s%d","newfile",i);
        else
            sprintf(path,"%s/%s%d",prefix,"newfile",i);
        
        if(stat(path,&testfile) == 0)
            continue;
        if(errno == ENOENT)
             break;
        else
        {
            perror("creat file");
            exit(errno);
        }
    }
}

int main(int argc,char *argv[])
{
    if(argc != 3 && argc != 4)
    {
        puts("Wrong argument !");
        exit(EXIT_FAILURE);
    }
    
    int sockfd,len;
    
    sockaddr_in dest;
    
    if( (sockfd = socket(AF_INET,SOCK_STREAM,0) ) < 0 )
    {
        perror("Socket");
        exit(errno);    
    }
    
    printf("scoket created ! \n");
    bzero(&dest,sizeof(dest));
    
    
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    
    if(inet_aton(argv[1],(in_addr*)&dest.sin_addr.s_addr) == 0)
    {
        perror("wrong address : argv[1]");
        exit(errno);
    }
    
    if(connect(sockfd,(sockaddr *)&dest,sizeof(dest) ) == -1)
    {
        perror("Connect ");
        exit(errno);
    }
    
    puts("I am ready to recieve file!");
     
    FILE *des;
    
    char path[1024];
   
    struct stat testfile;    
    
    if(argc == 4)
    {
        strcpy(path,argv[3]);
        
//        if(stat(path,&testfile) != 0 && errno == ENOENT)
//        {
//            perror("get file state");
//            exit(EXIT_FAILURE);
//        }
 
        int res = stat(path,&testfile);
 
        if(res != 0 && errno != ENOENT)
        {
            perror("get file state ");
            exit(EXIT_FAILURE);
        }      
        
        if(res == 0 && S_ISDIR(testfile.st_mode))
        {
            getpath(path,argv[3]);       
        }
    }
    else
    {
        getpath(path,"");
    }
    
    if( (des = fopen(path,"w") ) == NULL)
    {
        perror("open file ");
        exit(EXIT_FAILURE);
    }
    
    char buffer[MAXBUF];
   
    long int totalRec = 0,totalWri = 0;
    long int part;
    while(1)
    {
        if( (len = recv(sockfd,buffer,MAXBUF,0)) == -1)
        {
            perror("recieve file");
            exit(EXIT_FAILURE);
        }
        
        if(len > 0)
        {
            if( (part = fwrite(buffer,1,len,des)) == -1)
            {
                perror("write file");
                exit(EXIT_FAILURE);
            }    
        }
        else
            break;
        
        totalRec += len;
        totalWri += part;
        
        printf("recieve = %ld write = %ld\n",totalRec,totalWri);
    }
    
    fclose(des);
    close(sockfd);
    
    return 0;
}
