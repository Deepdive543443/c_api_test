#pragma once
#include "float.h"
#include "c_api.h"

typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
    
    float prob;
    int label;
} BoxInfo;

typedef BoxInfo* (*Detect) (unsigned char *pixels, int pixel_w, int pixel_h, BoxInfo *objects, void *self_ptr);

typedef struct
{
    void *self;
    ncnn_net_t net;
    int input_size;
    float mean_vals[3];
    float norm_vals[3];
    Detect detect;
} Detector;



/**
 * Todo -- General function that share with all detector
 */

float fast_exp(float x);
float fast_sigmoid(float x);
float fast_tanh(float x);
int activation_function_softmax_inplace(float *src, int length);

/**
 * Todo -- Nanodet's widget
 */

Detector create_nanodet(int input_size);
BoxInfo *nanodet_detect(unsigned char *pixels, int pixel_w, int pixel_h, BoxInfo *objects, void *self_ptr);


/**
 * Todo -- FastestDet's widget
 */