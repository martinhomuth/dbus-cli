#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "terminal.h"

void welcome_message()
{
	printf("welcome to the fancy dbus interactor\n");
}

int main()
{
	welcome_message();

	input_loop();

	return 0;
}
