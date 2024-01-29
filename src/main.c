#include "stdio.h"
#include "stdlib.h"
#include "c_api.h"

#include "detector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// #define STB_DS_IMPLEMENTATION
// #include "stb_ds.h"




void print_mat(ncnn_mat_t *mat)
{
    printf("w: %d\nh: %d\nc: %d\nelesize: %d\ncstep: %d\n", ncnn_mat_get_w(mat), ncnn_mat_get_h(mat), ncnn_mat_get_c(mat), ncnn_mat_get_elemsize(mat), ncnn_mat_get_cstep(mat));
}

int main(int argc, char** argv)
{
    // union 
    // {
    //     struct
    //     {
    //         __uint8_t r;
    //         __uint8_t g;
    //         __uint8_t b;
    //         __uint8_t a;
    //     };
    //     __uint32_t rgba;
    // } color;

    // color.r = 216;
    // color.g = 82;
    // color.b = 24;
    // color.a = 255;

    int target_size = 416;

    // Allocate pixel
    // unsigned char *pixel = malloc(sizeof(unsigned char) * 320 * 320 * 3);
    // memset(pixel, 255, 320 * 320 * 3); // White image 

    // /**
    //  * Load image using stb image
    //  */
    // int width, height, n;
    // const char *file = "../asset/1.jpg";
    // unsigned char *pixels = stbi_load(file, &width, &height, &n, 0);
    // printf("%d %d %d\n", width, height, n);


    // /**
    //  * Create nanodet
    //  * 
    //  */
    // Detector nanodet = create_nanodet(target_size);
    // BoxInfo *objects = NULL; // Dynamic array doesn't work for now, waiting for an implementation
    // nanodet.detect(pixels, width, height, objects, &nanodet);
    // free(pixels);


    /**
     * CTest box array
     * 
     */

    BoxVec boxVec;
    create_box_vector(&boxVec, 10);
    BoxVec_pop(&boxVec);
    for (int i = 0; i < 60; i++)
    {
        BoxInfo box = {i,i,i,i,i,i};
        // printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
        boxVec.push_back(box, &boxVec);
        
        printf("%d %d \n", boxVec.num_item, boxVec.capacity);

    }

    // Testing getItem
    BoxInfo box;
    box = boxVec.getItem(1, &boxVec);
    printf("\n%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
    box = boxVec.getItem(54, &boxVec);
    printf("\n%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);



    

    // BoxInfo obj = objects[0];
    // printf("%f %f %f %f %f %d\n", objects[0].x1, objects[0].x2, objects[0].y1, objects[0].y2, objects[0].prob, objects[0].label);


    // BoxInfo *boxxes = NULL;
    // BoxInfo xbox = {0,0,1,1,26, 0.8};

    // stbds_arrput(boxxes, xbox);

    // printf("%d %d %d %f ", boxxes[0].x1, boxxes[0].y1, boxxes[0].label, boxxes[0].prob);
    // float trrr[] = {0.123, 1.1, -0.312, 0.9712, 1.12312, 2.112753, -2.123124, -0.128973};
    // activation_function_softmax_inplace(&trrr, 8);

    // printf("%f %f %f %f %f %f %f %f", trrr[0], trrr[1], trrr[2], trrr[3], trrr[4], trrr[5], trrr[6], trrr[7]);
    printf("\nC API TEST\n");
}