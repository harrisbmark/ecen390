#include "sort.h"

void swap(double* data, uint32_t i, uint32_t j)
{
    double temp;

    temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}

uint32_t partition(double* data, uint32_t first, uint32_t last)
{
    double pivot = data[first];
    uint32_t i = first - 1;
    uint32_t j = last + 1;

    while (true)
    {
        do i++;
        while (data[i] > pivot);

        do j--;
        while (data[j] < pivot);

        if (i >= j)
            return j;

        swap(data, i, j);
    }
}

void quicksort_algorithm(double* data, uint32_t first, uint32_t last)
{
    if (first < last)
    {
        uint32_t temp_partition = partition(data, first, last);

        quicksort_algorithm(data, first, temp_partition);
        quicksort_algorithm(data, temp_partition + 1, last);
    }
}

void quicksort(double* data, uint32_t size)
{
    quicksort_algorithm(data, 0, size - 1);
}

