#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>  // 多路复用 I/O

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#include <sys/stat.h>  // posix: file info

#include <time.h>

#include <thread>
#include <math.h>

#include <map>
#include <vector>

int main() {

    std::vector<int> li = {1};

    for(auto it = li.begin(); it != li.end(); ){
        it = li.erase(it);
    }
    printf("%d\n", li.size());

    return 0;
}


