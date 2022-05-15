#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <ctime>

int *get_random_array(size_t n)
{
    int *arr = new int[n];
    int *__tail = arr;
    srand(time(NULL));
    for (size_t i = 0; i < n; ++i, ++__tail)
    {

        *__tail = ((rand() % 2 ? -1 : 1) * rand()) / 10;
    }
    return arr;
}

#endif