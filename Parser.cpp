#include "Parser.h"
#include "Database.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>
#include <utility>

using namespace std;

bool Parser::execute(string input)
{
	bool retVal = true;
	try{
		lex(input);
		parse();
	}
	catch(exception& e) {
		retVal = false;
	}
	return retVal;
}

template <class T> bool Parser::compare(T leftArg, T rightArg, Token op) 
{
	bool retVal;

	switch(op) {
		case eq:
			retVal = leftArg == rightArg;
			break;
		case notEq:
			retVal = leftArg != rightArg;
			break;
		case less:
			retVal = leftArg < rightArg;
			break;
		case greater:
			retVal = leftArg > rightArg;
			break;
		case greaterEq:
			retVal = leftArg >= rightArg;
			break;
		case lessEq:
			retVal = leftArg <= rightArg;
			break;
		default:
			throw runtime_error("compare: unexpected error");
			break;
	}

	return retVal;
}

Parser::Token Parser::getCurrToken() 
{
	return tokens[currIndex];
}

string Parser::getPrevData() 
{
	return rawData[currIndex - 1];
}

string Parser::getView() 
{
	string viewName = to_string(currAnonView);
	++currAnonView;
	return viewName;
}


//commaND FUNCTIONS
void Parser::open() 
{
	expect(identifier, "open: expected identifier");
	string file_name = getPrevData();
	expect(semiCol, "command: expected semicolon");
	ifstream inFile;
	inFile.open(file_name + ".db");
	if(!inFile.is_open())
		throw runtime_error("Open: file failed to open");

	vector<Token> oldTokens = tokens;
	vector<string> oldRawData = rawData;
	string newLine;
	getline(inFile, newLine);
	if(execute(newLine) == false) {
		inFile.close();
		tokens = oldTokens;
		rawData = oldRawData;
		throw runtime_error("open: relation is already open");
	}

	while(!inFile.eof()) {
		getline(inFile, newLine);
		execute(newLine);
	}
	inFile.close();

	tokens = oldTokens;
	rawData = oldRawData;
}

void Parser::close() 
{
	expect(identifier, "close: expected identifier");
	string tableName = getPrevData();
	expect(semiCol, "command: expected semicolon");
	db.close(tableName);
}

void Parser::write() 
{
	expect(identifier, "write: expected identifier");
	string tableName = getPrevData();
	expect(semiCol, "command: expected semicolon");
	db.write(tableName);
}

void Parser::show() 
{
	string tableName = atomicExpr();
	expect(semiCol, "command: expected semicolon");
	db.show(tableName);
}

void Parser::createTable() 
{
	pair<vector<string>, vector<string> > attbsTypes;
	vector<string> keys;
	string tableName;

	expect(identifier, "create table: expected identifier");
	tableName = getPrevData();

	expect(leftParen, "create table: expected '('");
	attbsTypes = typedAttbList();
	expect(rightParen, "create table: expected ')'");


	expect(PRIMARY_KEY, "create table: expected 'PRIMARY KEY'");
	expect(leftParen, "create table: expected '('");
	keys = attbList();
	expect(rightParen, "create table: expected ')'");

	expect(semiCol, "command: expected semicolon");
	db.create(tableName, attbsTypes.first, attbsTypes.second, keys);
}

void Parser::update() 
{
	string tableName;
	vector<string> attbNames;
	vector<string> newVal;
	vector<Token> type;
	vector<vector<string> > table;
	vector<bool> truthVals;

	expect(identifier, "update: expected identifier");
	tableName = getPrevData();

	expect(SET, "update: expected 'SET'");
	do {
		expect(identifier, "update: expected identifier");
		attbNames.push_back(getPrevData());

		expect(equals, "update: expected '='");
		if(accept(literal)) {
			type.push_back(literal);
			newVal.push_back(getPrevData());
		}
		else if(accept(integer)) {
			type.push_back(integer);
			newVal.push_back(getPrevData());
		}
		else
			throw runtime_error("update: expected literal or integer");
	} while (accept(comma));

	table = db.getTable(tableName);
	expect(WHERE, "update: expected 'WHERE'");
	truthVals = condition(table);

	expect(semiCol, "command: expected semicolon");

	for(unsigned int i = 0; i < truthVals.size(); ++i) 
		if(truthVals[i] == true)
			db.update(tableName, attbNames, newVal, i + 3);
}

void Parser::insertInto() 
{
	string relName;

	expect(identifier, "insert into: expected identifier");
	relName = getPrevData();

	if(accept(VALUES_FROM)) {
		vector<string> tuple;

		expect(leftParen, "insert into: expected '('");
		do {
			if(accept(literal))
				tuple.push_back(getPrevData());
			else if(accept(integer))
				tuple.push_back(getPrevData());
			else if(accept(minus)) {
				expect(integer, "insert into: expected integer");
				string value = "-";
				value += getPrevData();
				tuple.push_back(value);
			}
			else
				throw runtime_error("insert into: unexpected symbol");
		} while(accept(comma));
		expect(rightParen, "insert into: expected ')'");
		expect(semiCol, "command: expected semicolon");
		db.insertTupleInto(relName, tuple);
	}
	else if(accept(VALUES_FROM_RELATION)) {
		string viewName = expression();
		expect(semiCol, "command: expected semicolon");
		db.insertViewInto(relName, viewName);
	}
	else
		throw runtime_error("insert into: unexpected symbol");
}

void Parser::deleteFrom() 
{
	string relName;
	vector<bool> truthVals;
	unsigned int vectorOffset = 3;

	expect(identifier, "delete from: expected identifier");
	relName = getPrevData();
	const vector<vector<string> >& table = db.getTable(relName);

	expect(WHERE, "delete from: expected 'WHERE'");
	truthVals = condition(table);

	for(unsigned int i = 0; i < truthVals.size(); ++i)
		if(truthVals[i] == true) {
			db.remove(relName, i + vectorOffset);
			--vectorOffset;
		}
}



//LEXER
void Parser::lex(string inputStr) 
{
	rawData.clear();
	tokens.clear();
	for(unsigned int index = 0; index<inputStr.size();){
		unsigned int startIndex = index;
		bool check = false;
		while(isalpha(inputStr[index]) || isdigit(inputStr[index]) || inputStr[index] == '_'){
			if(index < inputStr.size())
				index++;
			check = true;
		}
		unsigned int endIndex = index - 1;
		string tempStr;
		if(check == true){
			for(unsigned int i=startIndex; i<=endIndex; i++)
				tempStr.push_back(inputStr[i]);
			if(getSecondWord(tempStr)){
				tempStr.push_back(' ');
				index++;
				while(isalpha(inputStr[index])){
					tempStr.push_back(inputStr[index]);
					if(index < inputStr.size())
						index++;
				}
				if(index+1 < inputStr.size() && inputStr[index+1] == 'R'){
					tempStr.push_back(' ');
					index++;
					while(isalpha(inputStr[index])){
						tempStr.push_back(inputStr[index]);
						if(index < inputStr.size())
							index++;
					}

				}

			}
			addToken(getToken(tempStr), tempStr);
		}
		else{	
			string switchStr;
			switch(inputStr[index]){
				case '<':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index+1] == '='){
						switchStr.push_back(inputStr[index+1]);
						addToken(lessEq, switchStr);
						index++;
					}
					else if(index+1 < inputStr.size() && inputStr[index+1] == '-'){
						switchStr.push_back(inputStr[index+1]);
						addToken(arrow, switchStr);
						index++;
					}
					else
						addToken(less, switchStr);
					break;
				case '(':
					switchStr.push_back(inputStr[index]);
					addToken(leftParen, switchStr);
					break;
				case ')':
					switchStr.push_back(inputStr[index]);
					addToken(rightParen, switchStr);
					break;
				case '+':
					switchStr.push_back(inputStr[index]);
					addToken(plus, switchStr);
					break;
				case '-':
					switchStr.push_back(inputStr[index]);
					addToken(minus, switchStr);
					break;
				case '*':
					switchStr.push_back(inputStr[index]);
					addToken(mult, switchStr);
					break;
				case ',':
					switchStr.push_back(inputStr[index]);
					addToken(comma, switchStr);
					break;
				case ';':
					switchStr.push_back(inputStr[index]);
					addToken(semiCol, switchStr);
					break;
				case '=':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index+1] == '='){
						switchStr.push_back(inputStr[index+1]);
						addToken(eq,switchStr);
						index++;
					}
					else
						addToken(equals, switchStr);
					break;
				case '!':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index +1] == '='){
						switchStr.push_back(inputStr[index+1]);
						addToken(notEq, switchStr);
						index++;
					}
					else
						throw runtime_error("Bad Input");
					break;
				case '>':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index+1] == '='){
						switchStr.push_back(inputStr[index+1]);
						addToken(greaterEq,switchStr);
						index++;
					}
					else
						addToken(greater, switchStr);
					break;
				case '|':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index+1] == '|'){
						switchStr.push_back(inputStr[index+1]);
						addToken(Or,switchStr);
						index++;
					}
					else
						throw runtime_error("Bad Input");
					break;
				case '&':
					switchStr.push_back(inputStr[index]);
					if(index+1 < inputStr.size() && inputStr[index+1] == '&'){
						switchStr.push_back(inputStr[index+1]);
						addToken(And,switchStr);
						index++;
					}
					else
						throw runtime_error("Bad Input");
					break;
				case '"':
					index++;
					while(inputStr[index] != '"' && index < inputStr.size()){
						switchStr.push_back(inputStr[index]);
						index++;
					}
					if(inputStr[index] != '"')
						throw runtime_error("Bad Input");
					else {
						addToken(literal, switchStr);
					}

					break;
				case ' ': case '\n': case '\r':
					break;
				default:
					throw runtime_error("Invalid token");
					break;
			}
			index++;
		}
	}	
}

Parser::Token Parser::getToken(string s)
{
	if(s == "select")
		return SELECT;
	else if(s == "project")
		return PROJECT;
	else if(s== "rename")
		return RENAME;
	else if(s == "OPEN")
		return OPEN;
	else if(s == "CLOSE")
		return CLOSE;
	else if(s == "WRITE")
		return WRITE;
	else if(s == "EXIT")
		return EXIT;
	else if(s == "SHOW")
		return SHOW;
	else if(s == "UPDATE")
		return UPDATE;
	else if(s == "SET")
		return SET;
	else if(s == "WHERE")
		return WHERE;
	else if (s == "CREATE TABLE")
		return CREATE_TABLE;
	else if (s == "PRIMARY KEY")
		return PRIMARY_KEY;
	else if (s == "INSERT INTO")
		return INSERT_INTO;
	else if (s == "VALUES FROM")
		return VALUES_FROM;
	else if (s == "VALUES FROM RELATION")
		return VALUES_FROM_RELATION;
	else if (s == "DELETE FROM")
		return DELETE_FROM;
	else if(s == "VARCHAR")
		return VARCHAR;
	else if(s == "integer")
		return INTEGER_SYM;
	else if(isdigit(s[0]))		
		return integer;
	else if(isalpha(s[0]))
		return identifier;
	else
		throw runtime_error("Invalid token");
}

void Parser::addToken(Token token, string s)
{
	tokens.push_back(token);
	rawData.push_back(s);
}

bool Parser::getSecondWord(string firstWord)
{
	if(firstWord == "CREATE" || firstWord == "PRIMARY"
		|| firstWord == "INSERT" || firstWord == "VALUES" || firstWord == "DELETE")
		return(true);
	else
		return(false);
}

int Parser::accept(Token t) 
{
	if(currIndex >= tokens.size())
		throw runtime_error("parser: invalid input");
	if(tokens[currIndex] == t) {
		++currIndex;
		return 1;
	}
	return 0;
}

int Parser::expect(Token t, string error) 
{
	if(accept(t)) 
		return 1;
	throw runtime_error(error);
	return 0;
}

void Parser::parse() 
{
	currIndex = 0;
	if(!query() && !command())
		throw runtime_error("parser: unexpected symbol");
}


bool Parser::query() 
{
	string expected_name;
	string anon_name;
	
	//Check if query
	if(!accept(identifier))
		return false; 
	expected_name = getPrevData();

	expect(arrow, "query: expected left arrow");
	anon_name = expression();
	expect(semiCol, "query: expected semicolon");

	db.updateViewName(expected_name, anon_name);
	return true;
}

bool Parser::command() 
{
	if(accept(OPEN))
		open();
	else if(accept(CLOSE)) 
		close();
	else if(accept(WRITE)) 
		write();
	else if(accept(EXIT))
		db.exit();
	else if(accept(SHOW))
		show();
	else if(accept(CREATE_TABLE))
		createTable();
	else if(accept(UPDATE))
		update();
	else if(accept(INSERT_INTO)) 
		insertInto();
	else if(accept(DELETE_FROM))
		deleteFrom();
	else 
		return false;
	return true;
}

//EXPRESSIONS
string Parser::expression() {
	string viewName;

	if(accept(SELECT)) 
		viewName = selection();
	else if(accept(PROJECT))
		viewName = projection();
	else if(accept(RENAME))
		viewName = renaming();
	else {
		viewName = atomicExpr();
		if(accept(plus))
			viewName = setUnion(viewName);
		else if(accept(minus))
			viewName = setDifference(viewName);
		else if(accept(mult))
			viewName = crossProduct(viewName);
	}

	return viewName;
}

string Parser::atomicExpr() 
{
	string viewName;

	if(accept(identifier)) {
		viewName = getPrevData();
	}
	else if(accept(leftParen)) {
		viewName = expression();
		expect(rightParen, "atomic expression: expected ')'");
	}
	else
		throw runtime_error("atomic expression: unexpected symbol");

	return viewName;
}

//QUERY 
string Parser::selection() 
{
	string viewName = getView();
	string inTableName;
	int begin_condition;
	int after_atomic;
	vector<bool> truthVals;

	// first parse the condition without executing any comparisons
	expect(leftParen, "selection: expected '('");
	begin_condition = currIndex;
	condition();
	expect(rightParen, "selection: expected ')'");
	inTableName = atomicExpr();
	after_atomic = currIndex;

	// now re-parse the condition while doing all comparisons
	currIndex = begin_condition;
	const vector<vector<string> >& table = db.getTable(inTableName);
	truthVals = condition(table);

	// call select on every index
	for(unsigned int i = 0; i < truthVals.size(); ++i)
		if(truthVals[i] == true)
			db.selection(viewName, inTableName, i+3);

	currIndex = after_atomic;
	return viewName;
}

string Parser::projection() 
{
	string viewName = getView();
	string inTableName;
	vector<string> attributes;

	expect(leftParen, "projection: expected '('");
	attributes = attbList();
	expect(rightParen, "projection: expected ')'");
	inTableName = atomicExpr();

	db.projection(viewName, inTableName, attributes);

	return viewName;
}

string Parser::renaming() 
{
	string viewName = getView();
	string inTableName;
	vector<string> attributes;

	expect(leftParen, "rename: expected '('");
	attributes = attbList();
	expect(rightParen, "rename: expected ')'");
	inTableName = atomicExpr();

	db.rename(viewName, inTableName, attributes);

	return viewName;
}

string Parser::setUnion(string leftArgument) 
{
	string viewName = getView();
	string rightArgument = atomicExpr(); 

	db.setUnion(viewName, leftArgument, rightArgument);

	return viewName;
}

string Parser::setDifference(string leftArgument) 
{
	string viewName = getView();
	string rightArgument = atomicExpr(); 

	db.setDifference(viewName, leftArgument, rightArgument);

	return viewName;
}

string Parser::crossProduct(string leftArgument) 
{
	string viewName = getView();
	string rightArgument = atomicExpr(); 

	db.crossProduct(viewName, leftArgument, rightArgument);

	return viewName;
}

//CONDITIONAL
vector<bool> Parser::condition(const vector<vector<string> >& table) 
{
	vector<bool> values;
	if(table.size() > 2) {
		vector<bool> initializer(table.size()-3, false);
		values = initializer;
	}

	do {
		vector<bool> newVal = conjunction(table);
		for(unsigned int i = 0; i < values.size() && i < newVal.size(); ++i) 
			values[i] = values[i] || newVal[i];
	} while(accept(Or));

	return values;
}

vector<bool> Parser::conjunction(const vector<vector<string> >& table) 
{
	vector<bool> values;
	if(table.size() > 2) {
		vector<bool> initializer(table.size()-3, true);
		values = initializer;
	}

	do {
		vector<bool> newVal = comparison(table);
		for(unsigned int i = 0; i < values.size() && i < newVal.size(); ++i) 
			values[i] = values[i] && newVal[i];
	} while(accept(And));

	return values;
}

vector<bool> Parser::comparison(const vector<vector<string> >& table) 
{
	vector<bool> values;
	if(table.size() > 2) {
		vector<bool> initializer(table.size()-3);
		values = initializer;
	}

	if(accept(leftParen)) {
		values = condition(table);
		expect(rightParen, "comparison: expected ')'");
	}
	else {
		expect(identifier, "comparison: left operand: expected identifier");
		string leftArg = getPrevData();

		Token op = getCurrToken();
		if(!(accept(eq) || accept(notEq) || accept(greater) || accept(less) || accept(lessEq) || accept(greaterEq)))
			throw runtime_error("op: unexpected operator");

		Token right_arg_type = getCurrToken();
		if(!(accept(identifier) || accept(literal) || accept(integer)))
			throw runtime_error("comparison: right operand: unexpected operand");
		string rightArg = getPrevData();

		if(table.size() > 2) {
			// get first attribute index
			int attribute_index1 = -1;
			Token attribute_index1_type;
			for(unsigned int i = 0; i < table[1].size(); ++i)
				if(table[1][i] == leftArg) {
					attribute_index1 = i;
					string type = table[2][i];
					switch(type[0]) {
						case 'I':
							attribute_index1_type = integer;
							break;
						case 'V':
							attribute_index1_type = VARCHAR;
							break;
					}
					break;
				}
			if(attribute_index1 == -1)
				throw runtime_error("comparison: left operand: no such attribute");

			switch(right_arg_type) {
				case identifier: {
					// get second attribute index
					int attribute_index2 = -1;
					Token attribute_index2_type;
					for(unsigned int i = 0; i < table[1].size(); ++i)
						if(table[1][i] == rightArg) {
							attribute_index2 = i;
							string type = table[2][i];
							switch(type[0]) {
								case 'I':
									attribute_index2_type = integer;
									break;
								case 'V':
									attribute_index2_type = VARCHAR;
									break;
							}
							break;
						}
					if(attribute_index2 == -1)
						throw runtime_error("comparison: right operand: no such attribute");

					if(attribute_index1_type != attribute_index2_type)
						throw runtime_error("comparison: incompatible attribute types");

					// do comparisons on all tuples
					if(attribute_index1_type == integer)
						for(unsigned int i = 3; i < table.size(); ++i) {
							int left = stoi(table[i][attribute_index1]);
							int right = stoi(table[i][attribute_index2]);
							values[i-3] = compare<int>(left, right, op);
						}
					else if(attribute_index1_type == VARCHAR)
						for(unsigned int i = 3; i < table.size(); ++i) {
							string left = table[i][attribute_index1];
							string right = table[i][attribute_index2];
							values[i-3] = compare<string>(left, right, op);
						}
					else
						throw runtime_error("comparison: argument types: unexpected error");

					break;
				}
				case literal: {
					if(attribute_index1_type != VARCHAR)
						throw runtime_error("comparison: incompatible arguments");

					for(unsigned int i = 3; i < table.size(); ++i) 
						values[i-3] = compare<string>(table[i][attribute_index1], rightArg, op);
					break;
				}
				case integer: {
					if(attribute_index1_type != integer)
						throw runtime_error("comparison: incompatible arguments");

					for(unsigned int i = 3; i < table.size(); ++i) {
						int left = stoi(table[i][attribute_index1]);
						int right = stoi(rightArg);
						values[i-3] = compare<int>(left, right, op);
					}
					break;
				}
				default:
					throw runtime_error("comparison: unexpected error");
					break;
			}
		}
	}

	return values;
}

//LISTS
vector<string> Parser::attbList() 
{
	vector<string> attributes;

	do {
		expect(identifier, "attribute list: expected identifier");
		attributes.push_back(getPrevData());
	} while(accept(comma));

	return attributes;
}

pair<vector<string>, vector<string> > Parser::typedAttbList() 
{
	vector<string> attributes;
	vector<string> types;

	do {
		expect(identifier, "typed attribute list: expected identifier");
		attributes.push_back(getPrevData());

		types.push_back(type());
	} while(accept(comma));

	pair<vector<string>, vector<string> > return_pair (attributes, types);
	return return_pair;
}

string Parser::type() 
{
	string type;
	if(accept(VARCHAR)) {
		type += getPrevData();

		expect(leftParen, "VARCHAR: expected '('");
		type += getPrevData();

		expect(integer, "VARCHAR: expected integer");
		type += getPrevData();

		expect(rightParen, "VARCHAR: expected ')'");
		type += getPrevData();
	}
	else if(accept(INTEGER_SYM)) {
		type += getPrevData();
	}
	else
		throw runtime_error("type: unexpected symbol");
	return type;
}


