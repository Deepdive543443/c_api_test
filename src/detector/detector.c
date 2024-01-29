#include "detector.h"

void create_box_vector(BoxVec *box_vector, size_t capacity)
{
    // BoxVec box_vector;
    box_vector->self = &box_vector;
    box_vector->capacity = capacity;
    box_vector->num_item = 0;
    box_vector->data = (BoxInfo *) malloc(sizeof(BoxInfo) * capacity);

    box_vector->getItem = &BoxVec_getItem;
    box_vector->pop = &BoxVec_pop;
    box_vector->remove = &BoxVec_remove;
    box_vector->push_back = &BoxVec_push_back;
    box_vector->insert = &BoxVec_insert;
    box_vector->fit = &BoxVec_fit_size;
    box_vector->free = &BoxVec_free;
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
        printf("Index:%ld out of range\n", index);
        return boxVec->data[boxVec->num_item - 1];
    }

}

BoxInfo BoxVec_pop(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    BoxInfo empty_box;
    
    if(boxVec->num_item > 0)
    {
        BoxInfo empty_box = boxVec->data[boxVec->num_item - 1];
        boxVec->num_item--;
        return empty_box;
    }
    else
    {
        printf("No box in vector\n");
        return empty_box;
    }
}

BoxInfo BoxVec_remove(size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    BoxInfo empty;

    if (index < boxVec->num_item - 1 && index >= 0)
    {
        int num_to_copy = boxVec->num_item - index + 1;
        BoxInfo empty = boxVec->data[index];
        BoxInfo temp[num_to_copy];

        memcpy(&temp, &boxVec->data[index + 1], sizeof(BoxInfo) * num_to_copy);
        memset(&boxVec->data[index], 0.0, sizeof(BoxInfo) * num_to_copy + 1);
        memcpy(&boxVec->data[index], &temp, sizeof(BoxInfo) * num_to_copy);

        boxVec->num_item--;
        return empty;
    }
    else if (index == boxVec->num_item - 1)
    {
        return boxVec->pop(self_ptr);
    }
    printf("Index:%ld out of range\n", index);
    return empty;
}

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

void BoxVec_insert(BoxInfo item, size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    if (index == boxVec->num_item)
    {
        boxVec->push_back(item, self_ptr);
        return;
    }

    if (boxVec->capacity == boxVec->num_item)
    {
        void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->capacity + 20));
        if (data_ptr == NULL)
        {
            printf("Ran out of mem\n");
            return;
        }
        else
        {
            boxVec->data = (BoxInfo *) data_ptr;
            boxVec->capacity += 20;
        }
    }

    int num_to_copy = boxVec->num_item - index;
    BoxInfo temp[num_to_copy];

    memcpy(&temp, &boxVec->data[index], sizeof(BoxInfo) * num_to_copy);
    boxVec->data[index] = item;
    memcpy(&boxVec->data[index+1], &temp, sizeof(BoxInfo) * num_to_copy);
    boxVec->num_item++;
}

void BoxVec_fit_size(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->num_item));
    if (data_ptr == NULL)
    {
        printf("Ran out of mem\n");
        return;
    }
    else
    {
        boxVec->data = (BoxInfo *) data_ptr;
        boxVec->capacity = boxVec->num_item;
    }
}

void BoxVec_free(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    free(boxVec->data);
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

void qsort_descent_inplace(BoxVec *objects, int left, int right)
{
    int i = left;
    int j = right;

    float p = objects->getItem((int) (left + right) / 2, objects).prob;

    while (i <= j)
    {
        while (objects->getItem(i, objects).prob > p)//(objects[i].prob > p)
            i++;

        while (objects->getItem(j, objects).prob < p)
            j--;

        if (i <= j)
        {
            // swap
            BoxInfo temp = objects->getItem(i, objects);
            memcpy(&objects->data[i], &objects->data[j], sizeof(BoxInfo));
            objects->data[j] = temp;

            i++;
            j--;
        }
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            if (left < j) qsort_descent_inplace(objects, left, j);
        }
        #pragma omp section
        {
            if (i < right) qsort_descent_inplace(objects, i, right);
        }
    }
}

float intersection(BoxInfo *box1, BoxInfo *box2)
{
    float xA = fmaxf(box1->x1, box2->x1);
    float yA = fmaxf(box1->y1, box2->y1);
    float xB = fminf(box1->x2, box2->x2);
    float yB = fminf(box1->y2, box2->y2);

    return fmaxf(0, xB - xA) * fmaxf(0, yB - yA);
}