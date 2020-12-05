/**
 * 
 * Author: vicent
 * Create date: 2020/12/02
 * 
 **/

#include <iostream>

using namespace std;

enum ImgType {
    IMAGE_RAW = 0,
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
};

ISPPipelineIntf::ISPPipelineIntf() {
}

ISPPipelineIntf::~ISPPipelineIntf() {
}

int ISPPipelineIntf::demosaic(const ImgFrame* rawFrame, const ImgFrame* outputFrame) {
    return 0;
}
