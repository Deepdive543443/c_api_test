#include "detector.h"

void create_box_vector(BoxVec *box_vector, size_t capacity)
{
    // BoxVec box_vector;
    box_vector->self = &box_vector;
    box_vector->capacity = capacity;
    box_vector->num_item = 0;
    box_vector->data = (BoxInfo *) malloc(sizeof(BoxInfo) * capacity);

    box_vector->getItem = &BoxVec_getItem;
    // box_vector.pop = &BoxVec_pop;
    // box_vector.remove = &BoxVec_remove;
    box_vector->push_back = &BoxVec_push_back;
    // box_vector.insert = &BoxVec_insert;
}

BoxInfo BoxVec_getItem(size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    if (index < boxVec->num_item && index >= 0)
    {
        return boxVec->data[index];
    }
    else
    {
        printf("Index out of range\n");
        return boxVec->data[boxVec->num_item - 1];
    }

}

BoxInfo BoxVec_pop(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    size_t num_item = boxVec->num_item;
    BoxInfo empty_box;
    
    if(num_item > 0)
    {
        BoxInfo empty_box = boxVec->data[num_item - 1];
        return empty_box;
    }
    else
    {
        printf("No box in vector\n");
        return empty_box;
    }
}

/*TODO*/

void BoxVec_push_back(BoxInfo item, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    size_t num_item = boxVec->num_item;
    if (boxVec->capacity == boxVec->num_item)
    {
        void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->capacity + 20));
        if (data_ptr == NULL)
        {
            printf("Ran out of mem\n");
        }
        else
        {
            boxVec->data = (BoxInfo *) data_ptr;
            boxVec->data[boxVec->num_item] = item;
            boxVec->capacity += 20;
            boxVec->num_item++;
        }
    }
    else if (boxVec->capacity > boxVec->num_item)
    {
        boxVec->data[boxVec->num_item] = item;
        boxVec->num_item++;
    }
}


float fast_exp(float x)
{
    union {
        __uint32_t i;
        float f;
    } v;
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

float fast_sigmoid(float x)
{
    return 1.0f / (1.0f + fast_exp(-x));
}

float fast_tanh(float x)
{
    return 2.f / (1.f + fast_exp(-2 * x)) - 1.f;
}

int activation_function_softmax_inplace(float *src, int length)
{
    float alpha = -FLT_MAX;
    for (int i = 0; i < length; ++i)
    {
        if (alpha < src[i]) alpha = src[i];
    }

    float denominator = 0.f;

    for (int i = 0; i < length; ++i)
    {
        src[i] = fast_exp(src[i] - alpha);
        denominator += src[i];
    }

    for (int i = 0; i < length; ++i)
    {
        src[i] /= denominator;
    }
    return 0;
}