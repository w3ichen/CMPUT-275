/*
	Name: Weichen Qiu
	ID: 1578205
	CMPUT 275, Winter 2020

	Weekly Exercise #6: STL Concepts
	Task #2: Post Fix calculator
*/
//use pair and stack
#include <iostream>
#include <utility> // for pair
#include <stack> // for stack
using namespace std;

int main(){
	stack <pair<long long,long long>> s; // stack of pairs
	pair <long long,long long> p; // pair of real and imaginary

	// declare variables to store calculations and inputs
	string command;
	string operand;
	pair <long long,long long> result, pair1, pair2;
	// keep looping
	while (true){
		cin >> command;
		if (command == "V" || command == "v"){
			// complex operand "V r1 r2" , where r1 + i*r2
			cin >> p.first; //real part
			cin >> p.second; //imaginary part
			s.push(p); // add pair to the stack
		}else if (command == "B" || command == "b"){
			// binary operator: "+" or "-" or "*"
			cin >> operand;
			if (operand == "+"){
				// addition
				pair1 = s.top(); // get top item
				s.pop(); // remove top item
				pair2 = s.top(); // get new top item again
				s.pop(); // remove top item again
				result.first = pair1.first + pair2.first; // add reals together
				result.second = pair1.second + pair2.second; // add imag together
				s.push(result); //add the results to the stack
				//cout<<result.first<<" + "<<result.second<<"i"<<endl;
			}else if (operand == "-"){
				// subtraction
				pair1 = s.top(); // newest
				s.pop();
				pair2 = s.top(); // oldest
				s.pop();
				result.first = pair2.first - pair1.first; // subtract reals
				result.second = pair2.second - pair1.second; // subtract imagi
				s.push(result); //add the results to the stack
				//cout<<result.first<<" + "<<result.second<<"i"<<endl;
			}else if (operand == "*"){
				// multiplication
				pair1 = s.top(); 
				s.pop();
				pair2 = s.top(); 
				s.pop();
				result.first = (pair1.first*pair2.first)  - (pair1.second*pair2.second); // subtract reals
				result.second = (pair1.first*pair2.second) + (pair1.second*pair2.first); // subtract imagi
				s.push(result); //add the results to the stack
				//cout<<result.first<<" + "<<result.second<<"i"<<endl;
			}

		}else if (command == "U" || command == "u"){
			// unary operator
			cin >> operand;
			if (operand == "-"){
				// negation
				result = s.top(); 
				s.pop();
				result.first *= -1; //negate
				result.second *= -1;
				s.push(result); //add the results to the stack
				//cout<<result.first<<" + "<<result.second<<"i"<<endl;
			}else if (operand == "c"){
				// conjugation
				result = s.top(); 
				s.pop();
				result.second *= -1; // flip signs of imaginary
				s.push(result); //add the results to the stack
				//cout<<result.first<<" + "<<result.second<<"i"<<endl;
			}
		}
		else if (command == "S" || command == "s"){
			// STOP
			result = s.top();
			cout << result.first << " " << result.second << endl; //print the answer
			break;
		}
	}
}