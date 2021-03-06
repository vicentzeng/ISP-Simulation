#include "ISPPipelineIntf.h"

ISPPipelineIntf::ISPPipelineIntf() {
}

ISPPipelineIntf::~ISPPipelineIntf() {
}

int ISPPipelineIntf::demosaic(const ImgFrame* rawFrame, const ImgFrame* outputFrame) {
    uint8_t* bayer = rawFrame->imgBuf;
    uint8_t* dst = outputFrame->imgBuf;
    size_t raw_size = rawFrame->width * rawFrame->height;
    int w = rawFrame->width;
    int h = rawFrame->height;
    if (8 != rawFrame->bits)
    {
        return -1;
    }

    //skip bounder 1
    for (size_t i = 1; i < h - 1; i++) {
        for (size_t j = 1; j < w - 1; j++) {
            /* code */
            int post_w_h = i * w + j;
            int post_n1_n1 = (i - 1) * w + j - 1; //左上
            int post_n1_0 = (i - 1) * w + j;      //上
            int post_n1_1 = (i - 1) * w + j + 1;  //右上
            int post_0_n1 = i * w + j - 1;        //左
            int post_0_0 = i * w + j;
            int post_0_1 = i * w + j + 1;
            int post_1_n1 = (i + 1) * w + j - 1;
            int post_1_0 = (i + 1) * w + j;
            int post_1_1 = (i + 1) * w + j + 1;
            int R_id = post_w_h * 3;
            int G_id = post_w_h * 3 + 1;
            int B_id = post_w_h * 3 + 2;

            //RGGB
            if (rawFrame->type == IMAGE_RAW_RGGB) {
                if (i%2 == 0) {
                    if (j%2 == 0) {
                        dst[post_w_h * 3] = bayer[post_0_0];  // R origin [0,0]
                        dst[post_w_h * 3 + 1] = ((uint16_t)bayer[post_n1_0] + (uint16_t)bayer[post_1_0] + (uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1])/ 4;  // G
                        dst[post_w_h * 3 + 2] = (bayer[post_n1_n1] + bayer[post_n1_1] + bayer[post_1_n1] + bayer[post_1_1]) / 4; //B
                    } else {
                        dst[post_w_h * 3] = ((uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1]) / 2; //R
                        dst[post_w_h * 3 + 1] = bayer[post_0_0];  // G origin[0,1]
                        dst[post_w_h * 3 + 2] = (bayer[post_n1_0] + bayer[post_1_0]) / 2; //B
                    }
                } else {
                    if (j%2 == 0) {
                        dst[post_w_h * 3] = ((uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1]) / 2; //R
                        dst[post_w_h * 3 + 1] = bayer[post_0_0];  // G origin [1,0]
                        dst[post_w_h * 3 + 2] = ((uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1]) / 2; // B
                    } else {
                        dst[post_w_h * 3 + 1] = ((uint16_t)bayer[post_0_0] + (uint16_t)bayer[post_0_0] + (uint16_t)bayer[post_0_0] + (uint16_t)bayer[post_0_0]) / 4;  // G
                        dst[post_w_h * 3 + 2] = bayer[post_0_0];  // B origin[1,1]
                        dst[post_w_h * 3] = ((uint16_t)bayer[post_n1_n1] + (uint16_t)bayer[post_n1_1] + (uint16_t)bayer[post_1_n1] + (uint16_t)bayer[post_1_1]) / 4; //R
                    }
                }
            }

            //GBRG
            if (rawFrame->type == IMAGE_RAW_GBRG || rawFrame->type == IMAGE_RAW_GRBG) {

                //temp switch R and B to solve, not efficient
                if (rawFrame->type == IMAGE_RAW_GRBG) {
                    int temp = B_id;
                    B_id = R_id;
                    R_id = temp;
                }

                if (i%2 == 0) {
                    if (j%2 == 0) {
                        dst[G_id] = bayer[post_0_0];  // G origin [0,0]
                        dst[B_id] = ((uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1]) / 2; // 左右/2 B
                        dst[R_id] = ((uint16_t)bayer[post_1_0] + (uint16_t)bayer[post_n1_0]) / 2; // 上下 R
                    } else {
                        dst[R_id] = ((uint16_t)bayer[post_n1_n1] + (uint16_t)bayer[post_n1_1] + (uint16_t)bayer[post_1_n1] + (uint16_t)bayer[post_1_1]) / 4; // 四角/4 R
                        dst[B_id] = bayer[post_0_0];  // B origin[0,1]
                        dst[G_id] = ((uint16_t)bayer[post_n1_0] + (uint16_t)bayer[post_1_0] + (uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1])/ 4;  // G
                    }
                } else {
                    if (j%2 == 0) {
                        dst[R_id] = bayer[post_0_0];  // R origin [1,0]
                        dst[G_id] = ((uint16_t)bayer[post_n1_0] + (uint16_t)bayer[post_1_0] + (uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1])/ 4;  // G
                        dst[B_id] = ((uint16_t)bayer[post_n1_n1] + (uint16_t)bayer[post_n1_1] + (uint16_t)bayer[post_1_n1] + (uint16_t)bayer[post_1_1]) / 4; // 四角/4 B
                    } else {
                        dst[R_id] = ((uint16_t)bayer[post_0_n1] + (uint16_t)bayer[post_0_1]) / 2; // 左右/2 R
                        dst[G_id] = bayer[post_0_0];  // G origin[1,1]
                        dst[B_id] = ((uint16_t)bayer[post_1_0] + (uint16_t)bayer[post_n1_0]) / 2; // 上下B
                    }
                }
            }

            //debug
            if (i > 368  && i < 378 && j < 10 )
            {
                //printf("%d ",  (int)dst[post_w_h * 3]);

                //printf("%d ",  (int)bayer[post_0_0]);

                //printf("%d %d %d\n", (int)bayer[post_n1_n1], (int)bayer[post_n1_0], (int)bayer[post_n1_1]);
                //printf("%d %d %d\n", (int)bayer[post_0_n1], (int)bayer[post_0_0], (int)bayer[post_0_1]);
                //printf("%d %d %d\n", (int)bayer[post_0_n1 + w], (int)bayer[post_1_0], (int)bayer[post_1_1]);
                //if (j == 1) printf("i = %d \n", i);
            }
            if (i > 3110  && i < 3120 && j < 10 )
            {
                //printf("%d ",  (int)dst[post_w_h * 3]);

                //printf("%d ",  (int)bayer[post_0_0]);

                //printf("%d %d %d\n", (int)bayer[post_n1_n1], (int)bayer[post_n1_0], (int)bayer[post_n1_1]);
                //printf("%d %d %d\n", (int)bayer[post_0_n1], (int)bayer[post_0_0], (int)bayer[post_0_1]);
                //printf("%d %d %d\n", (int)bayer[post_0_n1 + w], (int)bayer[post_1_0], (int)bayer[post_1_1]);
                //if (j == 1) printf("i = %d \n", i);
            }
            
        }   //end i
    }
    printf("\n");
    //todo 边缘一圈
    return 0;
}
