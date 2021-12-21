// Name: Weichen Qiu
// ID: 1578205

// CMPUT 275 Fall 2020
// WeeklyExercise #3: PseudoCode to Code
#include<iostream>
using namespace std;
#include<cmath>

int ceil(int number1, int number2, int n){
/* 
  ceil rounds up the calculation of (number1 * n) divided by number2
*/
  if (((number1*n) % number2) >0 ){
    // if there is a remainder round up by adding a one
    return (number1*n / number2)+1;
  }
  else{
    // no remainder and divides fully, then just return division
    return int(number1*n/number2);
  }
}

void flipFlopSort(int n,uint32_t *a, int start, int end){
    if (n == 2){
        if (a[start] > a[end-1]){
            // when array is down to 2, swap elements if out of order
            swap(a[start],a[end-1]);
        }
    }else{
      // find the interval, that is the segement of the array a
      uint32_t interval = ceil(2,3,n);
      // make recursive calls to the function to sort the first and last 2/3 of array
      flipFlopSort(interval, a, start, start+interval);
      flipFlopSort(interval, a, end - interval ,end);
      flipFlopSort(interval, a, start, start+interval);
    }
}

int main(){
  int n; // n is the length of array a
  uint32_t a[200]; 
  cin >> n;
  for (int read=0; read<n; read++){
    cin >> a[read];
  }
  if (n==1){
    cout<< a[0];
  }else{
    // call the function
    flipFlopSort(n,a,0,n);

    //print the array
    for (int i=0; i<n;i++){
      cout<<a[i]<<" ";
    }cout<<endl;
  }
}