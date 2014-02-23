#include "Parser.h"
#include "Database.h"
#include <iostream>

int main() {
	string inp;
	Parser parse;

	while(getline(cin,inp))
		parse.execute(inp);
}