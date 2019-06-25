#ifndef ZOUWU_H
#define ZOUWU_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZOUWU_PROC_LEN 512
#define ZOUWU_FS 16000

// 初始化
// initialization
int32_t ZouwuInit(void **hInst);


// 读模型
// load model
int32_t ZouwuLoadModel(void *pinst, const char *fname);

// 设置参数
// set model sensitivity (0~1) 
int32_t ZouwuSetParam(void *pinst, const char *name, void *val, int32_t id);
 
// 处理一帧数据，wake=id代表检测到唤醒词，wake=0代表未唤醒
// processor
int32_t ZouwuProc(void *pinst, const int16_t *wav, int32_t *wake);

// 拟初始化
// free
int32_t ZouwuFinal(void **hInst);

#ifdef __cplusplus
}
#endif

#endif
