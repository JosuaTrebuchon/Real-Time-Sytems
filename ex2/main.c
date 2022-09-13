#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

void task_a(long x_dim) {
    long xy_size = 1000*1000*500;
    //long x_dim = 100;
    long y_dim = xy_size/x_dim;
    printf("x: %ld,   y: %ld,\n",x_dim, y_dim );

    long** matrix = malloc(y_dim*sizeof(long*));

    for(long y = 0; y < y_dim; y++) {
        matrix[y] = malloc(x_dim*sizeof(long));
    }

    printf("Allocation complete (press any key to continue...)\n");
    getchar();
}

void task_a_0s(long x_dim) {
    long xy_size = 1000*1000*500;
    //long x_dim = 100;
    long y_dim = xy_size/x_dim;
    printf("x: %ld,   y: %ld,\n",x_dim, y_dim );

    long** matrix = malloc(y_dim*sizeof(long*));

    for(long y = 0; y < y_dim; y++) {
        matrix[y] = malloc(x_dim*sizeof(long));
        memset(matrix[y], 0, x_dim*sizeof(long));
    }

    

    printf("Allocation complete (press any key to continue...)\n");
    getchar();
}


void task_b_insertBack() {
    Array arr = array_new(3);
    
    array_insertBack(&arr,100);
    array_insertBack(&arr,150);
    array_insertBack(&arr,12);
    array_insertBack(&arr,140);

    array_print(arr);

    array_destroy(arr);
}

void task_c_reserve() {
    Array arr = array_new(3);
    
    array_insertBack(&arr,100);
    array_insertBack(&arr,150);
    array_insertBack(&arr,12);

    array_reserve(&arr,10);

    array_print(arr);



    array_destroy(arr);
}

void task_c_addReserve() {
    Array arr = array_new(4);
    

    array_insertBack(&arr,100);
    array_insertBack(&arr,150);
    array_insertBack(&arr,12);
    array_insertBack(&arr,190);
    array_insertBack(&arr,174);
    array_insertBack(&arr,498);
    array_insertBack(&arr,2100);
    array_insertBack(&arr,2150);
    array_insertBack(&arr,212);
    array_insertBack(&arr,2190);
    array_insertBack(&arr,2174);
    array_insertBack(&arr,2498);


    array_print(arr);

    printf("Front: %ld, Back: %ld, Capacity: %ld\n",arr.front,arr.back, arr.capacity);


    array_destroy(arr);
}


void task_d() {
    //Atempt at plotting using gnuplot
    
    long maxcap = 1024;

    long i;
    Array arr = array_new(2);

    for(i = 0; i < maxcap; i++ ){

        array_insertBack(&arr, i);

        printf("%ld %ld\n", arr.capacity, array_length(arr));

    }
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("No argument given !\n");
        return -1;
    }
    switch(atoi(argv[1]))
    {
        case 1:
            task_a(100);
            break;
        case 2:
            task_a(1000);
            break;
        case 3:
            task_a(10000);
            break;
        case 4:
            task_a_0s(10000);
            break;
        case 5:
            task_b_insertBack();
            break;
        case 6:
            task_c_reserve();
            break;
        case 7:
            task_c_addReserve();
            break;
        case 8:
            task_d();
            break;
            
    }

    return 0;
}