#include<iostream>
#include<Arduino.h>
using namespace std;

int isort(int length, int *A){
	uint16_t startTime = millis();
	int i = 1;
	while (i < length){
		int j = i;
		while (j > 0 && A[j-1] > A[j]){
			swap(A[j], A[j-1]);
			j--;
		}
		i++;
	}
	uint16_t endTime = millis();
	return endTime - startTime;


}
int main(){
	int A[200]={1,9,3,2,1};
	isort(5,A);
	for (int i=0;i<5;i++){
		cout<<A[i]<<" ";
	}
}