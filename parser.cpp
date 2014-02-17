#include "parser.h"

void parser::parse_this(string command, string input)
{
	if(command.compare("CREATE TABLE") == 0)
		create_table(input);
	else if(command.compare("INSERT INTO") == 0)
		insert_into(input);
}

void parser::strip_information(string input)
{
	string command = function_called(input);

	cout << "Command entered = " << command << endl;
	if(command.compare("EXIT") == 0)
		cout << "Exiting RDBMS language\n";
	else if(command.compare("<-") == 0)
	{
		//handle queries here
	}

	else
	{
		//handle commands here
		parse_this(command, input);
	}
}

string parser::function_called(string input)
{	
	// looking at the index 0 and checking what command is at that index, returning the command found
	// we are assuming good input

	if(input.find("CREATE TABLE") == 0)
		return "CREATE TABLE";
	else if(input.find("INSERT INTO") == 0)
		return "INSERT INTO";
	else if(input.find("SHOW") == 0)
		return "SHOW";
	else if(input.find("WRITE") == 0)
		return "WRITE";
	else if(input.find("CLOSE") == 0)
		return "CLOSE";
	else if(input.find("EXIT") == 0)
		return "EXIT";
	else if(input.find("<-"))
		return "<-";
	else
		return "COMMAND NOT RECOGNIZED!\n";
}

void parser::create_table(string input)
{
	int index = 13; //index of the input we are starting at.
	string table_name;
	string attribute_name;
	string attribute_type;
	string key_name;

	while(input[index] != ' ') //getting the name of the new table
	{
		table_name += input[index];
		++index;
	}
	cout << "\tTable name: " << table_name << endl;
	++index; //incrementing the index to the next value to take in

	while(index < input.size() && input[index] != ')')
	{
		/*
			all the code here will read in the attribute names and types
			there should be no spaces after each comma otherwise the parse is errorific
			I figured this was allowed because we are just hardcoding a parser, as in not something that can handle
			multiple types of input? This assumption will be heald throughout the parser.
		*/

		attribute_name = "";
		attribute_type = "";
		++index; 

		while(input[index] != ' ') //this loop will read the name of the attribute
		{
			attribute_name += input[index];
			++index;
		}
		++index;

		while(input[index] != ',' && input[index] != ')') 
		//this loop reads teh type of the attribute. by reading through the input until either comma or parenthisis is encountered
		{
			attribute_type += input[index];
			++index;
		}

		cout << "\n\tAttribute name: " << attribute_name
				<< "\tAttribute type: " << attribute_type;

		attribute_names.push_back(attribute_name);
		attribute_types.push_back(attribute_type);
	}

	index += 14;
	if(index >= input.size())
		cerr << "Looks like you didn't specify a primary key stoopid!\n";
	while(index < input.size() && input[index] != ')') //this part handles the primary keys, in the same fashion as the att. names
	{
		key_name = "";
		++index;
		while(input[index] != ',' && input[index] != ')')
		{
			key_name += input[index];
			++index;
		}

		cout << "\n\tKey Name: " << key_name;
		keys.push_back(key_name);
	}

	/*Addfunctionalotyhere*/

	cout << endl;
}

void parser::insert_into(string input)
{
	//INSERT INTO animals VALUES FROM ("Joe", "cat", 4);
	int index = 12; //start at the first letter of the table name;
	string table_name;
	string val_to_insert;

	while(input[index] != ' ')
	{
		table_name += input[index];
		++index;
	}

	cout << "Table name: " << table_name << endl; //
	index += 13; //will go to the first letter of the tuple sent
	if(input[index] == 'R') //If we are inserting from a relation
	{
		cout << "Inserting into RELATION\n";
	}
	else //if we are inserting from a tuple
	{
		while(index < input.size() && input[index] != ')')
		{
			val_to_insert = "";
			++index;
			while(input[index] != ')' && input[index] != ',')
			{
				val_to_insert += input[index];
				++index;
			}

			cout << "\n\tVal to insert : " << val_to_insert;
			to_insert.push_back(val_to_insert);
		}

		cout << endl;
	}

}
