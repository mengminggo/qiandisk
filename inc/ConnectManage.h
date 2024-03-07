#ifndef QIANDISK_INC_CONNECTMANAGE_H
#define QIANDISK_INC_CONNECTMANAGE_H

#include "qd_datas.h"
#include "qd_types.h"
#include "qd_utils.h"


#include <vector>
#include <map>

#include <stdio.h>
#include <string.h>
#include <math.h>

class ServiceManager;
class ServerFile;
class ServerFileManager;

class ConnectManage{
public:
    // 创建 socket
    static int CreateSocket();

    // 服务端用于绑定和监听端口
    static int BindAndListen(int fd, struct sockaddr_in *addr, int queue = 20);

    // 服务端阻塞获取客户端连接
    // cli fd         svr fd                   cli addr         cli len
    static int Accept(int fd, struct sockaddr_in *cli_addr, socklen_t *len);

    // 客户端连接服务端
    //      cli fd       svr fd     svr addr
    static int Connect(int fd, struct sockaddr_in *svr_addr);

    static int Close(int fd);


public:
    ConnectManage() = default;


    ~ConnectManage() = default;

};

class ServiceManager{
public:
    SocketInfo mainSvr;
    int mSvr1;   // 简化 fd

    // fd:socketinfo
    std::map<QINT_32, SocketInfo *> mClientList;

    ServerFileManager *mSvrFileManage;

    QCHAR   mWorkPath[256];

public:
    ServiceManager(const char *workPath);
    int CreateServer(SocketInfo *socketInfo);

    int BindAndListen(SocketInfo *socketInfo, const int port);

    // 此方法是一直运行的, 用于接收新客户端的连接
    // Accept()
    void GetFileInfo();

    // 保存新连接的客户端
    void AddClient(SocketInfo *cli);

    // 根据客户端的 socket, 来下发指令, 获取对应文件的 buff
    void GetFileBuff();

    ~ServiceManager();
};


// cli / svr 均可使用
class ServerFile{
    // 此类 用于读写文件, 与网络无关

private:
    FILE        *mFp;       // 文件指针

    int         mWorkId;        // 每个任务仅处理一个文件

public:

    char        mFilePath[255];

    int         mBlockSize;     // 块大小
    int         mBlockTotal;    // 总块数量

    int         tag;        // 0 写入完成 1 正在写入 -1 读取错误, 或者写入错误
public:
    ServerFile() = default;

    ServerFile(FileInfo *fileInfo);


    // -1 打开失败, 0 打开成功
    int Open();        // 打开这个文件


    //  buffLen 需要写入的长度, 如果 blockSize > buffLen 会写入多余的 '\0' 字符, 所以需要控制 buffLen

    //  写入文件 index: 需要写入的块            buffLen
    void Write(int index, int buffLen, char *buff);

    //  读取文件 index 需要读取的块, 将数据保存到 buff中
    void Read(int index, char *buff);

    /**

        块索引从 0 开始,
        fseek(fp, index * mBlockSize, SEEK_SET);

    */

    void Close();
    ~ServerFile();

};

class ServerFileManager{

public:

    //
    std::map<QINT_32, ServerFile *> mFiles;             // 一次传输任务和 mClient 是对应的

    std::map<QINT_32, SocketInfo *> mClientList;        //

    std::map<SocketInfo *, ServerFile *> mTasks;        // 客户端 -> 任务列表

    std::vector<QINT_32>            mWorkIds;

public:
    ServerFileManager();
    // 新建任务, 客户端-文件信息
    void AddWork(SocketInfo *cli, FileInfo *fileInfo);

    // 发送任务, 接收buff
    void Run();

    // 给 cli 发送 buffSet, 并获取 cli 传过来的 buff
    void Read(SocketInfo *cli, TransferBufferSet *buffSet, TransferBuffer *buff);

    // 本地buff写入
    void Write(ServerFile *files, TransferBuffer *buff);

    // 根据 workid 获取 块数量
    QUINT_64 GetTotal(int workId);

    int Check(TransferBuffer *buff);

    void Close(QINT_32 workId);
    ~ServerFileManager();
};


#endif // #ifndef QIANDISK_INC_FILEINFOMANAGER_H

