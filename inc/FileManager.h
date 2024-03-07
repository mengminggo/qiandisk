#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>


#include <cstdio>
#include <cstring>


class BuffSrc;
class FileManager;
class BuffManager;

class WorkManager;



#define MAX_BUFF_SIZE 1024


// 用于传输的结构体
typedef struct TransBuff_t {
    int id;             // 每次传输任务的 id
    char buff[MAX_BUFF_SIZE];     // data
    int size;

    char data[MAX_BUFF_SIZE];     // data
    int dataSize;       // 数据大小

    int check;          // 校验码, 每次需要验证正确后, 才能够继续保存

} TransBuff;



class BuffSrc{
private:
    std::list<TransBuff*> buffs;
    int mLenght  = 99999;

    std::list<int> mIntNum;

    std::mutex mLock;

    std::mutex mLockSrc;

    std::condition_variable mCv_empty;
    std::condition_variable mCv_full;
    int maxSize = 1024;

public:
    BuffSrc() = default;
    ~BuffSrc() = default;
    void addData(TransBuff *buff);

    void getData(TransBuff **res);

    void setNum(int i);
    void getNum(int *res);

};



class FileManager{
private:
    char mFileName[128];
    char mParentPath[128];
    char mFullPath[512];

    // 一个文件传输任务, 对应一个文件句柄
    FILE *mFp;

    // 获取buff时, copy到该结构体, 保存文件时也从此处获取
    char mBuff[MAX_BUFF_SIZE];

    // 数据源
    BuffSrc *dataSrc;

    std::thread mWorkThread;

public:
    ~FileManager();
    FileManager() = default;
    FileManager(const char *fileName, const char *parentPath);
    FileManager(BuffSrc *src, const char *fileName, const char *parentPath);

    // 绑定数据源
    void setDataSrc(BuffSrc *src);

    // 保存文件 info
    int SaveInfo();

    // 将数据源的buff, 保存到 mBuff 中
    int GetBuff(char *buff, int size);

    // end_tag  1 正常传输, 0 末位
    int SaveFile(int size, int end_tag);

    void Loop();

    void Start();

    // 删除文件
    void remove();

    // 重命名文件
    void renameTo();

    // 移动文件
    void moveTo();
};

class BuffManager{
private:

    // 获取buff时, copy到该结构体, 保存文件时也从此处获取
    char mBuff[MAX_BUFF_SIZE];

    // 数据源
    BuffSrc *mDataSrc;

    const char *mFileName;

    std::thread mWorkThread;

public:
    ~BuffManager() = default;
    BuffManager() = default;
    BuffManager(BuffSrc *inSrc);

    BuffManager(BuffSrc *inSrc, const char *filename );


    // 绑定数据源, 从这里插入数据
    void setDataSrc(BuffSrc *inSrc);

    void addDataSrc(TransBuff *inBuff);

    void addBuff(int id, const char *data, int dataSize, int check);

    void GetLocalFile(const char *fullPath);

    void GetRemoteFile();

};




