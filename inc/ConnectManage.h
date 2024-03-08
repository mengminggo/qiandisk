#ifndef QIANDISK_INC_CONNECTMANAGE_H
#define QIANDISK_INC_CONNECTMANAGE_H


#include "inc/FileIO.h"

#include "qd_datas.h"
#include "qd_types.h"
#include "qd_utils.h"


#include <vector>
#include <map>

#include <stdio.h>
#include <string.h>
#include <math.h>

class ServiceManager;
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


class ServerFileManager{

public:

    std::map<QINT_32, FileIO *> mFiles;

    std::map<QINT_32, SocketInfo *> mClientList;        //

    std::map<SocketInfo *, FileIO *> mTasks;        // 客户端 -> 任务列表

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
    void Write(FileIO *files, TransferBuffer *buff);

    // 根据 workid 获取 块数量
    QUINT_64 GetTotal(int workId);

    int Check(TransferBuffer *buff);

    void Close(QINT_32 workId);
    ~ServerFileManager();
};


#endif // #ifndef QIANDISK_INC_FILEINFOMANAGER_H

