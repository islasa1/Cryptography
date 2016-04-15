#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "login.h"

// Test driver to login
int main(void)
{
	char userInput[LINE_LEN];
	bool success = false;
	LoginModuleInit();
	while(1)
	{
		printf("Enter option [l login, n new user, d del user]: ");
		readInput(userInput);
		if(userInput[0] == 'q') break;
		success = loginProtocol(userInput[0]);
	}
	
	success ? printf("Test Complete!\n") : printf("Test Fail!\n");
}