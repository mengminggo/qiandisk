#ifndef QIANDISK_INC_FILEIO_H
#define QIANDISK_INC_FILEIO_H


#include <stdio.h>

#include <string.h>


class FileIO{
    // 设置块大小, 分块读写磁盘文件

private:

    FILE        *mFp;           // 文件指针
    char        mFilePath[255]; // 文件绝对路径

    int         mBlockSize;     // 块大小
    int         mBlockTotal;    // 块的数量
public:
    FileIO() = default;

    FileIO(const char *__filename, int blockSize, int blockTotal);


    // -1 打开失败, 0 打开成功
    int Open(const char * __mode);   // mode: 打开模式   r  w


    //  buffLen 需要写入的长度, 如果 blockSize > buffLen 会写入多余的 '\0' 字符, 所以需要控制 buffLen

    //  写入文件 index: 需要写入的块            buffLen
    int Write(int index, int buffLen, char *buff);

    //  读取文件 index 需要读取的块, 将数据保存到 buff中
    int Read(int index, char *buff);

    /**

        块索引从 0 开始,
        fseek(fp, index * mBlockSize, SEEK_SET);

    */

    void Close();
    ~FileIO();

};



#endif // QIANDISK_INC_FILEIO_H

