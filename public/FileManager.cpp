#include "inc/FileManager.h"


/**
###################################################################

这里暂时应该没有问题

*/


void BuffSrc::addData(TransBuff *buff){

   // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
        std::unique_lock<std::mutex> lock(mLock);
        int i = buffs.size();
        mCv_full.wait(lock, [&]{ return buffs.size() < maxSize;});

        buffs.push_back(buff);
      //  printf("BuffList size = %d --- %d\n", buffs.size(), i);

    }
    mCv_empty.notify_all();
}

void BuffSrc::getData(TransBuff **res){
   // std::this_thread::sleep_for(std::chrono::milliseconds(2));
    {
        std::unique_lock<std::mutex> lock(mLock);
        mCv_empty.wait(lock, [&]{return !buffs.empty();});

        *res = buffs.front();

        //printf("getData = %d\n", (*res)->id);
        buffs.pop_front();
    }
    mCv_full.notify_all();
}


void BuffSrc::setNum(int i){
    //printf("setNum()\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    {
        std::unique_lock<std::mutex> lock(mLock);

        mCv_full.wait(lock, [&]{ return mIntNum.size() < maxSize;});

        mIntNum.push_back(i);

        printf("List size = %d --- %d\n", mIntNum.size(), i);
    }

    mCv_empty.notify_all();
}

void BuffSrc::getNum(int *res){
    //printf("getNum()\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    {
        std::unique_lock<std::mutex> lock(mLock);

        mCv_empty.wait(lock, [&]{return !mIntNum.empty();});

        *res = mIntNum.front();
       // printf("del %d\n", mIntNum.front());
        mIntNum.pop_front();
    }
    mCv_full.notify_all();
}

/*************************************************************************
    文件管理, 从数据源中拿取需要的数据, 保存成文件
*/

FileManager::FileManager(const char *fileName, const char *parentPath){
    memset(mFileName,0, sizeof(mFileName));
    memset(mParentPath,0, sizeof(mParentPath));
    memset(mFullPath,0, sizeof(mFullPath));

    // strcpy(mFileName, fileName);
    // strcpy(mParentPath, parentPath);

    strcpy(mFullPath, fileName);
    strcat(mFullPath, parentPath);

    mFp = fopen(mFullPath, "w");
}
FileManager::FileManager(BuffSrc *src, const char *fileName, const char *parentPath){

    memset(mFullPath,0, sizeof(mFullPath));

    dataSrc = src;

    strcpy(mFullPath, fileName);
    strcat(mFullPath, parentPath);

    mFp = fopen(mFullPath, "w");
}


void FileManager::setDataSrc(BuffSrc *src){
    dataSrc = src;
}


int FileManager::SaveInfo(){
    return 0;
}

int FileManager::SaveFile(int size, int end_tag){
    if(end_tag == 0){
        return fclose(mFp);
    }
    fwrite(mBuff, sizeof(char), size, mFp);
    memset(mBuff, 0, sizeof(mBuff));
    return 0;
}

int FileManager::GetBuff(char *buff, int size){
    int buffSize = strlen(buff);
    memcpy(mBuff, buff, size);
    return buffSize;
}

void FileManager::Loop() {
    mFp = fopen(mFullPath, "w");

    BuffSrc *src = (BuffSrc *)dataSrc;

    TransBuff *data = NULL;

    // 在数据源中, 找到对应 id 的值

    while(true){

        src->getData(&data);

        // printf("Loop %d\n", buff->id);

        if(data->size == 0){
            printf("File Save Success: %s\n", mFullPath);
            SaveFile(0, 0); // 保存文件
            break;
        }

        GetBuff(data->buff, data->size);

        // printf("%s\n", mBuff);
        SaveFile(data->size, 1);
    }
}

void FileManager::Start(){
    mWorkThread = std::thread(&FileManager::Loop, this);
    printf("Start Save: %s\n", mFullPath);

}

FileManager::~FileManager(){
    mWorkThread.join();
}


/*********************************************************************
    获取远程数据, 插入到数据源中
*/

BuffManager::BuffManager(BuffSrc *inSrc)
    : mDataSrc(inSrc)
{

}


BuffManager::BuffManager(BuffSrc *inSrc, const char * filename)
    : mDataSrc(inSrc), mFileName(filename)
{

}

void BuffManager::setDataSrc(BuffSrc *inSrc){
    mDataSrc = inSrc;
    //printf("添加数据源\n");
}

void BuffManager::addDataSrc(TransBuff *inBuff){
    mDataSrc->addData(inBuff);
}

void BuffManager::addBuff(int id, const char *data, int dataSize, int check){

    TransBuff *buff = new TransBuff;

    buff->id = id;

    memcpy(buff->data, data, dataSize);

    buff->check = check;

    addDataSrc(buff);

}

void BuffManager::GetLocalFile(const char *fullPath){
    int i = 1;

    FILE *fp1 = fopen(fullPath, "r");
    char buff[88];

    memset(buff, 0, sizeof(buff));

    int size = 0;

    while((size = fread(buff, 1, sizeof(buff),fp1)) >= 0){
        TransBuff *bb = new TransBuff;

        bb->id = 9;
        memcpy(bb->buff, buff, size);
        bb->size = size;

        addDataSrc(bb);
    }

    fclose(fp1);

}

void BuffManager::GetRemoteFile(){
    // 使用长连接
    // 循环发送数据
}






