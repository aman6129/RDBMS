#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "parser.h"

using namespace std;

int main(){
	string input;
	bool Exit = false;
	parser my_parser;

	while(Exit == false)
	{
		getline(cin,input);
		if(input.find("EXIT") == 0)
			Exit = true;

		my_parser.strip_information(input);
	}
}
