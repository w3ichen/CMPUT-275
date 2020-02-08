#include<iostream>

using namespace std;

int isort(int length, int *A){

	int i = 1;
	while (i < length){
		int j = i;
		while (j > 0 && A[j-1] > A[j]){
			swap(A[j], A[j-1]);
			j--;
		}
		i++;
	}



}
int main(){
	int length=50;
	int A[length]={1,9,3,2,1,5,99,3,5,1,5,0,5,6,3,22,56,98,88};
	isort(length,A);
	for (int i=0;i<length;i++){
		cout<<A[i]<<" ";
	}
}