#ifndef QIANDISK_INC_QD_DATAS_H
#define QIANDISK_INC_QD_DATAS_H

#include "qd_types.h"


// 客户端向服务端发送的文件基本信息
typedef struct {
    QCHAR   path[127]; // 本地路径
    QCHAR   name[127]; // 本地文件名
    QCHAR   full[255]; // 本地全路径
    QINT_64 size;    // 本地文件大小

    QCHAR   user[16];  // 用户 username

    QCHAR       workPath[255];    // 远程路径

    QINT_32     workId;         // 传输任务的id

    QINT_32     blockSize;      // 分块大小, 每次传输的buff大小
    QUINT_64    blockTotal;     // 文件分块  索引从 0 开始

} FileInfo;

typedef struct{
    QINT_32     workId;         // 传输任务的id
    QINT_32     blockSize;      // 分块大小, 一般就是实际的data
    QUINT_64    currentBlock;   // 当前传输的块
    QUINT_64    totalBlock;     // 总块

    QINT_32     tag;       // 0 正常传输     1 重试传输     // -1 末尾
    QINT_32     end;       // 0 传输中       -1 末尾数据

}TransferBufferSet;

typedef struct{
    QINT_32     id;             // 传输任务的id
    QUINT_64    size;           // 传输数据量
    QCHAR       data[60000];   // data < 65535
    QINT_32     dataSize;       // 传输的实际数据大小

    QUINT_64    current;        // 切片传输
    QUINT_64    total;          // 总切片大小

    QINT_32     blockSize;      // 分块大小, 一般就是实际的data

    QUINT_64    currentBlock;  // 当前传输的块
    QUINT_64    totalBlock;    // 总块

    QUINT_32    checkCode;

    QINT_32     tag;       // 0 正常传输    1 重试传输   // 2 末尾
    QINT_32     end;       // 1 传输中   0 末尾数据

}TransferBuffer;


typedef struct {
    QINT_32     id;
    QINT_32     sockfd;
    QCHAR       ip[16];
    QINT_32     port;
} ClientInfo;

#include <netinet/in.h>
typedef struct {
    QINT_32             workId;
    QINT_32             id;      // workid

    QINT_32             fd;

    struct sockaddr_in  addr;
    socklen_t           socklen;

    QINT_32             port;

    QINT_32             type;     // 0  server, 1 client
} SocketInfo;

#if 0

typedef struct{
    QINT_32             workId;
    ClientInfo          cli;
    FileInfo            fileInfo;
    BlockInfo           blockInfo;
};

#endif


// 接收文件数据后, 返回验证信息, 是否需延迟, 接受到的数据大小
typedef struct {
    QUINT_32 check;
    QINT_32  recvDataSize;
    QINT_32  delay;
} ResFileData;


#endif // #ifndef QIANDISK_INC_QD_DATAS_H

