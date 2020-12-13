#ifndef AWB_CORE_H
#define AWB_CORE_H

#include "ISPPipelineIntf.h"

class AwbCore {
public:
    AwbCore();
    ~AwbCore();
    int process(ImgFrame* pImg);
    int calRgbAvg(ImgFrame* pImg, int &r, int &g, int &b);
    int wbGainProc(ImgFrame* pImg, float rGain, float bGain);
    int digitalGainProc(ImgFrame* pImg, float digitalGain);
    //int colorTemp(ImgFrame* pImg);
};
#endif
