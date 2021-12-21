#include<iostream>
using namespace std;

int32_t partition( int *a, int32_t left_index, int32_t right_index, int32_t pivot){
    while (left_index <= right_index){
        // keep looping if left is smaller than right index
        while (a[left_index] < pivot){
            // keep moving left index right until value is bigger than pivot
            left_index++;
        }
        while (a[right_index] > pivot){
            // keep moving right index left unril value is less than pivot
            right_index--;
        }
        if (left_index <= right_index){
            // swap the two values so that small is on left and big is on right of pivot
            swap(a[left_index], a[right_index]);
            // increment left and right indexes
            left_index++;
            right_index--;
        }
    }
    // return the left index
    return left_index;
}
int quickSort(int *a, int32_t left_index, int32_t right_index){
    if (left_index < right_index){
        // pivot chosen to be center of array
        int pivot = a[(left_index+right_index)/2];
        // call partition to get the pivot index
        uint32_t pivot_index = partition(a, left_index, right_index, pivot);
        //recursively call twice
        quickSort(a, left_index, pivot_index-1);
        quickSort(a, pivot_index, right_index);
    }
}

int main(){

    while(true){
    int a[5];
    int n=5;
    for (int i=0;i<5;i++){
        cin >> a[i];
    }
    quickSort(a,0,n-1);

    for (int i=0;i<n;i++){
        cout<<a[i]<<" ";
    }cout<<endl;

}
}