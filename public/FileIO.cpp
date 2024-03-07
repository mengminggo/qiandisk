#include "inc/FileIO.h"

#include "inc/qd_datas.h"
#include "inc/qd_types.h"
#include "inc/qd_utils.h"

FileIO::FileIO(const char *__filename, int blockSize, int blockTotal)
    : mBlockSize(blockSize), mBlockTotal(blockTotal)
{
    strcpy(mFilePath, __filename);
}

int FileIO::Open(const char * __mode){
    mFp = fopen(mFilePath, __mode);

    if(mFp == NULL) {
        QD_LOGD("open %s faild!", mFilePath);
        return -1;
    }
    QD_LOGD("open %s, block: (%d , %d)", mFilePath, mBlockSize, mBlockTotal);

    return 0;
}
int FileIO::Write(QINT_32 index, QINT_32 buffLen, QCHAR *buff){
    //QD_LOGD("%d %d %d", index, mBlockSize, buffLen);
    fseek(mFp, index * mBlockSize, SEEK_SET);
    int writeSize = fwrite(buff, 1, buffLen, mFp);
    //QD_LOGD("fwrite:%d", writeSize);
    int nf = fflush(mFp);
    //QD_LOGD("fflush file %d", nf);
}

int FileIO::Read(QINT_32 index, QCHAR *buff){
    fseek(mFp,index * mBlockSize, SEEK_SET);
    int rNum = fread(buff, 1, mBlockSize, mFp);
    //printf("%d\n", rNum);
    return rNum;
}

void FileIO::Close(){
    int res = fclose(mFp);
    QD_LOGD("res:%d", res);
}

FileIO::~FileIO(){
    fclose(mFp);
}

