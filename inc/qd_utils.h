

#undef LOG_TAG
#define LOG_TAG ""

#define QD_LOG(log_level,func, fmt, ...) \
    printf("%s %s [%s(): %d] " fmt "\n", log_level, LOG_TAG , func, __LINE__, ##__VA_ARGS__)

#define QD_LOGD(fmt, ...)  QD_LOG("D", __func__, fmt,  ##__VA_ARGS__)
#define QD_LOGE(fmt, ...)  QD_LOG("E", __func__, fmt,  ##__VA_ARGS__)

#define QD_LOG0 printf("\n");

#define FUNCTION_IN printf("D %s() +\n",__func__)
#define FUNCTION_OUT printf("D %s() -\n",__func__)

#define FUNCTION_LINE printf("LINE: %d\n", __LINE__)



// 拼接字符串
void splicString(char *dst, int len, int argc, ...);


// 生成 uuid, 通过返回值或参数将结果返回
char *generateUUID_4_1(char buf[37]);

unsigned int checksum(char *buf, int nwords);



// 变量置零
#define MMG_ZERO(object) \
    memset(&(object), 0, sizeof(object))
