#include "qd_utils.h"
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#include <stdarg.h>   // va_list

char *generateUUID_4_1(char buf[37])
{
    const char *c = "89ab";   // UUID 大都是变体 1，取值是 8、9、a、b 中的一个。
    char *res = buf;
    int n;

    for( n = 0; n < 16; ++n )
    {
        int b = rand()%255;  // 生成16个16进制的值

        switch( n )
        {
        case 6:
            sprintf(
                res,
                "4%x",
                b%15 );   // 版本 4：UUID 使用随机性或伪随机性生成
            break;
        case 8:
            sprintf(
                res,
                "%c%x",
                c[rand()%strlen( c )],
                b%15 );     // UUID 变体
            break;
        default:
            sprintf(
                res,
                "%02x",
                b );
            break;
        }

        res += 2;
#if 0  // 添加间隔 -
        switch( n )
        {
        case 3:
        case 5:
        case 7:
        case 9:
            *res++ = '-';
            break;
        }
#endif

    }
    *res = 0;
    return buf;
}

// 简单校验和
unsigned int checksum(char *buf, int nwords) {
    unsigned long sum = 0;
    for (int i = 0; i<nwords; ++i) {
        sum += *(buf+i);
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned int)(~sum);
}



// 将多个字符串拼接到一个字符数组中
// dst 保存最后的结果, len 是长度,  argc 需要拼接的字符串个数
void splicString(char *dst, int len, int argc, ...){
    memset(dst, 0, len);
    va_list args;
    va_start(args, argc);
    char *next = NULL;
    for(int i =0; i< argc; ++i){
        next = va_arg(args, char*);
        strcat(dst, next);
    }
    va_end(args);
}

// 简化函数用法


