#include "stdio.h"
#include "detector/detector.h"

int main(int argc, char** argv)
{
    BoxVec boxVec;
    create_box_vector(&boxVec, 10);
    boxVec.pop(&boxVec);


    // Testing realloc
    for (int i = 0; i < 20; i++)
    {
        BoxInfo box = {i,i,i,i,i,i};
        // printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
        boxVec.push_back(box, &boxVec);
        printf("%ld %ld \n", boxVec.num_item, boxVec.capacity);
    }

    // Testing remove
    boxVec.remove(-12, &boxVec);
    boxVec.remove(998, &boxVec); 
    boxVec.remove(19, &boxVec); printf("%ld %ld \n", boxVec.num_item, boxVec.capacity);
    boxVec.remove(7, &boxVec); printf("%ld %ld \n", boxVec.num_item, boxVec.capacity);
    
    // Testing insert
    for (int i=0; i < 20; i++)
    {
        int temp = i + 30;
        BoxInfo box = {temp,temp, temp, temp, temp, temp };
        boxVec.insert(box, 5, &boxVec);
        printf("%ld %ld \n", boxVec.num_item, boxVec.capacity);
    }

    // Testing getItem
    BoxInfo box;
    for (int i=0; i < boxVec.num_item; i++)
    {
   
        BoxInfo box = {i,i,i,i,i,i};
        box = boxVec.getItem(i, &boxVec);
        printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
    }

    //Test free
    boxVec.free(&boxVec);

    printf("Box Vec Test\n");
}