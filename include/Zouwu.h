#ifndef ZOUWU_H
#define ZOUWU_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZOUWU_PROC_LEN 512
#define ZOUWU_FS 16000

// ��ʼ��
// initialization
int32_t ZouwuInit(void **hInst);


// ��ģ��
// load model
int32_t ZouwuLoadModel(void *pinst, const char *fname);

// ���ò���
// set model sensitivity (0~1) 
int32_t ZouwuSetParam(void *pinst, const char *name, void *val, int32_t id);
 
// ����һ֡���ݣ�wake=id�����⵽���Ѵʣ�wake=0����δ����
// processor
int32_t ZouwuProc(void *pinst, const int16_t *wav, int32_t *wake);

// ���ʼ��
// free
int32_t ZouwuFinal(void **hInst);

#ifdef __cplusplus
}
#endif

#endif
