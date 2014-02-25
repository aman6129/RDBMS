#include "GameDBapp.h"
//#include "Parser.h"

void GameDBapp::add()
{
	cout << "Which relation would you like to add to? ";
	cin >> relation_name;
	cout << "Insert the elements you would like to add in the format described below\n"
		 << "( elem_1, elem_2, ..., elem_n)\n";
	string inputs;
	getline(cin, inputs);
	//cout << inputs << "\n";

}

void GameDBapp::wait_for_input()
{
	int input;
	while(cin >> input)
	{
		if(input == 10)
		{
			cout << "Exiting the application\n";
			break;
		}
		else if(input == 1)
		{
			add();
		}
		else if(input == 2)
		{
			//remove();
		}
		else if(input == 3)
		{}
		else if(input == 4)
		{}
		else if(input == 5)
		{}
		else if(input == 6)
		{}
		else if(input == 7)
		{}
		else if(input == 8)
		{}
		else if(input == 9)
		{
			show_main_advanced();
		}
		else
		{
			cout << "Bad input! Press 10 to Exit, press 9 to show the advanced menu\n\n";
		}
	}
}

void GameDBapp::show_main_basic()
{

	cout << "Commands & Queries:\n"
		 << "Enter the number that corresponds with the option below to call that command/query\n\n"
		 << "1. Add\n"
		 << "2. Remove\n"
		 << "3. Combine\n" //Do the relational algebra shit here
		 << "4. Display\n"
		 << "5. New\n"
		 << "6. Save\n"
		 << "7. Open\n"
		 << "8. Close\n"
		 << "9. Show advanced menu\n"
		 << "10. Exit\n\n";

	wait_for_input();

	
}

void GameDBapp::show_main_advanced()
{
	cout << "Command & Queries Advanced menu:\n"
		 << "Enter the number that corresponds with the option below to call that command/query\n\n"
		 << "1. Add to a specific relation\n"
		 << "2. Remove from a relation based on conditions\n"
		 << "3. Combine allows you to perform relational algebra on relations/views\n"
		 << "4. Display the contents of a relation or view\n"
		 << "5. Create a new relation\n"
		 << "6. Saves stuff to file\n"
		 << "7. Open and read from a file\n"
		 << "8. Close and save values to a file\n"
		 << "9. Shows the more advanced table\n"
		 << "10. Quit the database application\n\n";

	wait_for_input();
}

void GameDBapp::start_app()
{
	show_main_basic();
}