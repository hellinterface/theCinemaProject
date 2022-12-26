#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define bgcolor "48;2;120;100;200;"
#define bgcolor2 "48;2;20;0;40"
#define fgcolor "38;2;10;10;30"

#define COLOR_SHADOW ";38;2;60;40;120"

#define FB(arg) #arg

// https://github.com/pikvic/fefu2022/blob/main/cinema.md
// ☆★
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
// https://en.wikipedia.org/wiki/ANSI_escape_code

typedef struct {
  char *title;
  int year;
  char *country;
  char *genre;
  float rating;
} film;

typedef struct {
  film *previous;
  film *self;
  film *next;
} filmListItem;

int main(void) {
  system("clear");
  printf("\n\n");
  fillBackground();
  gotoxy(0, 2);
  fillLine(bgcolor);
  fillLine(bgcolor);
  fillLine(bgcolor);
  printShadow(VIEWPORT_WIDTH);
  cursorHide();
  gotoxy(4, 3);
  printf("КАТАЛОГ");
  gotoxy(10, 10);
  printf("456☆★");
  gotoxy(0, 0);
  return 0;
}