// 传统阻塞 IO
// 读写分离
// 主线程负责获取客户端的连接, 文件信息管理
// work 线程接受文件
// file 文件的保存

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>  // 多路复用 I/O

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <math.h>

#include <vector>
#include <map>

#include <signal.h>

#include "inc/qd_utils.h"

#include "inc/qd_datas.h"
#include "inc/qd_types.h"

#include "inc/ConnectManage.h"

#include "inc/ThreadPool.h"

#define QUEUE   20

#define WORK_PATH "../test/remote/"

void handleSignal(int signal) {
    if (signal == SIGINT) {
        printf("Program Exit\n");
        exit(1);
    }
}

/*
    8848 main 端口
    8849 file 端口 --- 随机端口
*/

int main(int argc, char *argv[]) {
    signal(SIGINT, handleSignal);
    int sleepNum = 1000;
    if(argc > 1){
        sleepNum = atoi(argv[1]);
    }
    printf("Server Startup ........\n");

    ServiceManager app("../test/");

    auto v1 = &ServiceManager::GetFileInfo;
    auto v2 = &ServiceManager::GetFileBuff;

    std::thread t(v1, &app);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t2(v2, &app);


    t2.join();
    t.join();

    //CreateClient(&app.mainSvr,&app.fileSvr);

    return 0;
}


