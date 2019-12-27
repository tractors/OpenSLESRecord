//
// Created by 惠建华 on 2019/10/2.
//

#ifndef FFMUSIC_ANDROIDLOG_H
#define FFMUSIC_ANDROIDLOG_H

#endif //FFMUSIC_ANDROIDLOG_H

#include "android/log.h"

#define LOG_DEBUG true

#define LOGD(FORMAT,...)__android_log_print(ANDROID_LOG_DEBUG,"JniThread",FORMAT, ##__VA_ARGS__);
#define LOGE(FORMAT,...)__android_log_print(ANDROID_LOG_ERROR,"JniThread",FORMAT, ##__VA_ARGS__);