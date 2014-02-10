#include "Database.h"
#include <fstream>
#include <stdexcept>
#include <string>

void Database::select(string view_name, string in_table_name, int row_index)
{
	
}

void Database::project(string view_name, string in_table_name, vector<string> attributes)
{

}

void Database::rename(string new_view, string existing_table, vector<string> attributes){
	vector<string> key_attribute_indices;
	const vector<vector<string>> & old_table = get_table(existing_table);

	if(old_table[1].size() != attributes.size())
		throw runtime_error("rename: incorrect number of attributes");

	VIEW_LIST.push_back(vector<vector<string>>(2));
	vector<vector<string>> & view = VIEW_LIST[VIEW_LIST.size() - 1];

	// set title
	view[0].push_back(new_view);

	// set attributes
	view[1] = attributes;

	// copy data
	for(unsigned int i = 2; i < old_table.size(); ++i)
		view.push_back(old_table[i]);

	// set new key names to reflect new attributes
	for(unsigned int i = 1; i < old_table[0].size(); ++i) {
		for(unsigned int j = 0; j < old_table[1].size(); ++j)
			if(old_table[0][i] == old_table[1][j])
				view[0].push_back(attributes[j]);
	}
}

void Database::set_union(string view_name, string table1_name, string table2_name)
{
	
}

void Database::set_difference(string view_name, string table1_name, string table2_name)
{
	
}

void Database::cross_product(string view_name, string table1_name, string table2_name)
{
	
}

void Database::natural_join(string view_name, string table1_name, string table2_name)
{
	
}

void Database::create(string table_name, vector<string> attributes, vector<string> attribute_types, vector<string> keys)
{
	// push back empty table to relation list and get reference to table
	RELATION_LIST.push_back(vector<vector<string> >());
	vector<vector<string> > & relation_table = RELATION_LIST[RELATION_LIST.size() - 1];

	// push back empty row and intialize with title and keys
	relation_table.push_back(vector<string>());
	relation_table[0].push_back(table_name);
	relation_table[0].insert(relation_table[0].end(), keys.begin(), keys.end());

	// add attribute names and attribute types
	relation_table.push_back(attributes);
	relation_table.push_back(attribute_types);

}

void update(string relation_name, vector<string> attribute, vector<string> data, int row_index)
{
	
}

void insert_tuple(string relation_name, vector<string> tuple)
{
	
}

void insert_view(string relation_name, string view_name)
{
	
}

void delete_tuple(string relation_name, string view_name)
{
	
}

void Database::remove(string relation_name, int row_index){
	int relation_index;
	if((relation_index = get_relation_index(relation_name)) == -1)
		throw runtime_error("remove: no such relation");

	vector<vector<string> >& relation_table = RELATION_LIST[relation_index];

	if(unsigned(row_index) >= relation_table.size())
		throw runtime_error("remove: no such row index ( to_string(row_index) )");

	relation_table.erase(relation_table.begin() + row_index);
}


int Database::get_relation_index(string table_name)
{
	int INDEX = -1;

	for (unsigned int i = 0; i<RELATION_LIST.size(); i++)
	if (RELATION_LIST[i][0][0] == table_name)
		INDEX = i;
	return INDEX;
}

int Database::get_view_index(string table_name)
{
	int INDEX = -1;

	for (unsigned int i = 0; i<VIEW_LIST.size(); i++)
	if (VIEW_LIST[i][0][0] == table_name)
		INDEX = i;
	return INDEX;
}

int Database::get_attribute_index(TableType type, int table_index, string attribute_name)
{
	int COLUMN_INDEX = -1;
	if (type == RELATION)
	{
		for (unsigned int i = 0; i<RELATION_LIST[table_index][1].size(); i++)
		if (attribute_name == RELATION_LIST[table_index][1][i])
			COLUMN_INDEX = i;
	}
	else
	{
		for (unsigned int i = 0; i<VIEW_LIST[table_index][1].size(); i++)
		if (attribute_name == VIEW_LIST[table_index][1][i])
			COLUMN_INDEX = i;
	}
	return COLUMN_INDEX;
}



