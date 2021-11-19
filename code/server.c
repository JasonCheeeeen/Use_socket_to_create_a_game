#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 41229
#define serverPort2 48763

char* check(char* state){
    int proc = 0;
    if((state[0] == state[1] && state[1] == state[2] && (state[0] == 'O' || state[0] == 'X')) || (state[3] == state[4] && state[4] == state[5] && (state[3] == 'O' || state[3] == 'X')) || (state[6] == state[7] && state[7] == state[8] && (state[6] == 'O' || state[6] == 'X')) || (state[0] == state[3] && state[3] == state[6] && (state[0] == 'O' || state[0] == 'X')) || (state[1] == state[4] && state[4] == state[7] && (state[1] == 'O' || state[1] == 'X')) || (state[2] == state[5] && state[5] == state[8] && (state[2] == 'O' || state[2] == 'X')) || (state[0] == state[4] && state[4] == state[8] && (state[0] == 'O' || state[0] == 'X')) || (state[2] == state[4] && state[4] == state[6] && (state[2] == 'O' || state[2] == 'X'))){
        return "Win!";
    }
    for(int i=0;i<9;i++){
    	if(state[i] == 'O' || state[i] == 'X'){
    	    proc++;
    	}
    }
    if(proc == 9){
    	return "Deuce!";
    }
    return "continue";
}

int main(int argc , char *argv[])
{
    // message buffer
    char buf[10] = {0};

    // 建立 socket
    int socket_fd = socket(PF_INET , SOCK_DGRAM , 0);
    int socket_fd2 = socket(PF_INET , SOCK_DGRAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }
    if (socket_fd2 < 0){
        printf("Fail to create a socket.");
    }
    
    // server 地址
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(serverPort)
    };
    struct sockaddr_in serverAddr2 = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(serverPort2)
    };
    
    // 將建立的 socket 綁定到 serverAddr 指定的 port
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    if (bind(socket_fd2, (const struct sockaddr *)&serverAddr2, sizeof(serverAddr2)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd2);
        exit(0);
    }
    	
    printf("Server ready!\n");

    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    struct sockaddr_in clientAddr2;
    int len2 = sizeof(clientAddr2);
    
    // get port address
    if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0){
    	printf("fail to get client A's address");
    }
    memset(buf, 0, sizeof(buf));
    if (recvfrom(socket_fd2, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr2, &len2) < 0){
    	printf("fail to get client A's address");
    }
    memset(buf, 0, sizeof(buf));
    
    while (1) {
        // 當有人使用 UDP 協定送資料到 48763 port
        // 會觸發 recvfrom()，並且把來源資料寫入 clientAddr 當中
        if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0) {
            break;
        }

	char* tmp = check(buf);
        if (strcmp(tmp, "Deuce!") == 0) {
            sendto(socket_fd2, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr2, sizeof(clientAddr2));
            sendto(socket_fd, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
            printf("Result is DEUCE!, closing the server...\n");
            break;
        }
        if (strcmp(tmp, "Win!") == 0) {
            tmp = "p1 Win!";
            sendto(socket_fd2, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr2, sizeof(clientAddr2));
            sendto(socket_fd, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
            printf("Result is p1 Win!, closing the server...\n");
            break;
        }
        

        // 根據 clientAddr 的資訊，回傳至 client 端                
        sendto(socket_fd2, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr2, sizeof(clientAddr2));
      
        // 清空 message buffer
        memset(buf, 0, sizeof(buf));
        
        if (recvfrom(socket_fd2, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr2, &len2) < 0) {
            break;
        }

	tmp = check(buf);
        if (strcmp(tmp, "Win!") == 0) {
            tmp = "p2 Win!";
            sendto(socket_fd2, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr2, sizeof(clientAddr2));
            sendto(socket_fd, tmp, sizeof(tmp), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
            printf("Result is p2 Win!, closing the server...\n");
            break;
        }

        // 根據 clientAddr 的資訊，回傳至 client 端                
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));

        // 清空 message buffer
        memset(buf, 0, sizeof(buf));
    }
    // 關閉 socket，並檢查是否關閉成功
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }
    
    return 0;
}
