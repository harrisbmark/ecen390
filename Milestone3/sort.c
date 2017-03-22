#include "sort.h"

void swap(uint32_t* data, uint32_t i, uint32_t j)
{
    uint32_t temp;

    temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}

uint32_t partition(uint32_t* data, uint32_t first, uint32_t last)
{
    uint32_t pivot = data[first];
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

void quicksort_algorithm(uint32_t* data, uint32_t first, uint32_t last)
{
    if (first < last)
    {
        uint32_t temp_partition = partition(data, first, last);

        quicksort_algorithm(data, first, temp_partition);
        quicksort_algorithm(data, temp_partition + 1, last);
    }
}

void quicksort(uint32_t* data, uint32_t size)
{
    quicksort_algorithm(data, 0, size - 1);
}

