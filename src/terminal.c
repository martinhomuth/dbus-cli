#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define TERMINAL_SAVE_SCREEN "\x1b[?47h"
#define TERMINAL_RESTORE_SCREEN "\x1b[?47l"
#define TERMINAL_CLEAR_SCREEN "\x1b[2J"
#define TERMINAL_GOTO_ROW_COL(x, y) printf("\033[%u;%dH", x, y);
#define INPUT_SIZE 100

enum PROMPT_TYPE { COMMAND_MODE = 0, INPUT_MODE };

static void clear_screen() { write(STDOUT_FILENO, TERMINAL_CLEAR_SCREEN, 4); }

static void save_screen() { write(STDOUT_FILENO, TERMINAL_SAVE_SCREEN, 4); }

static void restore_screen() { write(STDOUT_FILENO, TERMINAL_RESTORE_SCREEN, 4); }

static struct termios orig_termios;
static void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

static void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    /* TODO: disable cursor */
    raw.c_lflag &= ~((unsigned int)ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static int get_winsize(struct winsize *w) {
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, w) == -1) {
        perror("ioctl");
        return 1;
    }

    return 0;
}

static char *_get_current_prompt(int val) {
    static char *_current_prompt;
    switch (val) {
    case COMMAND_MODE:
        _current_prompt = "Command Mode";
        break;
    case INPUT_MODE:
        _current_prompt = "Insert: ";
        break;
    }

    return _current_prompt;
}

void handle_sigwinch(int sig) {
    (void)sig;
    /* we need nothing to do right now */
}

void input_loop() {
    struct winsize w;
    memset(&w, 0, sizeof(w));

    /* register a sigwinch handler to interrupt input and redraw the
       screen */
    struct sigaction sa;
    sa.sa_handler = handle_sigwinch;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGWINCH, &sa, NULL);

    save_screen();
    enable_raw_mode();

    char input[INPUT_SIZE];
    static int prompt_type = 0;
    bool quit = false;

    while (!quit) {

        if (get_winsize(&w))
            return;

        clear_screen();

        TERMINAL_GOTO_ROW_COL(w.ws_row - 5, 1);
        printf("rows: %u, cols: %u\n", w.ws_row, w.ws_col);

        TERMINAL_GOTO_ROW_COL(w.ws_row - 1, 1);
        printf("%s", _get_current_prompt(prompt_type));
        (void)fflush(stdout);

        if (prompt_type == COMMAND_MODE) {
            char c;
            read(STDIN_FILENO, &c, 1);
            if (c == 'q')
                quit = true;
            else if (c == 'i')
                prompt_type = INPUT_MODE;
        } else if (prompt_type == INPUT_MODE) {
            /* TODO: if the terminal gets resized during fgets, the
               input is lost, maybe we want to try to store it in the
               meantime?  Would be done by not disabling raw mode but
               storing and printing every character entered */
            disable_raw_mode();
            fgets(input, INPUT_SIZE, stdin);
        } else {
            printf("Unknown prompt type\n");
            quit = true;
        }

        if (strncmp(input, "command", 6) == 0) {
            prompt_type = COMMAND_MODE;
            enable_raw_mode();
        }

        memset(input, 0, sizeof(input));
        usleep(200);
    }

    restore_screen();
}
