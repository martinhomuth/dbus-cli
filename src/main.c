#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "terminal.h"

static bool quit = false;

void welcome_message() { printf("welcome to the fancy dbus interactor\n"); }

int main() {

    welcome_message();

    while (!quit) {
        prompt();
    }

    return 0;
}
