#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "util.h"

#define TERMINAL_SAVE_SCREEN "\x1b[?47h"
#define TERMINAL_RESTORE_SCREEN "\x1b[?47l"
#define TERMINAL_CLEAR_SCREEN "\x1b[2J"
#define TERMINAL_ENABLE_CURSOR "\x1b[?25h"
#define TERMINAL_DISABLE_CURSOR "\x1b[?25l"
#define TERMINAL_GOTO_ROW_COL(x, y) printf("\033[%u;%dH", x, y);
#define INPUT_SIZE 100

enum PROMPT_TYPE { COMMAND_MODE = 0, INPUT_MODE };

static struct termios orig_termios;

static void clear_screen()
{
	write(STDOUT_FILENO, TERMINAL_CLEAR_SCREEN, 4);
}

static void restore_screen()
{
	write(STDOUT_FILENO, TERMINAL_RESTORE_SCREEN, 4);
}

static void save_screen()
{
	atexit(restore_screen);
	write(STDOUT_FILENO, TERMINAL_SAVE_SCREEN, 4);
}

static void enable_cursor()
{
	write(STDOUT_FILENO, TERMINAL_ENABLE_CURSOR, 4);
}

static void disable_cursor()
{
	atexit(enable_cursor);
	write(STDOUT_FILENO, TERMINAL_DISABLE_CURSOR, 4);
}

static void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disable_raw_mode);

	struct termios raw = orig_termios;
	/* TODO: disable cursor */
	raw.c_lflag &= ~((unsigned int)ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static int get_winsize(struct winsize *w)
{
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, w) == -1) {
		die("ioctl");
	}

	return 0;
}

static char *_get_current_prompt(int val)
{
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

static void handle_sigwinch(int sig)
{
	(void)sig;
	/* we need nothing to do right now */
}

static int register_sigwinch_handler()
{
	struct sigaction sa;
	sa.sa_handler = handle_sigwinch;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	return sigaction(SIGWINCH, &sa, NULL);
}

void input_loop()
{
	struct winsize w;
	memset(&w, 0, sizeof(w));

	if (register_sigwinch_handler()) {
		die("register sigwinch handler");
	}

	save_screen();
	enable_raw_mode();
	disable_cursor();

	char input[INPUT_SIZE];
	char command[INPUT_SIZE];
	char *input_ptr = input;
	static int prompt_type = 0;
	bool quit = false;

	while (!quit) {
		if (get_winsize(&w))
			return;

		clear_screen();

		TERMINAL_GOTO_ROW_COL(w.ws_row - 5, 1);
		printf("rows: %u, cols: %u\n", w.ws_row, w.ws_col);

		TERMINAL_GOTO_ROW_COL(w.ws_row - 8, 1);
		printf("command: %s\n", command);
		memset(command, 0, sizeof(command));

		TERMINAL_GOTO_ROW_COL(w.ws_row - 1, 1);
		printf("%s", _get_current_prompt(prompt_type));

		(void)fflush(stdout);

		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1)
			die("read");

		switch (prompt_type) {
		case COMMAND_MODE:
			if (c == 'q')
				quit = true;
			if (c == 'i') {
				prompt_type = INPUT_MODE;
				enable_cursor();
				disable_raw_mode();
			}
			break;
		case INPUT_MODE:
			if (c == '\n') {
				*input_ptr++ = '\0'; // TODO: add check to avoid
					// buffer overrun
				memcpy(command, input,
				       (size_t)(input_ptr - input));
				memset(input, 0, sizeof(input));
				input_ptr = input;
			} else {
				*input_ptr++ = c;
			}
			break;
		default:
			die("ERROR: unknown prompt");
		}

		if (strncmp(command, "command", 7) == 0) {
			prompt_type = COMMAND_MODE;
			enable_raw_mode();
			disable_cursor();
		}

		usleep(100);
	}

	restore_screen();
}
