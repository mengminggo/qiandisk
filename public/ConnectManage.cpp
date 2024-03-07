#include "inc/ConnectManage.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>  // 多路复用 I/O

#include <arpa/inet.h>

#include <unistd.h>

#include <stdio.h>

#include <chrono>
#include <thread>

int ConnectManage::CreateSocket(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if( fd < 0 ){
        QD_LOGE("socket faild: %d", fd);
    } else {
        QD_LOGD("create socket: %d", fd);
    }

    return fd;
}

int ConnectManage::BindAndListen(int fd, struct sockaddr_in *addr, int queue){
    int ret = bind(fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if(ret == -1) {
        QD_LOGE("bind error");
        return ret;
    }

    ///listen，成功返回0，出错返回-1
    ret = listen(fd, queue);
    if(ret == -1) {
        QD_LOGE("listen error");
        return ret;
    }

    QD_LOGD("bind and listen :%d", ntohs(addr->sin_port));
    return 0;
}

int ConnectManage::Accept(int svr, struct sockaddr_in *cli_addr, socklen_t *len){
    int fd = accept(svr, (struct sockaddr*)cli_addr, len);
    if( fd  < 0 ) {
        perror("client connect error !");
    } else {
        QD_LOGD("client:%d connect port: %d", fd, ntohs(cli_addr->sin_port));
    }
    return fd;
}

int ConnectManage::Connect(int svr, struct sockaddr_in *svr_addr){
    int resConn = connect(svr, (struct sockaddr *)svr_addr, sizeof(struct sockaddr_in));

    if (resConn < 0) {
        perror("connect faild:%d");
        QD_LOGE("connect faild:%d", resConn);
    } else {
        QD_LOGD("connect success %d ",ntohs(svr_addr->sin_port));
    }

    return resConn;

}

int ConnectManage::Close(int fd){
    close(fd);
    return 0;
}



//#############################################################################################
ServiceManager::ServiceManager(const char *workPath){

    mSvrFileManage = new ServerFileManager;

    memset(&mainSvr, 0, sizeof(SocketInfo));
    memset(mWorkPath,0, sizeof(mWorkPath));

    strcpy(mWorkPath, workPath);

    mSvr1 = CreateServer(&mainSvr);
    BindAndListen(&mainSvr, 8848);


}
int ServiceManager::CreateServer(SocketInfo *socketInfo){

    socketInfo->fd = ConnectManage::CreateSocket();

    return socketInfo->fd;
}

int ServiceManager::BindAndListen(SocketInfo *socketInfo, const int port){

    socketInfo->addr.sin_family = AF_INET;
    socketInfo->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketInfo->addr.sin_port = htons(port);

    socketInfo->type = 0;
    ConnectManage::BindAndListen(socketInfo->fd, &socketInfo->addr,20);

    return 0;
}


void ServiceManager::AddClient(SocketInfo *cli){
    FUNCTION_IN;
    cli->port = ntohs(cli->addr.sin_port);

    QD_LOGD("Client fd:port = (%d, %d)", cli->fd, cli->port);

    mClientList[cli->fd] = cli;

}


// 此方法是一直运行的, 用于接收新客户端的连接
void ServiceManager::GetFileInfo(){

    FileInfo fileInfo;

    while(1){
        SocketInfo *cli = new SocketInfo;
        memset(&fileInfo, 0, sizeof(FileInfo));

        memset(cli, 0, sizeof(SocketInfo));

        cli->socklen = sizeof(struct sockaddr_in);  // 仅在连接时需要

        // 阻塞 等待客户端连接, 只用于首次连接
        cli->fd = ConnectManage::Accept(mSvr1, &cli->addr, &cli->socklen);

        //QD_LOGD("%d", cli.fd);

        // 接收文件信息
        // 根据文件大小, 确认所需要的buff大小
        recv(cli->fd, &fileInfo, sizeof(fileInfo), 0);

        // 根据用户信息, 选择文件保存到的文件夹
        //splicString(fileInfo.full, 255, 2, fileInfo.path, fileInfo.name);
        splicString(fileInfo.workPath, 255, 3, mWorkPath, fileInfo.user, fileInfo.name);

        //printf("%s: %d bytes\n",fileInfo.name, fileInfo.size);

        fileInfo.blockSize = 60000;
        fileInfo.blockTotal = ceil((double)fileInfo.size / fileInfo.blockSize);

        // 此次传输的任务编号, 全局可见
        fileInfo.workId = 1213;


        QD_LOGD("Work: %d, [%s]",fileInfo.workId, fileInfo.workPath);

        QD_LOGD("File: [%ld, %ld, %ld]", fileInfo.size, fileInfo.blockSize,fileInfo.blockTotal);

        int nsend = send(cli->fd, &fileInfo, sizeof(FileInfo), 0);


        // 或客户端信息完成, 添加到任务列表
        //this->AddClient(cli);

        mSvrFileManage->AddWork(cli, &fileInfo);

    }
}

// 根据客户端的 socket, 来下发指令, 获取对应文件的 buff
void ServiceManager::GetFileBuff(){

    mSvrFileManage->Run();

}

ServiceManager::~ServiceManager(){
    close(mainSvr.fd);
    if(mSvrFileManage){
        delete mSvrFileManage;
        mSvrFileManage = NULL;
    }
}


//#############################################################################################

ServerFile::ServerFile(FileInfo *fileInfo){

    mWorkId = fileInfo->workId;

    mBlockSize = fileInfo->blockSize;
    mBlockTotal = fileInfo->blockTotal;

    strcpy(mFilePath, fileInfo->workPath);

    QD_LOGD("id:%d,blockSize:%d,blockTotal:%d,file:%s", mWorkId, mBlockSize, mBlockTotal, mFilePath);

    this->Open();

}

int ServerFile::Open(){
    mFp = fopen(mFilePath, "w");

    if(mFp == NULL) {
        QD_LOGD("open %s faild!", mFilePath);
        return -1;
    }
    QD_LOGD("open %s success!", mFilePath);

    return 0;
}
void ServerFile::Write(QINT_32 index, QINT_32 buffLen, QCHAR *buff){
    //QD_LOGD("%d %d %d", index, mBlockSize, buffLen);
    fseek(mFp, index * mBlockSize, SEEK_SET);
    int writeSize = fwrite(buff, 1, buffLen, mFp);
    //QD_LOGD("fwrite:%d", writeSize);
    int nf = fflush(mFp);
    //QD_LOGD("fflush file %d", nf);
}

void ServerFile::Read(QINT_32 index, QCHAR *buff){
    fseek(mFp,index * mBlockSize, SEEK_SET);
    int rNum = fread(buff, 1, mBlockSize, mFp);
    //printf("%d\n", rNum);
}

void ServerFile::Close(){
    int res = fclose(mFp);
    QD_LOGD("res:%d", res);
}
ServerFile::~ServerFile(){

}


//#############################################################################################

ServerFileManager::ServerFileManager(){
    FUNCTION_IN;
}


void ServerFileManager::AddWork(SocketInfo *cli, FileInfo *fileInfo){

    FUNCTION_IN;
    mClientList[cli->workId] = cli;
    mFiles[cli->workId] = new ServerFile(fileInfo);

    mWorkIds.push_back(cli->workId);

}


void ServerFileManager::Run(){
    // 遍历 cli info， 如果有的话，创建 ServerFile 进行传输任务
    while(1){
        QD_LOGD("Work Num: %d", mWorkIds.size());
        for(auto it = mWorkIds.begin(); it != mWorkIds.end();){
            QD_LOGD("Start Proces: %d", *it);
            SocketInfo  *cli = mClientList.at(*it);
            ServerFile  *svrFile = mFiles.at(*it);
            // 遍历 mBlockTotal

            TransferBufferSet   buffSet;
            TransferBuffer      buff;

            QUINT_64 current = 0;
            QUINT_64 total = svrFile->mBlockTotal;

            for(current = 0; current < total; ){

                memset(&buffSet, 0, sizeof(buffSet));
                memset(&buff, 0 ,sizeof(buff));


                buffSet.workId  = 1213;
                buffSet.blockSize = 60000;
                buffSet.end = 1;
                buffSet.currentBlock = current;
                buffSet.totalBlock = total;

                // 读取网络buff
                this->Read(cli, &buffSet, &buff);
                //QD_LOGD("Get [i:%u/%u] [s:%d]",buffSet.currentBlock,buff.currentBlock, buff.dataSize);

                // 如果 Check 失败， 重新处理当前 block

                if(this->Check(&buff) == -1){
                    continue;
                }

                // 写入本地
                this->Write(svrFile, &buff);

                ++current;
            }

            // 通过 tag 发送完成信息， 在客户端中关闭连接
            memset(&buffSet, 0, sizeof(buffSet));

            buffSet.end = -1;
            send(cli->fd, &buffSet, sizeof(buffSet), 0);


            // 传输完成, 删除任务
            it = mWorkIds.erase(it);

            mClientList.erase(*it);
            mFiles.erase(*it);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

// 根据 cli 获取buff
void ServerFileManager::Read(SocketInfo *cli, TransferBufferSet *buffSet, TransferBuffer *buff){

    int nsend = send(cli->fd, buffSet, sizeof(TransferBufferSet), 0);
    int nrecv = recv(cli->fd, buff, sizeof(TransferBuffer), 0);
}

void ServerFileManager::Write(ServerFile *file, TransferBuffer *buff){
    file->Write(buff->currentBlock, buff->dataSize, buff->data);
}

// 根据 workid 获取 块数量
QUINT_64 ServerFileManager::GetTotal(int workId){
    printf("%d\n", mFiles.size());

   // QD_LOGD("%d", mFiles[workId]->mBlockTotal);
    int total = mFiles[workId]->mBlockTotal;
    return total;
}

int ServerFileManager::Check(TransferBuffer *buff){
    if(buff->checkCode != checksum(buff->data, buff->dataSize)){
        QD_LOGD("retry %d", buff->currentBlock);
        //buffSet.tag = 1;
        return -1;
    }
    return 0;
}

void ServerFileManager::Close(QINT_32 workId){
    mFiles[workId]->Close();
}
ServerFileManager::~ServerFileManager(){

}


