#ifndef PARSER_H_
#define PARSER_H_

#include "Database.h"

#include <string>
#include <vector>
#include <utility>

using namespace std;

class Parser{

public:
	enum Token {
		arrow, leftParen, rightParen, plus, minus, mult, comma, semiCol, equals,
		eq, notEq, less, greater, lessEq, greaterEq, Or, And,

		SELECT, PROJECT, RENAME, OPEN, CLOSE, WRITE, EXIT, SHOW,
		CREATE_TABLE, PRIMARY_KEY, UPDATE, SET, WHERE, INSERT_INTO,
		VALUES_FROM, VALUES_FROM_RELATION, DELETE_FROM, VARCHAR, INTEGER_SYM,

		integer, identifier, literal
	};
	const char* token_strings [39] = {
		"arrow", "leftParen", "rightParen", "plus", "minus", "mult", "comma", "semiCol", "equals",
		"eq", "notEq", "less", "greater", "lessEq", "greaterEq", "Or", "And",

		"SELECT", "PROJECT", "RENAME", "OPEN", "CLOSE", "WRITE", "EXIT", "SHOW",
		"CREATE_TABLE", "PRIMARY_KEY", "UPDATE", "SET", "WHERE", "INSERT_INTO",
		"VALUES_FROM", "VALUES_FROM_RELATION", "DELETE_FROM", "VARCHAR", "INTEGER_SYM",

		"integer", "identifier", "literal"
	};
	bool execute(string input);


private:
	template <class T> bool compare(T leftArg, T rightArg, Token op);
	string getPrevData();
	string getView();
	Token getCurrToken();

	void lex(string input);
	bool getSecondWord(string firstWord);
	void addToken(Token token, string s);
	Token getToken(string s);
	void parse();

	int accept(Token);
	int expect(Token, string error);

	string expression();
	string atomicExpr();
	string selection();
	string projection();
	string renaming();
	string setUnion(string leftArgument);
	string setDifference(string leftArgument);
	string crossProduct(string leftArgument);

	vector<bool> condition(const vector<vector<string> >& table = vector<vector<string> >());
	vector<bool> conjunction(const vector<vector<string> >& table = vector<vector<string> >());
	vector<bool> comparison(const vector<vector<string> >& table = vector<vector<string> >());

	vector<string> attbList();
	pair<vector<string>, vector<string> > typedAttbList();
	string type();

	vector<Token> tokens;
	vector<string> rawData;
	unsigned int currIndex = 0;
	int currAnonView = 0;

	Database db;

	bool query();
	bool command();
	void open();
	void close();
	void write();
	void show();
	void createTable();
	void update();
	void insertInto();
	void deleteFrom();

};

#endif