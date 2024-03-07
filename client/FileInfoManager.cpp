#include "FileInfoManager.h"


FileInfoManager::FileInfoManager(const char *filePath){
    mInfo = new FileInfo;

    QCHAR  *mBasec = strdup(filePath);

    struct stat     fileStat;

    if (stat(filePath, &fileStat) == 0) {

        mInfo->size = fileStat.st_size;          // 文件大小

        realpath(mBasec, mInfo->full);            // 绝对路径

        strcpy(mInfo->name, basename(mBasec));    // 文件名

        strcpy(mInfo->path, dirname(mBasec));     // 上级路径 - 父路径

    } else {
        mInfo->size = 0;
    }

    free(mBasec);
    // 创建文件操作
}

FileInfo* FileInfoManager::getFileInfo(){
    return mInfo;
}

int FileInfoManager::SendFileInfo(int svrFd, const char* __username){

    strcpy(mInfo->user, __username);
    // 把文件基础信息传给 server

    send(svrFd, mInfo, sizeof(FileInfo), 0);

    // usleep(1000 * 1000);  // 单位微秒 μs
    // server端, 根据网络情况, 系统负载, 指定传输的 buff 大小
    int nrec1 = recv(svrFd, mInfo, sizeof(FileInfo), 0);

    QD_LOGD("Work: %d, [%s]", mInfo->workId, mInfo->full);

    QD_LOGD("File: (%ld, %ld, %ld)", mInfo->size, mInfo->blockSize,mInfo->blockTotal);


    mFile = new FileIO(mInfo->full, mInfo->blockSize, mInfo->blockTotal);

    mFile->Open("r");

    return 0;
}


// 根据 cli 获取buff
int FileInfoManager::Read(SocketInfo *svr, TransferBufferSet *buffSet, TransferBuffer *buff){
    int nrecv = recv(svr->fd, buffSet, sizeof(TransferBufferSet), 0);
    if (buffSet->end == -1){
        return -1;
    }

    int readSize = mFile->Read(buffSet->currentBlock, buff->data);

    // 设置各种参数
    buff->currentBlock = buffSet->currentBlock;
    buff->totalBlock = buffSet->totalBlock;
    buff->id = buffSet->workId;


    buff->dataSize = readSize;

    buff->checkCode = checksum(buff->data, buff->dataSize);

    return 0;
}

int FileInfoManager::Send(SocketInfo *svr, TransferBuffer *buff){

    int nsend = send(svr->fd, buff, sizeof(TransferBuffer), 0);

}

void FileInfoManager::Write(TransferBuffer *buff){
    mFile->Write(buff->currentBlock, buff->dataSize, buff->data);
}

FileInfoManager::~FileInfoManager(){

    free(mFile);
    free(mInfo);

    mInfo = NULL;
    mFile = NULL;

}
