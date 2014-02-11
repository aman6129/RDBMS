#include "Database.h"
#include <string>

void Database::select(string view_name, string in_table_name, int row_index)
{
	bool = true;
	// check to see if view_name already exists
	for(unsigned int i=0; i<VIEW_LIST.size(); i++)
		if(VIEW_LIST[i][0][0] == view_name)-{
			VIEW_CHECK = false;
		}
	
	bool IN_VIEW = false;
	bool IN_RELATION = false;
	for(unsigned int i=0; i<VIEW_LIST.size(); i++)
		if(VIEW_LIST[i][0][0] == in_table_name){
			IN_VIEW = true;
		}
	for(unsigned int i=0; i<RELATION_LIST.size(); i++)
		if(RELATION_LIST[i][0][0] == in_table_name){
			IN_RELATION = true;
		}

	vector<vector<string> > TEMP_VIEW_TABLE;
	if(IN_RELATION){
		if(VIEW_CHECK){
			int RELATION_INDEX = get_relation_index(in_table_name);
			vector<string> temp_vec;
			temp_vec.push_back(view_name);
			for(unsigned int i=1; i<RELATION_LIST[RELATION_INDEX][0].size(); i++)
				temp_vec.push_back(RELATION_LIST[RELATION_INDEX][0][i]);
			TEMP_VIEW_TABLE.push_back(temp_vec);
			TEMP_VIEW_TABLE.push_back(RELATION_LIST[RELATION_INDEX][1]);
			TEMP_VIEW_TABLE.push_back(RELATION_LIST[RELATION_INDEX][2]);
			TEMP_VIEW_TABLE.push_back(RELATION_LIST[RELATION_INDEX][row_index]);

			VIEW_LIST.push_back(TEMP_VIEW_TABLE);
		}
		else{
			int VIEW_INDEX = get_view_index(view_name);
			int RELATION_INDEX = get_relation_index(in_table_name);
			VIEW_LIST[VIEW_INDEX].push_back(RELATION_LIST[RELATION_INDEX][row_index]);
		}
	}
	else{
		if(VIEW_CHECK){
			int VIEW_INDEX = get_view_index(in_table_name);
			vector<string> temp_vec;
			temp_vec.push_back(view_name);
			for(unsigned int i=1; i<VIEW_LIST[VIEW_INDEX][0].size(); i++)
				temp_vec.push_back(VIEW_LIST[VIEW_INDEX][0][i]);

			TEMP_VIEW_TABLE.push_back(temp_vec);
			TEMP_VIEW_TABLE.push_back(VIEW_LIST[VIEW_INDEX][1]);
			TEMP_VIEW_TABLE.push_back(VIEW_LIST[VIEW_INDEX][2]);
			TEMP_VIEW_TABLE.push_back(VIEW_LIST[VIEW_INDEX][row_index]);

			VIEW_LIST.push_back(TEMP_VIEW_TABLE);
		}
		else{
			int VIEW_INDEX = get_view_index(view_name);
			int V_INDEX = get_view_index(in_table_name);
			VIEW_LIST[VIEW_INDEX].push_back(VIEW_LIST[V_INDEX][row_index]);
		}
	}
}
void Database::project(string view_name, string in_table_name, vector<string> attributes)
{
	if(view_name == in_table_name)
		throw runtime_error("Error: both names equal");
	int INDEX = get_relation_index(in_table_name);
	vector<vector<string> > TEMP_VIEW_TABLE;
	vector<int> columns;

	if(INDEX == -1){
		INDEX = get_view_index(in_table_name);
		if(INDEX == -1)
			throw runtime_error("Error: view does not exists");
		else{
			bool ATTRIBUTE_CHECK = false;
			for(unsigned int i = 0; i<attributes.size(); i++){
				for(unsigned int j=0; j<VIEW_LIST[INDEX][1].size(); j++)
					if(attributes[i] == VIEW_LIST[INDEX][1][j]){
						columns.push_back(i);
						ATTRIBUTE_CHECK = true;
					}
				if(ATTRIBUTE_CHECK == false) 
					throw runtime_error("Project: no such attribute exists (" + attributes[i] + ")");
			}
			vector<string> t_vec;
			t_vec.push_back(view_name);
			TEMP_VIEW_TABLE.push_back(t_vec);

			for(unsigned int i=1; i< VIEW_LIST[INDEX].size(); i++){
				vector<string> temp_vec;
				for(unsigned int j=0; j<columns.size(); j++)
					temp_vec.push_back(VIEW_LIST[INDEX][i][columns[j]]);
				TEMP_VIEW_TABLE.push_back(temp_vec);
			}
			VIEW_LIST.push_back(TEMP_VIEW_TABLE);
		}
	}
	else{
		bool ATTRIBUTE_CHECK = false;
		for(unsigned int i = 0; i<attributes.size(); i++){
			for(unsigned int j=0; j<RELATION_LIST[INDEX][1].size(); j++)
				if(attributes[i] == RELATION_LIST[INDEX][1][j]){
					columns.push_back(i);
					ATTRIBUTE_CHECK = true;
				}
			if(ATTRIBUTE_CHECK == false)
				throw runtime_error("Project: no such attribute exists (" + attributes[i] + ")");
		}
		vector<string> t_vec;
		t_vec.push_back(view_name);
		TEMP_VIEW_TABLE.push_back(t_vec);

		for(unsigned int i=1; i< RELATION_LIST[INDEX].size(); i++){
			vector<string> temp_vec;
			for(unsigned int j=0; j<columns.size(); j++)
				temp_vec.push_back(RELATION_LIST[INDEX][i][columns[j]]);
			TEMP_VIEW_TABLE.push_back(temp_vec);
		}
		VIEW_LIST.push_back(TEMP_VIEW_TABLE);
	}
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



