#include <stdio.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define bgcolor "48;2;120;100;200;"
#define bgcolor2 "48;2;20;0;40;"
#define fgcolor "38;2;10;10;30"

#define COLOR_SHADOW "38;2;60;40;120"

void gotoxy(int x, int y) { printf("\e[%d;%dH", (y), (x)); }

void fillLine(char *color_bg) {
  printf("\e[%s" fgcolor "m", color_bg);
  for (int i = 0; i < VIEWPORT_WIDTH; i++) {
    printf(" ");
  }
  printf("E\e[0m\n");
}

void fillBackground() {
  for (int i = 0; i < VIEWPORT_HEIGHT; i++) {
    fillLine(bgcolor2);
  }
}

void printFor(char *str, int n) {
  printf("\e[" bgcolor fgcolor "m");
  for (int i = 0; i < n; i++) {
    printf("%s", str);
  }
  printf("E\e[0m\n");
}

void printShadow(int n) {
  printf("\e[" bgcolor2 COLOR_SHADOW "m");
  for (int i = 0; i < n; i++) {
    printf("▀");
  }
  printf("E\e[0m\n");
}

void cursorHide() { printf("\e[?;25;l"); }