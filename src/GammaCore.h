#ifndef GAMMA_CORE_H
#define GAMMA_CORE_H

#include "ISPPipelineIntf.h"

class GammaCore {
public:
    GammaCore() {};
    ~GammaCore() {};
    int process(ImgFrame* pImg);
};
#endif
