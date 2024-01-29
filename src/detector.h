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


/**
 * @here we go again https://stackoverflow.com/questions/4694401/how-to-replicate-vector-in-c
 * TODO -- Get this vector of boxxes done
 */
typedef struct
{
    BoxInfo *data;
    size_t capacity;
    size_t num_item;
} BoxVec;

BoxVec create_box_vector();
void BoxVec_push_back();
void BoxVec_insert();

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