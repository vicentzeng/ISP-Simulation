/**
 * 
 * Author: vicent
 * Create date: 2020/12/02
 * 
 **/

#include <iostream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include "ISPPipelineIntf.h"
#include "./core/ISPLog.h"

#include "./AwbCore.h"
#include "./GammaCore.h"

#define OUTPUT_PATH "../../test_raw/output/"

#define LOG_LEVEL LOG_LEVEL_I

using namespace std;
using namespace cv;

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
    if(readSize == 0) {
        LOGE("%s size is zero!\n", __func__);
        goto FAIL;
    }
    buffer = (uint8_t *)malloc(readSize);
    if (buffer == NULL) {
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
    FILE *fpWrite = fopen(filename, "wb");
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

uint32_t writeFile(const uint8_t * buf, const char * path, const char * filename, uint32_t &count) {
    char fullName[256] = {0};
    sprintf(fullName, "%s/%s", path, filename);
    return writeFile(buf,fullName, count);
}

void writeMat2File(Mat mat, const char * path, const char * filename) {
    char fullName[256] = {0};
    sprintf(fullName, "%s/%s", path, filename);
    string s_name = fullName;
    imwrite(s_name, mat);
}

uint32_t writeRGB2Png(uint8_t * buf, const char * path,
    const char * filename, int width, int height) {
    Mat inMat(height, width, CV_8UC3, (void *)buf);
    Mat outMat;
    cvtColor(inMat, outMat, CV_RGB2BGR);
    writeMat2File(outMat, path, filename);
    return 0;
}

int runIspPipeline(uint8_t* rawBuf, int width, int height, int raw_bits, int buf_size) {
    int ret = 0;
    char filePath[256] = {0};
    uint32_t write_in_cnt = 0;
    uint16_t* new16bitsRaw = nullptr;
    uint8_t* new8bitsRaw = nullptr;
    size_t raw_pixel_size = width * height;
    //sprintf(filePath, "%s/%s", OUTPUT_PATH, "isp_s0_raw_4208x3120_8bits.raw");
    //writeFile(rawBuf, "../test_raw/output/raw_4208x3120_8bits_unpack_out.raw", read_out_cnt);
    ISPPipelineIntf ispIntf;
    LOGI("ISP pipeline process start");

    // Raw Buf prepare
    uint8_t* pipelineRawBuf = rawBuf;
    if (8 == raw_bits) {
        // Turn to 16bits, No use code
        LOGI("Turn to 16bits E");
        new16bitsRaw = (uint16_t*)malloc(raw_pixel_size * 2);
        for (uint32_t i = 0; i < raw_pixel_size; i++) {
            *(new16bitsRaw + i) = *(rawBuf + i);
        }
        write_in_cnt = raw_pixel_size * 2;
        writeFile((uint8_t*)new16bitsRaw, OUTPUT_PATH, "PlaneTo16_4208x3120_Turn16bits_out.raw", write_in_cnt);
    } else if (10 == raw_bits) {
        if (buf_size != raw_pixel_size * 2) {
            LOGE("raw buf size err!");
            return -EINVAL;
        }
        LOGI("Turn to 16bits E");
        uint16_t* temp_buf = (uint16_t*)rawBuf;
        for (uint32_t i = 0; i < raw_pixel_size; i++) {
            *(temp_buf + i) = *(temp_buf + i)>>2;   //raw10 >> 2 -> raw8
        }
        LOGI("Turn to 8bits E");    //now only support raw8
        new8bitsRaw = (uint8_t*)malloc(raw_pixel_size);
        for (uint32_t i = 0; i < raw_pixel_size; i++) {
            *(new8bitsRaw + i) = (uint8_t)(*(temp_buf + i));   //raw10 >> 2 -> raw8
        }

        // replace pipelineRawBuf
        pipelineRawBuf = new8bitsRaw;
    }

    // Demosaic
    LOGI("Demosaic E");
    ImgFrame rawFrame;
    rawFrame.imgBuf = pipelineRawBuf;
    rawFrame.width = width;
    rawFrame.height = height;
    rawFrame.bits = 8;  //focus to 8 bits
    rawFrame.type = IMAGE_RAW_GRBG;
    rawFrame.isPacked = false;
    ImgFrame demosaicOut = rawFrame;
    demosaicOut.type = IMAGE_RGB;
    demosaicOut.imgBuf = new uint8_t[rawFrame.width * rawFrame.height * 3];
    memset((void *)demosaicOut.imgBuf, 0, rawFrame.width * rawFrame.height * 3);
    ret = ispIntf.demosaic(&rawFrame, &demosaicOut);
    if( ret != 0) {
        LOGE("demsaic failed!");
    }

    write_in_cnt = raw_pixel_size * 3;    //for RGB
    //writeFile((uint8_t*)demosaicOut.imgBuf, OUTPUT_PATH, "Demosaic_4208x3120_8bits_out.rgb", write_in_cnt);
    writeRGB2Png((uint8_t*)demosaicOut.imgBuf, OUTPUT_PATH, "Demosaic_4208x3120_8bits_out.png",
        rawFrame.width, rawFrame.height);
    // NR
    LOGI("NR E");
    Mat rgbMat(rawFrame.height, rawFrame.width, CV_8UC3, demosaicOut.imgBuf);
    LOGI("NR E1");
    writeMat2File(rgbMat,  OUTPUT_PATH, "NR_4208x3120_8bits_in.png");
    Mat out;
    bilateralFilter(rgbMat, out, 25, 25/2, 25/2);
    LOGI("NR E2");
    writeMat2File(out, OUTPUT_PATH, "NR_4208x3120_8bits_out.png");
    //copy to in
    rgbMat = out.clone();   //? demosaicOut.imgBuf data is not changed
    //writeFile((uint8_t*)out.data, OUTPUT_PATH, "NR_4208x3120_8bits_out.rgb", write_in_cnt);

    // AWB Gain and AWB algo
    LOGI("AWB E");
    AwbCore awbCore;
    ImgFrame awbOut = demosaicOut;
    awbOut.imgBuf = out.data;
    awbCore.process(&awbOut);
    //writeFile((uint8_t*)awbOut.imgBuf, OUTPUT_PATH, "AWB_4208x3120_8bits_out.rgb", write_in_cnt);
    writeRGB2Png((uint8_t*)awbOut.imgBuf, OUTPUT_PATH, "AWB_4208x3120_8bits_out.png",
        rawFrame.width, rawFrame.height);
    //GammaCore
    LOGI("Gamma E");
    GammaCore gammaCore;
    ImgFrame gammaOut = awbOut;
    gammaCore.process(&gammaOut);
    //writeFile((uint8_t*)gammaOut.imgBuf, OUTPUT_PATH, "GAMMA_4208x3120_8bits_out.rgb", write_in_cnt);
    writeRGB2Png((uint8_t*)gammaOut.imgBuf, OUTPUT_PATH, "GAMMA_4208x3120_8bits_out.png",
        rawFrame.width, rawFrame.height);
    LOGI("ISP pipeline process out.\n");

    // Release resource
    free(rawBuf);
    rawBuf = nullptr;
    if (new16bitsRaw != nullptr) {
        free(new16bitsRaw);
        new16bitsRaw = nullptr;
    }

    if (new8bitsRaw != nullptr) {
        free(new8bitsRaw);
        new8bitsRaw = nullptr;
    }

    delete [] demosaicOut.imgBuf;
    demosaicOut.imgBuf = nullptr;
    return 0;
}

int main() {
    // Read input raw
    int ret = 0;
    char filePath[256] = {0};
    uint32_t read_out_cnt = 0;
    uint32_t write_in_cnt = 0;

    // test1
    uint8_t* rawBuf = readFile2Buf( "../../test_raw/raw_4208x3120_8bits.raw", read_out_cnt);
    if( rawBuf == 0) {
        LOGE("readFile2Buf failed!");
        return 0;
    }
    LOGI("%s read:%ld\n", __func__, read_out_cnt);
    //runIspPipeline(rawBuf, 4208, 3120, 8, read_out_cnt);

    //test2
    uint8_t* rawBuf2 = readFile2Buf( "../../test_raw/test2__4640x3488_10_2.raw", read_out_cnt);
    if( rawBuf2 == 0) {
        LOGE("readFile2Buf failed!");
        return 0;
    }
    LOGI("%s read:%ld\n", __func__, read_out_cnt);
    runIspPipeline(rawBuf2, 4640, 3488, 10, read_out_cnt);

    LOGD("openCV test E");
    Mat img = imread("main.jpg");
    imshow("image",img);
    waitKey();

    LOGI("exit\n");
    return 0;
}
