#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>

#include <signal.h>

#include <math.h>
#include <limits.h>

#include "inc/qd_utils.h"

#include "inc/qd_datas.h"
#include "inc/qd_types.h"

#include "FileInfoManager.h"
#include "ConnectManage.h"


#define WORK_PATH "../test/"

#define USERNAME "remote/"         // 暂定, 后续用用户名登录, 或者配置文件的方式获取 // 注意客户端中不能重复的问题

class ServerHandle;
class FileInfoManager;


void handleSignal(int signal) {
    if (signal == SIGINT) {
        printf("Program Exit\n");
        exit(1);
    }
}

class ServerHandle{
private:

    SocketInfo mainSvr;       // 保存服务端网络信息
    QINT_32 svrFd;

public:
    ServerHandle() = default;
    ServerHandle(const char *address, const int port){

        memset(&mainSvr, 0, sizeof(SocketInfo));
        svrFd = ConnectServer(&mainSvr,address, port);

    }

    int ConnectServer(SocketInfo *svr,const char *address, const int port){

        svr->addr.sin_family = AF_INET;
        svr->addr.sin_port = htons(port);                // 服务器端口
        svr->addr.sin_addr.s_addr = inet_addr(address);  // 服务器ip

        svr->socklen = sizeof(struct sockaddr_in);

        svr->fd = ConnectManage::CreateSocket();

        ConnectManage::Connect(svr->fd, &(svr->addr));

        return svr->fd;
    }


    // 发送文件信息


    int UpLoad(FileInfoManager *svrFile){

        FileInfo *fileInfo = svrFile->getFileInfo();

        // 发送本地文件信息
        svrFile->SendFileInfo(svrFd, USERNAME);

        TransferBufferSet buffSet;
        TransferBuffer buff;

        // 获取服务端发送的buffSet 根据 currentBlock 读取并发送相应的数据
        // 根据 end, 退出循环, 结束程序

        while(1){
            memset(&buffSet, 0, sizeof(TransferBufferSet));
            memset(&buff, 0, sizeof(TransferBuffer));

            // 注意服务端的 fd 别搞错
            int end = svrFile->Read(&mainSvr, &buffSet, &buff);

            if(end == -1){
                QD_LOGD("File Sending Completed: %d", end);
                break;
            }

            svrFile->Send(&mainSvr, &buff);
/*
            QD_LOGD("[i:%u/%u] [s:%d] [t:%d]",
                buffSet.currentBlock,buff.currentBlock,
                buff.dataSize,
                buffSet.tag); */
        }

        return 0;
    }


    ~ServerHandle(){
        close(svrFd);
    }
};

int main(int argc, char *argv[]) {


    char fullPath[256];

    memset(fullPath, 0, sizeof(fullPath));

    // 需要传输的文件名在这里
    if(argc > 1){
        strcpy(fullPath, argv[1]);
        printf("%d fullpath %s\n", argc, argv[1]);
    }

    FileInfoManager fileInfo(argv[1]);

    ServerHandle handle("127.0.0.1", 8848);

    handle.UpLoad(&fileInfo);

    return 0;
}



