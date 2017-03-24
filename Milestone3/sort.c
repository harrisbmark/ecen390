#include "sort.h"

//here is our swap function
void swap(double* data, uint32_t i, uint32_t j)
{
    double temp;
    //Create a temporary variable
    temp = data[i];
    //Set temporary variable as the first value
    data[i] = data[j];
    //Switch the first value with the second
    data[j] = temp;
    //Set the second value to the temporary value
}

//This is the partition function
uint32_t partition(double* data, uint32_t first, uint32_t last)
{
    double pivot = data[first]; //Pivot point is based on the first
    uint32_t i = first - 1; //Set i to the left of the first
    uint32_t j = last + 1; //Set j to the right of the last

    while (true)
    {
        do i++; //increment i
        while (data[i] > pivot);
        //While i is greater than our pivot point
        do j--; //Decrement j
        while (data[j] < pivot);
        //While j is less than our pivot point
        if (i >= j)
            //Return j if i is greater than j
            return j;
        //Swap i and j
        swap(data, i, j);
    }
}

void quicksort_algorithm(double* data, uint32_t first, uint32_t last)
{
    //If the first value is less than the last value
    if (first < last)
    {
        //Call partition function
        uint32_t temp_partition = partition(data, first, last);
        //Call quicksort algorithm on the first half of the partition
        quicksort_algorithm(data, first, temp_partition);
        //Call quicksort algorithm on the last half of the partition
        quicksort_algorithm(data, temp_partition + 1, last);
    }
}

//This function will take an array and will sort them based on their values
void quicksort(double* data, uint32_t size)
{
    //Call the quick_sort algorithm with data
    quicksort_algorithm(data, 0, size - 1);
}
