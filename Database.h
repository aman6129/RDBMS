#ifndef DATABASE_H_
#define DATABASE_H_
#include <string>
#include <vector>

using namespace std;

class Database{
private:
	vector<vector<vector<string> > > relationSet;
	vector<vector<vector<string> > > viewSet;
	enum TableType {VIEW, RELATION};
	void write_table(string tableName, const vector<vector<string> >& TABLE);

public:
	void selection(string viewName, string tableName, int rowIndex); 
	void projection(string viewName, string tableName, vector<string> attributes); 
	void rename(string viewName, string tableName, vector<string> attributes); 
	void setUnion(string viewName, string table1, string table2Name);
	void setDifference(string viewName, string table1, string table2Name);
	void crossProduct(string viewName, string table1, string table2Name);
	void naturalJoin(string viewName, string table1, string table2Name, string condition);
	void create(string tableName, vector<string> attributes, vector<string> attribute_types, vector<string> keys); 
	void update(string relationName, vector<string> attribute, vector<string> data, int rowIndex); 
	void insertTupleInto(string relationName, vector<string> tuple);
	void insertViewInto(string relationName, string viewName);
	void remove(string tableName, int rowIndex); 
	void show(string tableName);
	void write(string tableName);
	void close(string tableName);
	void exit();


	//Helper Functions
	static bool compareRows(vector<string> row1, vector<string> row2);
	const vector<vector<string> >& getTable(string tableName);
	void printTable(string tableName);
	void printView(string viewName);
	void updateViewName(string new_name, string old_name);
	vector<string> appendVectors(vector<string> vec1, vector<string> vec2);
	int getRelationIndex(string tableName); 
	int getViewIndex(string tableName); 
	int getAttributeIndex(TableType type, int table_index, string attribute_name); 

};

#endif