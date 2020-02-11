#include<iostream>
using namespace std;

int qsort(int length, int *a, int left_index, int right_index){
	uint16_t startTime = millis();
	if (left_index >= right_index){
		// stop programs if left is past or equal to the right index
		return 0;
	}
	int pivot = (left_index + right_index)/2;
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
			left_index++;
			right_index--;
		}

	}
	uint16_t endTime = millis();
	return endTime-startTime;
}

int main(){

	int a[]={9,1,9,2,1};
	qsort(5,a,0,5);

	for (int i=0;i<5;i++){
		cout<<a[i]<<" ";
	}cout<<endl;
}