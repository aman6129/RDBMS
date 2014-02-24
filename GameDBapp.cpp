#include "GameDBapp.h"

void GameDBapp::wait_for_input()
{

}

void GameDBapp::show_main_basic()
{

	cout << setw(40)
		 << "Commands & Queries:\n"
		 << "Press the number that corresponds with the option below to call that option\n\n"
		 << "1. Add\n"
		 << "2. Remove\n"
		 << "3. Combine\n"
		 << "4. Display\n"
		 << "5. New\n"
		 << "6. Save\n"
		 << "7. Open\n"
		 << "8. Save and Close\n"
		 << "9. Show advanced menu\n"
		 << "10. Exit\n";

	
}

void GameDBapp::start_app()
{
	show_main_basic();
}