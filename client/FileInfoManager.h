#ifndef QIANDISK_CLIENT_FILEINFOMANAGER_H
#define QIANDISK_CLIENT_FILEINFOMANAGER_H

#include <sys/stat.h>  // posix: file info
#include <libgen.h> // dirname basename

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fstream>

#include "inc/qd_types.h"
#include "inc/qd_datas.h"
#include "inc/qd_utils.h"


#include "inc/FileIO.h"


class ClientFile;


// 获取保存本地文件的信息
class FileInfoManager{
private:

    FileInfo        *mInfo;

    FileIO          *mFile;

public:
    FileInfoManager(const char *filePath);

    QUINT_64 getSize();

    // 获取文件信息
    FileInfo* getFileInfo();

    int SendFileInfo(int svrFd, const char* __username);

    // 从本地读取文件, 发送buff
    int Read(SocketInfo *svr, TransferBufferSet *buffSet, TransferBuffer *buff);

    int Send(SocketInfo *svr, TransferBuffer *buff);

    // 从网络获取data, 写入到本地文件
    void Write(TransferBuffer *buff);


    ~FileInfoManager();

};

#endif // QIANDISK_CLIENT_FILEINFOMANAGER_H
