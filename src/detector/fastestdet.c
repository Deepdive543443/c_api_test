#include "detector.h"

Detector create_fastestdet(int input_size, const char* param, const char* bin)
{
    Detector fastestdet;
    // nanodet.self = &nanodet;

    fastestdet.net = ncnn_net_create();
    ncnn_net_load_param(fastestdet.net, param);
    ncnn_net_load_model(fastestdet.net, bin);

    fastestdet.input_size = input_size;
    fastestdet.mean_vals[0] = 0.0f;
    fastestdet.mean_vals[1] = 0.0f;
    fastestdet.mean_vals[2] = 0.0f;
    fastestdet.norm_vals[0] = 0.00392157f;
    fastestdet.norm_vals[1] = 0.00392157f;
    fastestdet.norm_vals[2] = 0.00392157f;

    fastestdet.detect = &fastestdet_detect;
    return fastestdet;
}