#ifndef DATABASE_H_
#define DATABASE_H_

#include <cstdarg>
#include <string>
#include <vector>

using namespace std;

class Database{
private:
	vector<vector<vector<string>>> RELATION_LIST;
	vector<vector<vector<string>>> VIEW_LIST;
	enum TableType {VIEW, RELATION};

public:
	void select(string view_name, string in_table_name, int row_index);
	void project(string view_name, string in_table_name, vector<string> attributes);
	void rename(string view_name, string in_table_name, vector<string> attributes);
	void set_union(string view_name, string table1_name, string table2_name);
	void set_difference(string view_name, string table1_name, string table2_name);
	void cross_product(string view_name, string table1_name, string table2_name);
	void natural_join(string view_name, string table1_name, string table2_name);

	void create(string table_name, vector<string> attributes, vector<string> attribute_types, vector<string> keys);
	void update(string relation_name, vector<string> attribute, vector<string> data, int row_index);
	void insert_tuple(string relation_name, vector<string> tuple);
	void insert_view(string relation_name, string view_name);
	void delete_tuple(string relation_name, string view_name);
	void remove(string table_name, int row_index);

	int get_relation_index(string table_name);
	int get_view_index(string table_name);
	int get_attribute_index(TableType type, int table_index, string attribute_name);
};

#endif
