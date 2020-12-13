/**
 * 
 * Author: vicent
 * Create date: 2020/12/02
 * 
 **/
#ifndef ISP_PIPELINE_INTFACE_H
#define ISP_PIPELINE_INTFACE_H

#include <stdint.h>
#include <iostream>

using namespace std;

enum ImgType {
    IMAGE_RAW_RGGB = 0,
    IMAGE_RAW_GBRG,
    IMAGE_RAW_GRBG,
    IMAGE_RGB,
    IMAGE_YUV_NV12,
    IMAGE_YUV_YUYV,
};

struct ImgFrame {
    uint8_t* imgBuf;
    int width;
    int height;
    int bits;
    ImgType type;
    bool isPacked = false;
};

class ISPPipelineIntf {
private:

public:
    ISPPipelineIntf();
    ~ISPPipelineIntf();
    int demosaic(const ImgFrame* rawFrame, const ImgFrame* outputFrame) ;
    int wbGain(const ImgFrame* rawFrame, const ImgFrame* outputFrame) ;
};

#endif
