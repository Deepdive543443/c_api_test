#include "c_api.h"

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
    int label;
    float prob
} BoxInfo;

typedef struct
{
    ncnn_net_t net;
    int input_w;
    int input_h;
    BoxInfo (*detect)(unsigned char);
} Detector;

/**
 * Todo -- General function that share with all detector
 */

/**
 * Todo -- Nanodet's widget
 */

/**
 * Todo -- FastestDet's widget
 */