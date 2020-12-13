#include "AwbCore.h"
#include "./core/ISPLog.h"

AwbCore::AwbCore() {
}

AwbCore::~AwbCore() {
}

int AwbCore::process(ImgFrame* pImg) {
    int r =0, g = 0, b = 0;
    int rgbAvg = 0;
    if (pImg->type != IMAGE_RGB) {
        LOGE("%s: not support image type", __func__);
        return -1;
    }
    calRgbAvg(pImg, r, g, b);
    LOGI("%s: r:%d g:%d b:%d", __func__, r, g, b);
    float ratioGR = (float)g / r;
    float ratioGB = (float)g / b;
    LOGI("%s: ratioGR:%f ratioGR:%f", __func__, ratioGR, ratioGB);
    float rGain = ratioGR;
    float bGain = ratioGB;

    //wbGainProc
    wbGainProc(pImg, rGain, bGain);
    //D Gain
    float digitalGain = 2.0;
    digitalGainProc(pImg, digitalGain);
    return 0;

}

int AwbCore::calRgbAvg(ImgFrame* pImg, int &r, int &g, int &b) {
    uint64_t r_sum = 0, g_sum = 0, b_sum = 0;
    uint8_t* buf = pImg->imgBuf;
    int image_size = pImg->width * pImg->height;
    for (int i = 0; i < image_size; i++) {
        r_sum += buf[0];
        g_sum += buf[1];
        b_sum += buf[2];
        buf += 3;
    }

    r = r_sum / image_size;
    g = g_sum / image_size;
    b = b_sum / image_size;
    return 0;
}

int AwbCore::wbGainProc(ImgFrame* pImg, float rGain, float bGain) {
    uint8_t* buf = pImg->imgBuf;
    int image_size = pImg->width * pImg->height;
    for (int i = 0; i < image_size; i++) {
        if(buf[0] * rGain <= 255) {
            buf[0] = buf[0] * rGain;
        }
        if(buf[2] * bGain <= 255) {
            buf[2] = buf[2]* bGain;
        }
        buf += 3;
    }
    return 0;
}

int AwbCore::digitalGainProc(ImgFrame* pImg, float digitalGain) {
    uint8_t* buf = pImg->imgBuf;
    int image_size = pImg->width * pImg->height;
    for (int i = 0; i < image_size * 3; i++) {
        if(buf[0] * digitalGain <= 255) {
            buf[0] = buf[0] * digitalGain;
        } else {
             buf[0] = 255;
        }
        buf++;
    }
    return 0;
}