// Name: Weichen Qiu
// ID: 1578205

// CMPUT 275 Fall 2020
// WeeklyExercise #3: PseudoCode to Code

#include <iostream>
#include<cstring> //use strlen()
using namespace std;

void testMatch(const char *s, const char *t){
	// phase 1
	int step[100001];
	step[0] = -1;
	int k = -1;

	for (int i=1; i<int(strlen(s));i++){
		while ((k > 0 || k==0) && s[k+1] != s[i]){
			k= step[k];
		}
		if (s[k+1] == s[i]){
			k++;
		}
		step[i] = k;
	}

	// phase 2
	int m = -1;
	for (int i=0; i<int(strlen(t)); i++){
		while (((m > 0)||(m==0)) && s[m+1] != t[i]){
			m = step[m];
		}
		if (s[m+1] == t[i]){
			m++;
		}
		if (m == int(strlen(s)-1)){
			cout << (i+1-strlen(s))<<" ";
			m = step[m];
		}
	}
	cout<<endl;
}

int main(){
	// read in the inputs
	char s[100001], t[100001];
	cin >> s;
	cin >> t;
	// call the function
	testMatch(s, t);

}