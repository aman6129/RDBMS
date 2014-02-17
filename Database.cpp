#include "Database.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// Database Functions
void Database::create(string relationName, vector<string> attributes, vector<string> attributeTypes, vector<string> keys) // GOOD TO GO
{
	Table* relationTable;
	vector<string> titleRow;

	titleRow.push_back(relationName);

	relationTable->addRow(titleRow);
	relationTable->addRow(attributes);
	relationTable->setAttributeTypes(attributeTypes);
	relationTable->setKeys(keys);
}

void Database::update(string relationName, int rowIndex, vector<string> attributes, vector<string> data) // GOOD TO GO
{
	int relationIndex = getTableIndex(relationName);
	vector<int> attributeIndices;

	for (int i = 0; i < attributes.size(); i++)
	{
		int attributeIndex = getAttributeIndex(RELATION, relationIndex, attributes[i]);
		attributeIndices.push_back(attributeIndex);
	}
	if (attributeIndices.size() == data.size())
	{
		for (int i = 0; i < attributeIndices.size(); i++)
		{
			_relations[relationIndex]->setCell(rowIndex, attributeIndices[i], data[i]);
		}
	}
	else
	{
		throw runtime_error("update: mismatch between attributes and data lengths");
	}
}

void Database::insertTuple(string relationName, vector<string> tuple) // GOOD TO GO
{
	int relationIndex = getTableIndex(relationName);
	Table* relationTable = _relations[relationIndex];

	if (tuple.size() == relationTable->getRow(1).size())
	{
		relationTable->addRow(tuple);
	}
	else
	{
		throw runtime_error("insertTuple: tuple size doesn't match number of attributes");
	}
}

void Database::insertView(string relationName, string viewName) // GOOD TO GO
{
	int relationIndex = getTableIndex(relationName);
	int viewIndex = getTableIndex(viewName);
	Table* relationTable = _relations[relationIndex];
	Table* viewTable = _views[viewIndex];

	if (_views[viewIndex]->getRow(1).size() == _relations[relationIndex]->getRow(1).size())
	{
		for (int i = 0; i < _views[viewIndex]->getNumRows(); i++)
		{
			relationTable->addRow(_views[viewIndex]->getRow(i));
		}
	}
	else
	{
		throw runtime_error("insertView: view row size doesn't match relation row size");
	}
}

void Database::removeRow(string relationName, int rowIndex) // GOOD TO GO
{
	int relationIndex = getTableIndex(relationName);
	Table* relationTable = _relations[relationIndex];

	if (rowIndex >= 0 && rowIndex < relationTable->getNumRows())
	{
		relationTable->removeRow(rowIndex);
	}
	else
	{
		throw runtime_error("removeRow: no such row index");
	}
}

// Relational Algebra Functions
void Database::selection(string viewName, string tableName, int rowIndex) // GOOD TO GO
{
	bool viewNameDupe = true;
	for (int i = 0; i < _views.size(); i++)
	{
		if (_views.at(i)->getCell(0, 0) == viewName)
		{
			viewNameDupe = false;
		}
	}

	bool isInView = false;
	bool isInRelation = false;
	for (int i = 0; i < _views.size(); i++)
	{
		if (_views.at(i)->getCell(0, 0) == tableName)
		{
			isInView = true;
		}
	}
	for (int i = 0; i < _relations.size(); i++)
	{
		if (_relations.at(i)->getCell(0, 0) == tableName)
		{
			isInRelation = true;
		}
	}

	Table* viewTemp;
	if (isInRelation)
	{
		if (viewNameDupe)
		{
			int relationIndex = getTableIndex(tableName);
			vector<string> tempVec;
			tempVec.push_back(viewName);

			for (int i = 1; i < _relations.at(relationIndex)->getRow(0).size(); i++)
			{
				tempVec.push_back(_relations.at(relationIndex)->getCell(0, i));
			}

			viewTemp->addRow(tempVec);
			viewTemp->addRow(_relations.at(relationIndex)->getRow(1));
			viewTemp->addRow(_relations.at(relationIndex)->getRow(2));
			viewTemp->addRow(_relations.at(relationIndex)->getRow(rowIndex));
			_views.push_back(viewTemp);
		}
		else
		{
			int viewIndex = getTableIndex(viewName);
			int relationIndex = getTableIndex(tableName);
			_views.at(viewIndex)->addRow(_relations.at(relationIndex)->getRow(rowIndex));
		}
	}
	else
	{
		if (viewNameDupe)
		{
			int viewIndex = getTableIndex(tableName);
			vector<string> tempVec;
			tempVec.push_back(viewName);

			for (int i = 1; i < _views.at(viewIndex)->getRow(0).size(); i++)
			{
				tempVec.push_back(_views.at(viewIndex)->getCell(0, i));
			}

			viewTemp->addRow(tempVec);
			viewTemp->addRow(_views.at(viewIndex)->getRow(1));
			viewTemp->addRow(_views.at(viewIndex)->getRow(2));
			viewTemp->addRow(_views.at(viewIndex)->getRow(rowIndex));
			_views.push_back(viewTemp);
		}
		else
		{
			int viewIndex = getTableIndex(viewName);
			int vIndex = getTableIndex(tableName);
			_views.at(viewIndex)->addRow(_views.at(vIndex)->getRow(rowIndex));
		}
	}
}

void Database::projection(string viewName, string tableName, vector<string> attributes) // GOOD TO GO
{
	int relationIndex = getTableIndex(tableName);
	Table* viewTemp;
	vector<int> columns;

	if (relationIndex == -1)
	{
		relationIndex = getTableIndex(tableName);
		bool checkAttr = false;

		for (int i = 0; i < attributes.size(); i++)
		{
			for (int j = 0; j < _views.at(relationIndex)->getRow(1).size(); j++)
			{
				if (attributes[i] == _views.at(relationIndex)->getCell(1, j))
				{
					columns.push_back(i);
					checkAttr = true;
				}
			}
		}

		vector<string> tVec;
		tVec.push_back(viewName);
		viewTemp->addRow(tVec);

		for (int i = 1; i < _views.at(relationIndex)->getNumRows(); i++)
		{
			vector<string> tempVec;

			for (int j = 0; j < columns.size(); j++)
			{
				tempVec.push_back(_views.at(relationIndex)->getCell(i, columns[j]));
			}

			viewTemp->addRow(tempVec);
		}

		_views.push_back(viewTemp);
	}
	else
	{
		bool checkAttr = false;

		for (int i = 0; i < attributes.size(); i++)
		{
			for (int j = 0; j < _relations.at(relationIndex)->getRow(1).size(); j++)
			{
				if (attributes[i] == _relations.at(relationIndex)->getCell(1, j))
				{
					columns.push_back(i);
					checkAttr = true;
				}
			}
		}

		vector<string> tVec;
		tVec.push_back(viewName);
		viewTemp->addRow(tVec);

		for (int i = 1; i < _relations.at(relationIndex)->getNumRows(); i++)
		{
			vector<string> tempVec;

			for (int j = 0; j < columns.size(); j++)
			{
				tempVec.push_back(_relations.at(relationIndex)->getCell(i, columns[j]));
			}

			viewTemp->addRow(tempVec);
		}

		_views.push_back(viewTemp);
	}
}

void Database::rename(string newView, string existingTable, vector<string> attributes) // GOOD TO GO
{
	Table* oldTable = getTable(existingTable);
	Table* view;
	vector<string> titleRow;

	titleRow.push_back(newView);
	view->addRow(titleRow);
	view->addRow(attributes);

	for (int i = 2; i < oldTable->getNumRows(); i++)
	{
		view->addRow(oldTable->getRow(i));
	}
	for (int i = 1; i < oldTable->getRow(0).size(); i++)
	{
		for (int j = 0; j < oldTable->getRow(1).size(); j++)
		{
			if (oldTable->getCell(0, i) == oldTable->getCell(1, j))
			{
				view->getRow(0).push_back(attributes[j]);
			}
		}
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
			if (compareRows(relationSet[table2NameId][index2], viewSet[viewSet.size() - 1][indexView]))
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
			if (compareRows(relationSet[table2NameId][index2], viewSet[viewSet.size() - 1][indexView]))
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

// Accessor Functions
Table* Database::getTable(string tableName) // GOOD TO GO
{
	int index = getTableIndex(tableName);
	Table* table;

	switch (getTableType(tableName))
	{
	case RELATION:
		table = _relations[index];
		break;
	case VIEW:
		table = _views[index];
		break;
	default:
		throw runtime_error("getTable: no such table exists");
		break;
	}

	return table;
}

int Database::getTableIndex(string tableName) // GOOD TO GO
{
	int index = -1;

	for (int i = 0; i < _relations.size(); i++)
	{
		if (_relations[i]->getCell(0, 0) == tableName)
		{
			index = i;
		}
	}
	for (int i = 0; i < _views.size(); i++)
	{
		if (_views[i]->getCell(0, 0) == tableName)
		{
			index = i;
		}
	}
	if (index == -1)
	{
		throw runtime_error("getTableIndex: no such table exists");
	}

	return index;
}

Database::TableType Database::getTableType(string tableName) // GOOD TO GO
{
	for (int i = 0; i < _relations.size(); i++)
	{
		if (_relations[i]->getCell(0, 0) == tableName)
		{
			return RELATION;
		}
	}
	for (int i = 0; i < _views.size(); i++)
	{
		if (_views[i]->getCell(0, 0) == tableName)
		{
			return VIEW;
		}
	}
	throw runtime_error("getTableType: no such table exists");
}

int Database::getAttributeIndex(TableType type, int tableIndex, string attributeName) // GOOD TO GO
{
	int columnIndex = -1;

	switch (type)
	{
	case RELATION:
		for (int i = 0; i < _relations[tableIndex]->getRow(1).size(); i++)
		{
			if (attributeName == _relations[tableIndex]->getCell(1, i))
			{
				columnIndex = i;
			}
		}
		break;
	case VIEW:
		for (int i = 0; i < _views[tableIndex]->getRow(1).size(); i++)
		{
			if (attributeName == _views[tableIndex]->getCell(1, i))
			{
				columnIndex = i;
			}
		}
	default:
		throw runtime_error("getAttributeIndex: table has no type or doesn't exist");
	}
	
	return columnIndex;
}

// Print Function
void Database::printTable(string tableName) // GOOD TO GO
{
	int index = getTableIndex(tableName);

	switch (getTableType(tableName))
	{
	case RELATION:
		_relations[index]->printTable();
		break;
	case VIEW:
		_views[index]->printTable();
		break;
	default:
		throw runtime_error("printTable: no such table exists");
		break;
	}
}
