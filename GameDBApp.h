#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "Parser.h"

#include <vector>
#include <iostream>

using namespace std;

class Application{
	private:
		Parser parser;

		vector<string> tables {"Game", "Accounts", "Servers", "Characters", "Items", "Friends"};

		vector<string> gameAttb {"gmName"};
		vector<string> acctAttb {"acctName", "password"};
		vector<string> serverAttb {"svName", "ipAddress"};
		vector<string> charAttb {"chName", "level"};
		vector<string> itemAttb{"itName", "value"};


		vector<vector<string> > table_keys {
			{"gmName"},
			{"acctName", "password"},
			{"svName", "ipAddress"},
			{"chName", "level"},
			{"itName", "value"}};	

		vector<vector<string> > attributes {gameAttb, acctAttb, serverAttb,
			charAttb, itemAttb};

		vector<vector<string> > attbTypes {
			{"VARCHAR(255)"},
			{"VARCHAR(255)", "VARCHAR(255)"},
			{"VARCHAR(255)", "VARCHAR(255)"},
			{"VARCHAR(255)", "INTEGER"},
			{"VARCHAR(255)", "INTEGER"}};

		void display_main_menu();
		void display_table_menu(string table);
		void table_menu(int table_index);

		void add(string table, vector<string> attributes, vector<string> attbTypes);
		void remove(string table, vector<string> attributes,
			vector<string> attbTypes, vector<string> keys);
		void update(string table, vector<string> attributes,
			vector<string> attbTypes, vector<string> keys);

	public:
		Application();
		~Application();

		void mainMenu();
};
