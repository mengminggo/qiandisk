#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>  // 多路复用 I/O


#include <arpa/inet.h>

#include <netinet/in.h>
//#include <netinet/ip.h>


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <set>
#include <vector>

#include <math.h>

#include <signal.h>

#include "inc/utils.h"

#include "inc/qd_types.h"

#include "inc/qd_datas.h"


#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

#define SERVER_PORT 8848
#define SERVER_ADDRESS "0.0.0.0"


int main(int argc, char* argv[]) {
    // 创建监听套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // 绑定地址和端口
    struct sockaddr_in addr;
    MMG_ZERO(addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    // 初始化文件描述符集合
    fd_set cliFds;       // int_64[]
    FD_ZERO(&cliFds);    // 清空, 全置0
    FD_SET(server_fd, &cliFds);  // 添加socket描述符, 置1

    int max_fd = server_fd;  //

    printf("Server started at port %d\n", ntohs(addr.sin_port));

    // 解决无效遍历已关闭的的连接的问题 by 2023.1.31
    std::vector<int> avaFds;

    std::vector<ClientInfo> cliInfos;

    int count = 10;
    while (count--) {
        // 复制文件描述符集合，因为 select 会改变原始的集合
        fd_set tmp_fds = cliFds;

        // 调用 select 函数等待事件发生, 检测集合中所有的 socket有没有改变
        int nready = select(max_fd + 1, &tmp_fds, NULL, NULL, NULL);
        printf("select %d ok\n", nready);
        if (nready == -1) {
            perror("select() failed");
            exit(EXIT_FAILURE);
        }

        // 检测到 server_fd 变化, 接收 cli sockfd
        if(FD_ISSET(server_fd, &tmp_fds)){
            printf("server_fd:%d is change\n",server_fd);

            struct sockaddr_in client_addr;
            socklen_t client_addrlen = sizeof(client_addr);

            MMG_ZERO(client_addr);

            //recv client sockfd
            int cli_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addrlen);
            printf("Get client Sockfd: %d\n", cli_fd);

            if (cli_fd == -1) {
                // perror("accept() failed");
                // exit(EXIT_FAILURE);
            }

            char    client_ip[INET_ADDRSTRLEN];
            int     client_port = ntohs(client_addr.sin_port);
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            printf("New connection from %s:%d (socket=%d)\n",
                    client_ip, client_port, cli_fd);


            // 添加到描述符数组中
            FD_SET(cli_fd, &cliFds);

            avaFds.push_back(cli_fd);

            // 待使用
            // cliInfos.push_back(ClientInfo(000,cli_fd,client_ip,client_port));

            // select 监听, 从 0 到最大的一位 fd
            if (cli_fd > max_fd) {
                max_fd = cli_fd;
            }

        }


        // 处理事件
        // 遍历avaFds数组
        for(int i = 0; i < avaFds.size(); ++i){
            int sockfd = avaFds[i];
            printf("Each avaFds: %d %d\n", sockfd, avaFds.size());

            // 从可用fds中, 检测每一个有变化的 sockfd

            if (FD_ISSET(sockfd, &tmp_fds)) {
                printf("read data\n");


                /*
                char buf[BUFFER_SIZE];
                memset(buf, 0, sizeof(buf));
                int nread = recv(sockfd, buf, sizeof(buf), 0);

                printf("sockfd:%d -> recv data%d %s\n",sockfd, nread,buf);
                printf("errno:%d\n", errno);
*/

                u_int64_t total = 0UL;
                TransferBuffer tbuff;
                memset(&tbuff, 0, sizeof(TransferBuffer));

                int nread = recv(sockfd, &tbuff, sizeof(TransferBuffer), 0);
                if(nread <= 0){
                    // perror("read() failed");
                    // exit(EXIT_FAILURE);
                    printf("Connection closed from socket %d\n", sockfd);
                    close(sockfd);
                    FD_CLR(sockfd, &cliFds);  // 删除套接字
                    avaFds.erase(avaFds.begin() + i);
                    continue;
                }
                total = (u_int64_t)ceil((double)tbuff.size / sizeof(tbuff.data));
                printf("File Size:%llu , total:%llu\n",tbuff.size, total);
                send(sockfd, &total, sizeof(total), 0);

                FILE *fp2 = fopen("test/hash-a.exe", "w");

                printf("open:%d\n",fp2);
                for(u_int64_t i = 0; i <= total; ++i){
                    memset(&tbuff, 0, sizeof(TransferBuffer));
                    int ss = recv(sockfd, &tbuff, sizeof(TransferBuffer), 0);

                    int ss2 = fwrite(tbuff.data, 1, tbuff.dataSize,fp2);

                    //printf("i: %ld %ld\n", i, total);
                    send(sockfd, &i, sizeof(i), 0);
                }
                printf("接收完成\n");
                fclose(fp2);
            }
        }
    }

    return 0;
}

