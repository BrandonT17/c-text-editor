/*** includes ***/
#include <ctype.h> 
#include <errno.h> // errno and EAGAIN
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> // read() and STDIN_FILENO come from this library 
#include <termios.h> // termios, tcgetattr(), tcsetattr(), ECHO, TCSAFLUSH
// read() returns the # of bytes read 

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// print error message and exit program
void die(const char *s) {
	perror(s);
	exit(1);
}

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

// function to turn of echoing (redisplaying what is inputted into the program)
void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
	atexit(disableRawMode); // turn off raw mode when program exits
	
	struct termios raw = &orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // turn off echo and canonical mode (turn off line-by-line input reading) and turn off program termination when ctrl-c or ctrl-z are pressed
	// program time-out once no input is detected for a certain amount of time 
	raw.c_cc[VMIN] = 0; // set min. num of bytes of input needed before read() can return  
	raw.c_cc[VTIME] = 1; // max amount of time to wait before read() returns 

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 1) die("tcsetattr");
}

/*** init ***/

int main() {
	enableRawMode();

	char c;
	while (1) {
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read"); 
		if (iscntrl(c)) {
			printf("%d\r\n", c); // if char is a control character, don't print to screen (only print ASCII) 
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == CTRL_KEY('q')) break; // quit upon pressing Ctrl-Q
	}
	return 0;
}
