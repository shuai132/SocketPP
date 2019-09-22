/**
 * 方便打印日志
 *
 * 宏    含义    作用                       颜色
 * LOG:  LOG    等效于printf并换行         默认色
 * LOGT: TAG    第一个参数为tag            默认色
 * LOGI: INFO   带有文件名信息             默认色
 * LOGE: ERROR  错误信息 会有ERROR:前缀     红色
 * LOGD: DEBUG  debug模式（NDEBUG未定义时） 黄色
 * 为了保证输出顺序 错误也使用stdout而不是stderr
 */

#pragma once

#ifdef __cplusplus
#include <cstdio>
#include <cstring>
#else
#include <stdio.h>
#include <string.h>
#endif

// Truncates the full __FILE__ path, only displaying the basename
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(fmt, ...)           do{ printf(fmt "\n", ##__VA_ARGS__); } while(0)
#define LOGT(tag, fmt, ...)     do{ printf(tag ": " fmt "\n", ##__VA_ARGS__); } while(0)
#define LOGI(fmt, ...)          do{ printf("%s: " fmt "\n", __FILENAME__, ##__VA_ARGS__); } while(0)
#define LOGE(fmt, ...)          do{ printf("\033[31mERROR: %s: %s: %d: " fmt "\033[m\n", \
                                    __FILENAME__, __func__, __LINE__, ##__VA_ARGS__); \
                                } while(0)

#ifdef NDEBUG
#define LOGD(fmt, ...)          ((void)0)
#else
#define LOGD(fmt, ...)          do{ printf("\033[33m%s: " fmt "\033[m\n", __FILENAME__, ##__VA_ARGS__); } while(0)
#endif
