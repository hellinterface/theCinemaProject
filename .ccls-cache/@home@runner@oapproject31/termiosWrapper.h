#include <stdio.h>
#include <termios.h>

static struct termios old, current;

// Initialize new settings
void initTermios(int echo) 
{
  tcgetattr(0, &old); // Get
  current = old; // Switch
  current.c_lflag &= ~ICANON;
  if (echo) {
      current.c_lflag |= ECHO; // Echo mode
  } else {
      current.c_lflag &= ~ECHO; // No echo mode
  }
  tcsetattr(0, TCSANOW, &current); // Set
}

// Restore old settings
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

// Read 1 character (echo defines echo mode)
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

// Read 1 character without echo
char getch(void) 
{
  return getch_(0);
}

// Read 1 character with echo
char getche(void) 
{
  return getch_(1);
}