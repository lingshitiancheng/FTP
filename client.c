#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include<memory.h>
#define N 256
#define true 1
void help();
void list(struct sockaddr_in,char *);
void get_file(struct sockaddr_in,char *);

int main(int argc,char *argv[]){
	char cmd[N];
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	memset(&addr,addr_len,0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(20021);

	    printf("\n=---------------------欢迎使用FTP--------------------------|\n");
    	printf("|                                                           |\n");
   	    printf("|                 help:显示所有FTP服务器命令                  |\n");
    	printf("|                                                           |\n");
    	printf("|                 list : 显示FTP服务器的文件列表                |\n");
    	printf("|                                                           |\n");
    	printf("|                 get <file>：从FTP服务器下载文件             |\n");
   	    printf("|                                                           |\n");
    	printf("|-----------------------------------------------------------|\n");
	while(true){
		printf("ftp>");
		fgets(cmd,N,stdin);
		cmd[strlen(cmd) - 1] = '\0';
		if(strncmp(cmd,"help",4) == 0){
			help();
		}else if(strncmp(cmd,"list",4) == 0){
			list(addr,cmd);
		}else if(strncmp(cmd,"get",3) == 0){
			get_file(addr,cmd);
		}else{
			printf("cmd wrong!!\n");
			help();
		}
		printf("\n");
	}
	return 0;
}

void help(){
    printf("\n=---------------------欢迎使用FTP--------------------------|\n");
    printf("|                                                           |\n");
    printf("|                 help:显示所有FTP服务器命令                  |\n");
    printf("|                                                           |\n");
    printf("|                 list : 显示FTP服务器的文件列表                |\n");
    printf("|                                                           |\n");
    printf("|                 get <file>：从FTP服务器下载文件             |\n");
    printf("|                                                           |\n");
    printf("|-----------------------------------------------------------|\n");

    return;
}

void list(struct sockaddr_in addr,char * cmd){
	int sockfd;
	char dir_buf[N];
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("list socket error!\n");
		exit(1);
	}
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		printf("client connect fail!\n");
		exit(1);
	}

	if(write(sockfd,cmd,N) < 0){
		printf("send list cmd fail!\n");
		exit(1);
	}

	while(read(sockfd,dir_buf,N) > 0){
		printf("%s\n", dir_buf);
	}
	printf("\n");
	printf("\n");
	close(sockfd);
	return;
}

void get_file(struct sockaddr_in addr,char *filename){
	int file_fd,sockfd,file_data_len,file_of_size = 0;
	char file_buf[N];
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("get_file socket creat fail!\n");
		exit(1);
	}
	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0){
		printf("get_file connect fail!\n");
		exit(1);
	}

	write(sockfd,filename,N); //send filename to server
	read(sockfd,&file_of_size,sizeof(int));
	//printf("file_of_size is : %d\n",file_of_size);
	if((file_fd = open(filename + 4,O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0){
		printf("create local file error!!\n");
		exit(1);
	}
    int data_sum = 0;
	while(((file_data_len = read(sockfd,file_buf,N)) > 0) && (data_sum < file_of_size)){
		data_sum += file_data_len;
		write(file_fd,file_buf,file_data_len);
	}

	close(file_fd);
	close(sockfd);
	return;
}
