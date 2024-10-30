#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define TERMINAL_CLEAR_SCREEN printf("\033[2J");
#define TERMINAL_GOTO_ROW_COL(x, y) printf("\033[%u;%dH", x, y);
#define INPUT_SIZE 100

static int get_winsize(struct winsize *w) {
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, w) == -1) {
        perror("ioctl");
        return 1;
    }

    return 0;
}

void prompt() {
    struct winsize w;
    memset(&w, 0, sizeof(w));

    char input[INPUT_SIZE];

    while (true) {
        if (get_winsize(&w))
            return;

        TERMINAL_CLEAR_SCREEN;
        TERMINAL_GOTO_ROW_COL(w.ws_row - 5, 1);
        printf("rows: %u, cols: %u\n", w.ws_row, w.ws_col);

        TERMINAL_GOTO_ROW_COL(w.ws_row - 1, 1);
        printf("Oh what a happy day\n");
        (void)fflush(stdout);

        memset(input, 0, sizeof(input));

        usleep(200);
    }
}
