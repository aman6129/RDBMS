#include "Table.h"
#include <iostream>
#include <string>

Table::Table() // GOOD TO GO
{
	_cells = vector<vector<string>>();
}

Table::Table(vector<vector<string>> vec) // GOOD TO GO
{
	_cells = vec;
}

void Table::addRow(vector<string> row) // GOOD TO GO
{
	_cells.push_back(row);
}

void Table::removeRow(int row) // GOOD TO GO
{
	_cells.erase(_cells.begin() + row);
}

void Table::setAttributeTypes(vector<string> attributeTypes) // GOOD TO GO
{
	_attributeTypes = attributeTypes;
}

void Table::setKeys(vector<string> keys) // GOOD TO GO
{
	_keys = keys;
}

void Table::setCell(int row, int column, string data) // GOOD TO GO
{
	_cells[row][column] = data;
}

int Table::getNumRows() // GOOD TO GO
{
	return _cells.size();
}

vector<string> Table::getRow(int row) // GOOD TO GO
{
	return _cells[row];
}

string Table::getCell(int row, int column) // GOOD TO GO
{
	return _cells[row][column];
}

void Table::printTable() // GOOD TO GO
{
	for (int rows = 0; rows < _cells.size(); rows++)
	{
		for (int columns = 0; columns < _cells[rows].size(); columns++)
		{
			cout << _cells[rows][columns] << "\t";
		}
		cout << endl;
	}
}
