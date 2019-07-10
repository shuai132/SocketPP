//
// Created by liushuai on 2019/7/10.
//
// 方便打印日志 为了输出顺序一致 错误也使用stdout而不用stderr 使用ERROR前缀
//

#include <stdio.h>
#include <string.h>

// Truncates the full __FILE__ path, only displaying the basename
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOGF(fmt, ...)          do{ printf(fmt "\n", ##__VA_ARGS__); } while(0)
#define LOGT(tag, fmt, ...)     do{ printf(tag ": " fmt "\n", ##__VA_ARGS__); } while(0)
#define LOGI(fmt, ...)          do{ fprintf(stdout, "%s: " fmt "\n", __FILENAME__, ##__VA_ARGS__); } while(0)
#define LOGE(fmt, ...)          do{ fprintf(stdout, "ERROR: %s: %s: %d: " fmt "\n", __FILENAME__, __func__, __LINE__, ##__VA_ARGS__); } while(0)

#ifdef NDEBUG
    #define LOGD(fmt, ...)  ((void)0)
#else
    #define LOGD(fmt, ...)  LOGI(fmt, ##__VA_ARGS__)
#endif
