#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class parser
{


private:
	
	//private data types
	vector<string> attribute_names;
	vector<string> attribute_types;
	vector<string> keys;
	vector<string> to_insert; //values that will be inserted from the INSERT INTO function
	
	//helper functions
	string function_called(string); //returns the string value of the function called
	void create_table(string); //gets teh appropriate information for creat_table
	void insert_into(string); //gets teh appropriate information for insert_into

public:

	//Main functions that handle parsing
	void parse_this(string, string);
	void strip_information(string);


};
