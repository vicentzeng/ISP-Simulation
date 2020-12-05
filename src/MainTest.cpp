/**
 * 
 * Author: vicent
 * Create date: 2020/12/02
 * 
 **/

#include <iostream>
#include "ISPPipelineIntf.h"
#include "./core/ISPLog.h"

#define OUTPUT_PATH "../test_raw/output/"

#define LOG_LEVEL LOG_LEVEL_I

using namespace std;
uint8_t * readFile2Buf(const char * filename, uint32_t &count) {
    size_t readSize = 0;
    uint8_t *buffer = nullptr;
    FILE *fp = fopen(filename, "rb");
    if (nullptr == fp) {
        LOGE("%s file:%s open fail\n", __func__, filename);
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);     // 将文件指针移动文件结尾
    readSize = ftell(fp);       // 求出当前文件指针距离文件开始的字节数
    fseek(fp, 0, SEEK_SET);
    LOGD("%s readSize:%d\n", __func__, readSize);
    if(readSize == 0)
    {
        LOGE("%s size is zero!\n", __func__);
        goto FAIL;
    }
    buffer = (uint8_t *)malloc(readSize);
    if(buffer == NULL){
        LOGE("%s malloc failed\n", __func__);
    } else {
        count = fread((void *)buffer, 1, readSize, fp);
        LOGD("%s read:%d \n", __func__, count);
    }
FAIL:
    fclose(fp);
    fp = nullptr;
    return buffer;
}

uint32_t writeFile(const uint8_t * buf, const char * filename, uint32_t &count) {
    // 2.write to /data/vendor/camera/xx.json
    FILE *fpWrite = fopen(filename, "w");
    if (nullptr == fpWrite) {
        LOGE("file:%s open fail\n", filename);
        return -EIO;
    }
    int writeCount = fwrite(buf, 1, count, fpWrite);
    LOGD("writeCount:%d \n", writeCount);
    fclose(fpWrite);
    fpWrite = nullptr;
    if (writeCount == count) {
        LOGD("write suc\n");
    }
    return 0;
}

int main() {
    // Read input raw
    char filePath[256] = {0};
    uint32_t read_out_cnt = 0;
    uint32_t write_in_cnt = 0;
    uint8_t* rawBuf = readFile2Buf( "../test_raw/raw_4208x3120_8bits.raw", read_out_cnt);
    LOGI("%s read:%ld\n", __func__, read_out_cnt);

    sprintf(filePath, "%s/%s", OUTPUT_PATH, "isp_s0_raw_4208x3120_8bits.raw");
    writeFile(rawBuf, "../test_raw/output/raw_4208x3120_8bits_unpack_out.raw", read_out_cnt);
    ISPPipelineIntf ispIntf;
    LOGI("ISP pipeline process start");

    // Turn to 16bits
    LOGI("Turn to 16bits E");
    uint16_t* new16bitsRaw = (uint16_t*)malloc(read_out_cnt * 2);
    for (uint32_t i = 0; i < read_out_cnt; i++) {
        *(new16bitsRaw + i) = *(rawBuf + i);
    }
    write_in_cnt = read_out_cnt * 2;
    writeFile((uint8_t*)new16bitsRaw, "../test_raw/output/raw_4208x3120_Turn16bits_out.raw", write_in_cnt);

    // Demosaic
    LOGI("Demosaic E");
    ImgFrame rawFrame;
    rawFrame.imgBuf = rawBuf;
    rawFrame.width = 4208;
    rawFrame.height = 3120;
    rawFrame.bits = 16;
    rawFrame.type = IMAGE_RAW;
    rawFrame.isPacked = false;
    ImgFrame demosaicOut = rawFrame;
    demosaicOut.type = IMAGE_RGB;
    demosaicOut.imgBuf = new uint8_t[rawFrame.width * rawFrame.height * 3];
    ispIntf.demosaic(&rawFrame, &demosaicOut);
    
    // LOGD("Demosaic E");
    LOGI("ISP pipeline process out.\n");

    // Release resource
    free(rawBuf);
    rawBuf = nullptr;
    free(new16bitsRaw);
    new16bitsRaw = nullptr;
    delete [] demosaicOut.imgBuf;
    demosaicOut.imgBuf = nullptr;
    return 0;
}
