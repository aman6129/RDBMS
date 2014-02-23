#include "Database.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void Database::selection(string viewName, string tableName, int rowIndex)
{
	bool viewNameDupe = true;
	for( int i=0; i<viewSet.size(); i++)
		if(viewSet[i][0][0] == viewName){
			viewNameDupe = false;
		}
	bool isInView = false;
	bool isInRelation = false;
	for( int i=0; i<viewSet.size(); i++)
		if(viewSet[i][0][0] == tableName){
			isInView = true;
		}
	for( int i=0; i<relationSet.size(); i++)
		if(relationSet[i][0][0] == tableName){
			isInRelation = true;
		}

	vector<vector<string> > viewTemp;
	if(isInRelation){
		if(viewNameDupe){
			int relationIndex = getRelationIndex(tableName);
			vector<string> tempVec;
			tempVec.push_back(viewName);
			for( int i=1; i<relationSet[relationIndex][0].size(); i++)
				tempVec.push_back(relationSet[relationIndex][0][i]);

			viewTemp.push_back(tempVec);
			viewTemp.push_back(relationSet[relationIndex][1]);
			viewTemp.push_back(relationSet[relationIndex][2]);
			viewTemp.push_back(relationSet[relationIndex][rowIndex]);

			viewSet.push_back(viewTemp);
		}
		else{
			int viewIndex = getViewIndex(viewName);
			int relationIndex = getRelationIndex(tableName);
			viewSet[viewIndex].push_back(relationSet[relationIndex][rowIndex]);
		}
	}
	else{
		if(viewNameDupe){
			int viewIndex = getViewIndex(tableName);
			vector<string> tempVec;
			tempVec.push_back(viewName);
			for( int i=1; i<viewSet[viewIndex][0].size(); i++)
				tempVec.push_back(viewSet[viewIndex][0][i]);

			viewTemp.push_back(tempVec);
			viewTemp.push_back(viewSet[viewIndex][1]);
			viewTemp.push_back(viewSet[viewIndex][2]);
			viewTemp.push_back(viewSet[viewIndex][rowIndex]);

			viewSet.push_back(viewTemp);
		}
		else{
			int viewIndex = getViewIndex(viewName);
			int vIndex = getViewIndex(tableName);
			viewSet[viewIndex].push_back(viewSet[vIndex][rowIndex]);
		}
	}
}

void Database::projection(string viewName, string tableName, vector<string> attributes)
{
	int index = getRelationIndex(tableName);
	vector<vector<string> > viewTemp;
	vector<int> columns;

	if(index == -1){
		index = getViewIndex(tableName);
		bool checkAttr = false;
		for( int i = 0; i<attributes.size(); i++){
			for( int j=0; j<viewSet[index][1].size(); j++)
				if(attributes[i] == viewSet[index][1][j]){
					columns.push_back(i);
					checkAttr = true;
				}
		}
		vector<string> tVec;
		tVec.push_back(viewName);
		viewTemp.push_back(tVec);

		for( int i=1; i< viewSet[index].size(); i++){
			vector<string> tempVec;
			for( int j=0; j<columns.size(); j++)
				tempVec.push_back(viewSet[index][i][columns[j]]);
			viewTemp.push_back(tempVec);
		}
		viewSet.push_back(viewTemp);
		
	}
	else{
		bool checkAttr = false;
		for( int i = 0; i<attributes.size(); i++){
			for( int j=0; j<relationSet[index][1].size(); j++)
				if(attributes[i] == relationSet[index][1][j]){
					columns.push_back(i);
					checkAttr = true;
				}
		}
		vector<string> tVec;
		tVec.push_back(viewName);
		viewTemp.push_back(tVec);

		for( int i=1; i< relationSet[index].size(); i++){
			vector<string> tempVec;
			for( int j=0; j<columns.size(); j++)
				tempVec.push_back(relationSet[index][i][columns[j]]);
			viewTemp.push_back(tempVec);
		}
		viewSet.push_back(viewTemp);
	}
}

void Database::rename(string new_view, string existing_table, vector<string> attributes){
	vector<string> key_attribute_indices;
	const vector<vector<string> >& old_table = getTable(existing_table);

	viewSet.push_back(vector<vector<string> >(2));
	vector<vector<string> >& view = viewSet[viewSet.size() - 1];
	view[0].push_back(new_view);
	view[1] = attributes;
	for( int i = 2; i < old_table.size(); ++i)
		view.push_back(old_table[i]);

	for( int i = 1; i < old_table[0].size(); ++i) {
		for( int j = 0; j < old_table[1].size(); ++j)
			if(old_table[0][i] == old_table[1][j])
				view[0].push_back(attributes[j]);
	}
}

void Database::setUnion(string viewName, string table1, string table2Name)
{
	int table1Id = getRelationIndex(table1);
	int table2NameId = getRelationIndex(table2Name);
	
	viewSet.push_back(vector<vector<string> >());
	vector<vector<string> > & view_table = viewSet[viewSet.size() - 1];
	view_table.push_back(vector<string>());
	view_table[0].push_back(viewName);
	for (int i = 0; i < relationSet[table1Id].size(); i++)
	{
		view_table.push_back(relationSet[table1Id][i]);
	}
	for (int index2 = 2; index2 < relationSet[table2NameId].size(); index2++)
	{
		bool rowExists = false;
		for (int indexView = 0; indexView < viewSet[viewSet.size() - 1].size(); indexView++)
		{
			if(compareRows(relationSet[table2NameId][index2], viewSet[viewSet.size() - 1][indexView]))
			{
				rowExists = true;
			}
		}
		if (!rowExists)
		{
			view_table.push_back(relationSet[table2NameId][index2]);
		}
	}
}

void Database::setDifference(string viewName, string table1, string table2Name)
{
	int table1Id = getRelationIndex(table1);
	int table2NameId = getRelationIndex(table2Name);

	viewSet.push_back(vector<vector<string> >());
	vector<vector<string> > & view_table = viewSet[viewSet.size() - 1];
	view_table.push_back(vector<string>());
	view_table[0].push_back(viewName);

	for (int i = 0; i < relationSet[table1Id].size(); i++)
	{
		view_table.push_back(relationSet[table1Id][i]);
	}
	for (int index2 = 2; index2 < relationSet[table2NameId].size(); index2++)
	{
		for (int indexView = viewSet[viewSet.size() - 1].size() - 1; indexView >= 0; indexView--)
		{
			if(compareRows(relationSet[table2NameId][index2], viewSet[viewSet.size() - 1][indexView]))
			{
				viewSet[viewSet.size() - 1].erase(viewSet[viewSet.size() - 1].begin() + indexView);
			}
		}
	}
}

void Database::crossProduct(string viewName, string table1, string table2Name)
{
	int table1Id = getRelationIndex(table1);
	int table2NameId = getRelationIndex(table2Name);

	viewSet.push_back(vector<vector<string> >());
	vector<vector<string> > & view_table = viewSet[viewSet.size() - 1];
	view_table.push_back(vector<string>());
	view_table[0].push_back(viewName);
	for (int i = 0; i < 3; i++)
	{
		view_table.push_back(relationSet[table1Id][i]);
	}
	for (int i = 3; i < relationSet[table1Id].size(); i++)
	{
		for (int j = 3; j < relationSet[table2NameId].size(); j++)
		{
			view_table.push_back(appendVectors(relationSet[table1Id][i], relationSet[table2NameId][j]));
		}
	}
} 

void Database::naturalJoin(string viewName, string table1, string table2Name, string condition)
{
	int table1Id = getRelationIndex(table1);
	int table2NameId = getRelationIndex(table2Name);
	int column1 = 0;
	int column2 = 0;

	for (int i = 0; i < relationSet[table1Id][1].size(); i++)
	{
		if (condition == relationSet[table1Id][1][i])
		{
			column1 = i;
		}
	}
	for (int i = 0; i < relationSet[table2NameId][1].size(); i++)
	{
		if (condition == relationSet[table2NameId][1][i])
		{
			column2 = i;
		}
	}

	viewSet.push_back(vector<vector<string> >());
	vector<vector<string> > & view_table = viewSet[viewSet.size() - 1];
	view_table.push_back(vector<string>());
	view_table[0].push_back(viewName);
	for (int i = 0; i < 3; i++)
	{
		view_table.push_back(relationSet[table1Id][i]);
	}
	for (int i = 3; i < relationSet[table1Id].size(); i++)
	{
		for (int j = 3; j < relationSet[table2NameId].size(); j++)
		{
			if (relationSet[table1Id][i][column1] == relationSet[table2NameId][j][column2])
			{
				if (relationSet[table1Id][i] != relationSet[table2NameId][j])
				{
					view_table.push_back(appendVectors(relationSet[table1Id][i], relationSet[table2NameId][j]));
				}
			}
		}
	}
}

void Database::show(string tableName){
	int index;

	if((index = getRelationIndex(tableName)) != -1) 
		printTable(relationSet[index][0][0]);
	else if((index = getViewIndex(tableName)) != -1)
		printTable(viewSet[index][0][0]);
	else
		throw runtime_error("show: no such table");
}

void Database::create(string tableName, vector<string> attributes, vector<string> attribute_types, vector<string> keys)
{
	relationSet.push_back(vector<vector<string> >());
	vector<vector<string> > & relation_table = relationSet[relationSet.size() - 1];

	relation_table.push_back(vector<string>());
	relation_table[0].push_back(tableName);
	relation_table[0].insert(relation_table[0].end(), keys.begin(), keys.end());

	relation_table.push_back(attributes);
	relation_table.push_back(attribute_types);

}

void Database::update(string relationName, vector<string>attribute, vector<string> data, int rowIndex){
	int relationIndex = -1;
	vector<int> attribute_indices;

	relationIndex = getRelationIndex(relationName);

	for( int i = 0; i < attribute.size(); ++i) {
		int attribute_index = -1;
		attribute_index = getAttributeIndex(RELATION, relationIndex, attribute[i]);
		attribute_indices.push_back(attribute_index);
	}

	for( int i = 0; i < attribute_indices.size(); ++i)
		relationSet[relationIndex][rowIndex][attribute_indices[i]] = data[i];
}

void Database::exit(){
	relationSet.clear();						
	viewSet.clear();
}

void Database::close(string tableName){
	int index;
	if((index = getRelationIndex(tableName)) != -1)
		relationSet.erase(relationSet.begin() + index);
	else if((index = getViewIndex(tableName)) != -1)
		viewSet.erase(viewSet.begin() + index);
	else
		throw runtime_error("close: no such table");
}

void Database::write(string tableName){
	int index;

	if((index = getRelationIndex(tableName)) != -1)
		write_table(tableName, relationSet[index]);
	else if((index = getViewIndex(tableName)) != -1)
		write_table(tableName, viewSet[index]);
	else
		throw runtime_error("write: no such table");
}

void Database::write_table(string tableName, const vector<vector<string> >& TABLE){
	ofstream OUTPUT_FILE;
	OUTPUT_FILE.open(tableName + ".db");
	OUTPUT_FILE << "CREATE TABLE " << tableName << " (";
	for(unsigned int i=0; i<TABLE[1].size(); i++){
		if(i != (TABLE[1].size() - 1))
			OUTPUT_FILE << TABLE[1][i] << " " << TABLE[2][i] << ", ";
		else
			OUTPUT_FILE << TABLE[1][i] << " " << TABLE[2][i] << ") ";
	}
	OUTPUT_FILE << "PRIMARY KEY (";
	for(unsigned int i=1; i<TABLE[0].size(); i++){
		if(i != (TABLE[0].size() - 1))
			OUTPUT_FILE << TABLE[0][i] << ", ";
		else
			OUTPUT_FILE << TABLE[0][i] << ");" << endl;
	}
	for(unsigned int i=3; i<TABLE.size(); i++){
		OUTPUT_FILE << "INSERT INTO " << tableName << " VALUES FROM (";
		for(unsigned int j=0; j<TABLE[i].size(); j++){
			if(j != (TABLE[i].size() - 1)){
				if(TABLE[2][j] == "integer")
					OUTPUT_FILE << TABLE[i][j] << ", ";
				else
					OUTPUT_FILE << "\"" << TABLE[i][j] << "\", ";
			}
			else{
				if(TABLE[2][j] == "integer")
					OUTPUT_FILE << TABLE[i][j] << ");" << endl;
				else
					OUTPUT_FILE << "\"" << TABLE[i][j] << "\");" << endl;
			}
		}
	}
	OUTPUT_FILE.close();
}


void Database::insertTupleInto(string relationName, vector<string> tuple) 
{
	int relationIndex = getRelationIndex(relationName);
	if(relationIndex == -1) //making sure the relation exists in the db
		runtime_error("insertTupleInto: no such relaiton");

	vector<vector<string> >& relation_table = relationSet[relationIndex];

	if(tuple.size() > relation_table[0].size()) //making sure the tuple fits in the table
		throw runtime_error("inser_tuple: tuple size too large!");

	// relation_table.push_back(tuple);

	relation_table.push_back(tuple);
}

void Database::insertViewInto(string relationName, string viewName) 
{
	int relationIndex = getRelationIndex(relationName);
	int viewIndex = getViewIndex(viewName);

	if(relationIndex == -1)
		runtime_error("insertViewInto: no such relation");
	if(viewIndex == -1)
		runtime_error("insertViewInto: no such view");

	vector<vector<string> >& relation_table = relationSet[relationIndex];

	//need to put a check to make sure the view fits into the relation

	for(int vIndex1 = 0; vIndex1 < viewSet[viewIndex].size(); ++vIndex1)
	{
		relation_table.push_back(viewSet[viewIndex][vIndex1]);
	}
}

void Database::remove(string relationName, int rowIndex){
	int relationIndex;
	if((relationIndex = getRelationIndex(relationName)) == -1)
		throw runtime_error("remove: no such relation");

	vector<vector<string> >& relation_table = relationSet[relationIndex];

	if((rowIndex) >= relation_table.size())
		throw runtime_error("remove: no such row index ( to_string(rowIndex))");

	relation_table.erase(relation_table.begin() + rowIndex);
}

int Database::getRelationIndex(string tableName)
{
	int index = -1;

	for ( int i = 0; i<relationSet.size(); i++)
	if (relationSet[i][0][0] == tableName)
		index = i;
	return index;
}

int Database::getViewIndex(string tableName)
{
	int index = -1;

	for ( int i = 0; i<viewSet.size(); i++)
	if (viewSet[i][0][0] == tableName)
		index = i;
	return index;
}

int Database::getAttributeIndex(TableType type, int table_index, string attribute_name)
{
	int COLUMN_INDEX = -1;
	if (type == RELATION)
	{
		for ( int i = 0; i<relationSet[table_index][1].size(); i++)
		if (attribute_name == relationSet[table_index][1][i])
			COLUMN_INDEX = i;
	}
	else
	{
		for ( int i = 0; i<viewSet[table_index][1].size(); i++)
		if (attribute_name == viewSet[table_index][1][i])
			COLUMN_INDEX = i;
	}
	return COLUMN_INDEX;
}

bool Database::compareRows(vector<string> row1, vector<string> row2)
{
	bool equalRows = true;
	if (row1.size() == 0 && row2.size() != 0)
	{
		equalRows = false;
	}
	for(int i = 0; i < row1.size() && i < row2.size(); i++)
	{
		if (row1[i] != row2[i])
		{
			equalRows = false;
		}
	}

	return equalRows;
}

void Database::updateViewName(string new_name, string old_name){
	int VIEW_INDEX = getViewIndex(old_name);
	if(VIEW_INDEX == -1)
		throw runtime_error("Change View Name: No such table exists");
	else
		viewSet[VIEW_INDEX][0][0] = new_name;
}

void Database::printTable(string tableName)
{
	int tableID = getRelationIndex(tableName);
	for (int rows = 0; rows < relationSet[tableID].size(); rows++)
	{
		for (int columns = 0; columns < relationSet[tableID][rows].size(); columns++)
		{
			if (rows != 2)
			{
				cout << relationSet[tableID][rows][columns] << '\t';
			}
		}
		if (rows != 2)
		{
			cout << endl;
		}
	}
}

void Database::printView(string viewName)
{
	int viewID = getViewIndex(viewName);
	for (int rows = 0; rows < viewSet[viewID].size(); rows++)
	{
		for (int columns = 0; columns < viewSet[viewID][rows].size(); columns++)
		{
			cout << viewSet[viewID][rows][columns] << '\t';
		}
		cout << endl;
	}
}

const vector<vector<string> >& Database::getTable(string tableName){
	int index;

	if((index = getRelationIndex(tableName)) != -1)
		return relationSet[index];
	else if((index = getViewIndex(tableName)) != -1)
		return viewSet[index];
	else
		throw runtime_error("getTable: no such table");
}

vector<string> Database::appendVectors(vector<string> vec1, vector<string> vec2)
{
	for (int i = 0; i< vec2.size(); i++)
	{
		vec1.push_back(vec2[i]);
	}
	return vec1;
}