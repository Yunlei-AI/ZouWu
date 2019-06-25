#include "Zouwu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("%s <sensitivity> <test wav file>\n", argv[0]);
        exit(-1);
    }

    //char *tempPrefix = argv[1];
    float sensitivity = (float)atof(argv[1]);
    char *testFn = argv[2];
    
    void *hZouwu;
    int32_t ret;
    int32_t id = 1;
    // init zouwu
    ret = ZouwuInit(&hZouwu);
    if (ret != 0)
    {
        printf("Init failed. ret = %d\n", ret);
        exit(ret);
    }


    ret = ZouwuLoadModel(hZouwu, "./resources/model/mdl");
    if (ret != 0)
    {
        printf("LoadModel failed. ret = %d\n", ret);
        exit(ret);
    }

    ret = ZouwuSetParam(hZouwu, "sensitivity", &sensitivity, id);
    if (ret != 0)
    {
        printf("SetParam failed. ret = %d\n", ret);
        exit(ret);
    }

    // ÷÷°∂¡»°≤‚ ‘”Ô“Ù°¢ªΩ–—≤‚ ‘
    FILE *fp = fopen(testFn, "rb");
    fseek(fp, 44, SEEK_SET);
    int16_t test[ZOUWU_PROC_LEN];
    int32_t testFrame = 0;
    int32_t wake = 0;
    while (!feof(fp))
    {
        if (fread(test, sizeof(int16_t), ZOUWU_PROC_LEN, fp) != ZOUWU_PROC_LEN)
        {
            break;
        }

        ret = ZouwuProc(hZouwu, test, &wake);
        if (ret != 0)
        {
            printf("Proc failed. ret = %d\n", ret);
            exit(ret);
        }

        if (wake != 0)
        {
            printf("time = %f, wake = %d\n", (float)testFrame * ZOUWU_PROC_LEN / ZOUWU_FS, wake);
        }

        testFrame++;
    }
    fclose(fp);

    ret = ZouwuFinal(&hZouwu);
    if (ret != 0)
    {
        printf("Final failed. ret = %d\n", ret);
        exit(ret);
    }

    return 0;
}
