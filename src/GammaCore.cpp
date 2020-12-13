#include "GammaCore.h"
#include <math.h>
#include "./core/ISPLog.h"

int geneTable(uint8_t *table) {
    double gamma = 1/1.5;   //2.2
    printf("\n");
    for (int i = 2; i < 256; i++) {
        table[i] = (pow( (i)/256.0, gamma)) * 255;
        printf("%d:%d ", i , (int)table[i]);
    }
    printf("\n");
    return 0;
}

int GammaCore::process(ImgFrame* pImg) {
    uint8_t table[256] = {0};
    geneTable(table);

    uint8_t* buf = pImg->imgBuf;
    int image_size = pImg->width * pImg->height;
    for (int i = 0; i < image_size * 3; i++) {
        buf[i] = table[buf[i]];
    }
    return 0;
}