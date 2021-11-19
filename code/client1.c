#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 41229
#define serverIP "127.0.0.1"

int main() 
{
    // message buffer
    char buf[10] = {0};
    char recvbuf[10] = {0};
    
    // socket
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("Create socket fail!\n");
        return -1;
    }

    // server address
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(serverIP),
        .sin_port = htons(serverPort)
    };
    int len = sizeof(serverAddr);
    
    // connect to server
    printf("Already connect to server!\n");
    sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    
    for(int i=0;i<9;i++){
    	if(i == 2 || i == 5 || i == 8){
    	    printf(" %c\n", buf[i]);
    	}
    	else{
    	    printf(" %c |", buf[i]);
    	}		
    	if(i == 2 || i == 5){
    	    printf("-----------\n");
    	}
    }

    while (1) {
    	
        int row, col;
        printf("Please input your site (row col): ");
        while(1){
            scanf("%d %d",&row, &col);
            if(buf[3*row+col] != 'O' && buf[3*row+col] != 'X'){
      	        buf[3*row+col] = 'O';
        	break;
            }
            else{
            	printf("please input another site: ");
            }
        }
	
	for(int i=0;i<9;i++){
    	    if(i == 2 || i == 5 || i == 8){
    	    	printf(" %c\n", buf[i]);
    	    }
    	    else{
    	    	printf(" %c |", buf[i]);
    	    }
    	    if(i == 2 || i == 5){
    	    	printf("-----------\n");
    	    }
    	}
    	
        // 傳送到 server 端
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        
        // 接收到 exit 指令就退出迴圈
        if (strcmp(buf, "exit") == 0) 
            break;

        // 清空 message buffer
        memset(buf, 0, sizeof(buf));

        if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0) {
            printf("recvfrom data from %s:%d, failed!\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
            break;
        }
        
        if(strcmp(recvbuf,"p2 Win!") == 0){
      	    printf("You Loseeeee!");
      	    break;
	}
	if(strcmp(recvbuf,"p1 Win!") == 0){
      	    printf("You Winnnnn!");
      	    break;
	}
	if(strcmp(recvbuf,"Deuce!") == 0){
      	    printf("Decueeeee!");
      	    break;
	}
        
        printf("Wait for p2......\n");
        
        printf("Current state: \n");
        for(int i=0;i<9;i++){
    	    if(i == 2 || i == 5 || i == 8){
    	    	printf(" %c\n", recvbuf[i]);
    	    }
    	    else{
    	    	printf(" %c |", recvbuf[i]);
    	    }
    	    if(i == 2 || i == 5){
    	    	printf("-----------\n");
    	    }
    	}
    	
    	memcpy(buf, recvbuf, 10);
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    // 關閉 socket，並檢查是否關閉成功
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }

    return 0;
}
