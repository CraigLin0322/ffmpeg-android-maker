

#include <android/log.h>

#define LOGI(TAG, FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, TAG, FORMAT, ##__VA_ARGS__)
#define LOGE(TAG, FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, FORMAT, ##__VA_ARGS__)

#define STATUS_SUCCESS 0;
#define STATUS_FAILURE -1;