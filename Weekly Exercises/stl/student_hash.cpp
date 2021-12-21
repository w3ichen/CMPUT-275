/*
	Name: Weichen Qiu
	ID: 1578205
	CMPUT 275, Winter 2020

	Weekly Exercise #6: STL Concepts
	Task #1: Hashing a custom struct
*/
#include <iostream>
#include <unordered_set>
using namespace std;

// struct 
struct StudentRecord {
	string name;
	int id, grade;
};
// creates custom hash that can store studentrecord structs
namespace std{
	template<>
	struct hash<StudentRecord>{
		size_t operator()(const StudentRecord& recordInput) const{
			size_t newHash =  hash<int>()(recordInput.id);
			return newHash;
		}
	};
}
// operator overloading == for use in custom hash
bool operator==(const StudentRecord& lhs, const StudentRecord& rhs){
    if (lhs.name == rhs.name){
    	if (lhs.id == rhs.id){
    		if (lhs.grade == rhs.grade){
    			return true;
    		}
    	}else {
    		return false;
    	}
    }else {
    	return false;
    }
    return false;
}

/*
	finds all students with the same name as the query
	arguments: takes the a copy of the hash table, the name to query
	returns: true or false on whether the name was found
*/
bool STRfind(unordered_set<StudentRecord> table, string query){
	bool found = false;
	// iterate through all items in hash table
	for (auto itr = table.begin(); itr != table.end(); ++itr) {
		// if the name is same as query then print it
		if ((*itr).name == query){
			cout<<"Name: "<<(*itr).name
			<< ", ID: " << (*itr).id
			<< ", Grade: " << (*itr).grade << endl;
			found = true;
		}
	}
	return found;
}

/*
	Searches hash for students that match either the grade or ID query
	Arguments: a copy of the hash, the type of query: grade or id, the 
	query as an integer, true or false on whether to print the query
	Returns: true or false on whether the query was found
*/
bool INTfind(unordered_set<StudentRecord> table, 
		  string queryType, int query, bool print){
	bool found = false;
	// if the query type is grade
	if (queryType == "grade"){
		for (auto itr = table.begin(); itr != table.end(); ++itr) {
			// iterate through all items ad print all students that match the grade
    		if ((*itr).grade == query){
				cout<<"Name: "<<(*itr).name
				<< ", ID: " << (*itr).id
				<< ", Grade: " << (*itr).grade << endl;
    			found = true;
    		}
		}
	}else if (queryType == "id"){
		// print out only ONE name if print is true
		for (auto itr = table.begin(); itr != table.end(); ++itr) {
			if ((*itr).id == query && print == true){
				cout<<"Name: "<<(*itr).name
				<< ", ID: " << (*itr).id
				<< ", Grade: " << (*itr).grade << endl;
				return true; // only prints the first one
			}
			// if print is false, then just return true or false
			else if ((*itr).id == query && print == false){
				return true;
			}
		}
	}
	return found;
}

/*
	Removes student of the has
	Arguments: hash passed in by reference, the ID to remove
	Returns: void
*/
void remove(unordered_set<StudentRecord>& table, int ID){
	// iterate until ID is found and then erase from hash
	for (auto itr = table.begin(); itr != table.end(); ++itr) {
		if ((*itr).id == ID){
			table.erase((*itr));
			return;
		}
	}
}

int main() {
	// declare variables to take user inputs
	StudentRecord userInput;
	string command;
	int id, grade;
	string strInput;

	// initialize custom Hash
	unordered_set <StudentRecord> table;

	while (true){
		cin >> command;
		if (command == "I" || command == "i"){
			// insert into hash
			cin >> userInput.name; //read in name
			cin >> userInput.id; //read in ID
			cin >> userInput.grade; //read in grade
			// ERROR: don't allow duplicate insertions
			if (INTfind(table, "id", userInput.id, false) == 1){
				// if found
				cout<<"Error: Cannot insert duplicate ID"<<endl;
			}else{
				// if not already in table then insert
				table.insert(userInput);
			}		
		}else if (command == "R" || command == "r"){
			// remove from hash
			cin >> id;
			
			if (INTfind(table, "id", userInput.id, false) == 0){
				// if found
				cout<<"Error: Cannot remove non-existent ID"<<endl;
			}else{
				remove(table,id);
			}
		}else if (command == "Q" || command == "q"){
			// query for id
			cin >> strInput; //type of query
			if (strInput == "i" || strInput == "I"){
				// search by ID
				// search for ID and only print ONE
				cin >> id; //read in ID
				if (INTfind(table, "id", id, true) == 0){
					cout << "Error: No matches found" << endl;
				}

			}else if (strInput == "n" || strInput == "N"){
				// search by NAME
				// search for ID and print all
				cin >> strInput; //read in the Name
				if (STRfind(table, strInput) == 0){
					cout << "Error: No matches found" << endl;
				}
			}else if (strInput == "g" || strInput == "G"){
				// search by GRADE
				cin >> grade; //read in the grade
				if (INTfind(table, "grade", grade, true) == 0){
					cout << "Error: No matches found" << endl;
				}
			}
		}else if (command == "S" || command == "s"){
			// stop session
			break;
		}
	}
	return 0;
}