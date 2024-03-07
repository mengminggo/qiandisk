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

class ClientFile;


// 获取保存本地文件的信息
class FileInfoManager{
private:

    FileInfo        *mInfo;
    ClientFile      *mFile;

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

class ClientFile{
    // 此类 用于读写文件, 与网络无关

private:
    FILE        *mFp;       // 文件指针

    int         mWorkId;        // 每个任务仅处理一个文件

public:

    char        mWorkPath[255];

    int         mBlockSize;     // 块大小
    int         mBlockTotal;    // 总块数量

    int         tag;        // 0 写入完成 1 正在写入 -1 读取错误, 或者写入错误
public:
    ClientFile() = default;

    ClientFile(FileInfo *fileInfo);

    //  -1 打开失败, 0 打开成功
    int Open();        // 打开这个文件

    //  buffLen 需要写入的长度, 如果 blockSize > buffLen 会写入多余的 '\0' 字符, 所以需要控制 buffLen

    //  写入文件 index: 需要写入的块            buffLen
    void Write(int index, int buffLen, char *buff);

    //  读取文件 index 需要读取的块, 将数据保存到 buff中
    int Read(int index, char *buff);

    /**
        块索引从 0 开始,
        fseek(fp, index * mBlockSize, SEEK_SET);
    */

    void Close();
    ~ClientFile();

};


#endif // QIANDISK_CLIENT_FILEINFOMANAGER_H
