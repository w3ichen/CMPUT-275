#include<iostream>

using namespace std;

int insertionSort(int length, int *A){

	int i = 1;
	while (i < length){
		int j = 1;
		while (j > 0 && A[j-1] > A[j]){
			swap(A[j], A[j-1]);
			j--;
		}
		i++;
	}


}
int main(){
	int A[200]={5,4,3,2,1};
	insertionSort(5,A);
	for (int i=0;i<5;i++){
		cout<<A[i]<<" ";
	}
}