#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include<sys/stat.h>

#define N 256

void list(int);
void get_file(int,char*);

int main(){
	int ser_sockfd,cli_sockfd;
	socklen_t ser_addr_len,cli_addr_len;
	struct sockaddr_in ser_addr,cli_addr;
	char cmd[N];
	memset(cmd,N,0);
	if((ser_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("ser socket error!!\n");
		return -1;
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(20021);
	ser_addr_len = sizeof(ser_addr);
	memset(&ser_addr,sizeof(ser_addr),0);
	if((bind(ser_sockfd,(struct sockaddr*)&ser_addr,ser_addr_len)) < 0){
		printf("ser bind error!\n");
		return -1;
	}

	if(listen(ser_sockfd,5) < 0){
		printf("ser listen error!!\n");
		return -1;
	}
	cli_addr_len = sizeof(cli_addr);
	memset(&cli_addr,cli_addr_len,0);
	while(1){
		printf("server>\n");
		if((cli_sockfd = accept(ser_sockfd,(struct sockaddr*)&cli_addr,&cli_addr_len)) < 0){
			printf("ser accept error!!\n");
			exit(1);
		}
		if(read(cli_sockfd,cmd,N) < 0){
			printf("ser read error!!\n");
			exit(1);
		}
		if(strncmp(cmd,"list",4) == 0){
			list(cli_sockfd);
		}else if(strncmp(cmd,"get",3) == 0){
			get_file(cli_sockfd,cmd + 4);
		}else{
			printf("user cmd error!!\n");
		}
	}
	return 0;
}

void list(int sockfd){
	DIR * ser_dir = NULL;
	struct dirent *ser_item = NULL;
	char dir_buf[256];
	memset(dir_buf,N,0);
	if((ser_dir = opendir(".")) == NULL){
		printf("ser current dir open woring !\n");
		exit(1);
	}
	while((ser_item = readdir(ser_dir)) != NULL){
		sprintf(dir_buf,ser_item->d_name,N);
		write(sockfd,dir_buf,N);
	}
	closedir(ser_dir);
	close(sockfd);
}

void get_file(int sockfd,char *filename){
	int file_fd,file_bytes,file_of_size = 0;
	struct stat statbuf;
	stat(filename,&statbuf);
	file_of_size = statbuf.st_size;
	char data[N];
	memset(data,N,0);
	memcpy(data,&file_of_size,sizeof(int));
	write(sockfd,data,sizeof(int));
	if((file_fd = open(filename,O_RDONLY)) < 0){
		printf("ser  open file error!!\n");
		exit(1);
	}
	memset(data,N,0);
	while((file_bytes = read(file_fd,data,N)) > 0){
		if(write(sockfd,data,file_bytes) < 0){
			printf("ser trans file error!!\n");
			close(file_fd);
			exit(1);
		}
	}
	close(file_fd);
	close(sockfd);
	return;
}
