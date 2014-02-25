#include <iostream>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

class GameDBapp
{
private:
	//put private variables
	string relation_name;
	vector<string> input;


	//put private functions here
	void show_main_basic();
	void show_main_advanced();
	void wait_for_input();

	void add(); // add
	void remove(); // remove
	void combine(); // combine based on relational algebra
	void display(); // display something
	void new_relation(); // new table
	void save(); // save the db to a file
	void open(); //open a db file
	void close(); //close and save the db

	//open, close, write, exit, show
	//create table, insert into, update, delete
	//select, project, +, - , *

public:

	void start_app();


};