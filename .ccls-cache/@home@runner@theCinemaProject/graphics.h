#include <stdio.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define COLOR_BACKGROUND_FRONT "48;2;120;100;200;"
#define COLOR_BACKGROUND_BACK "48;2;64;51;117;"
#define COLOR_BACKGROUND_APP "48;2;20;0;40;"
#define COLOR_TEXT_FRONT "38;2;10;10;30"
#define COLOR_SHADOW_FRONT "38;2;60;40;120"
#define COLOR_SHADOW_BACK "38;2;33;26;66"

void gotoxy(int x, int y) { printf("\e[%d;%dH", (y), (x)); }

void fillLine(char *color_bg) {
  printf("\e[%s" COLOR_TEXT_FRONT "m", color_bg);
  for (int i = 0; i < VIEWPORT_WIDTH; i++) {
    printf(" ");
  }
  printf("\e[0m\n");
  // printf("E\e[0m\n");
}

void fillBackground() {
  for (int i = 0; i < VIEWPORT_HEIGHT; i++) {
    fillLine(COLOR_BACKGROUND_APP);
  }
}

void printFor(char *str, int n, char *color_bg) {
  printf("\e[%s" COLOR_TEXT_FRONT "m", color_bg);
  for (int i = 0; i < n; i++) {
    printf("%s", str);
  }
  printf("\e[0m\n");
  // printf("E\e[0m\n");
}

void printShadow(int x1, int y1, int x2, char *color_bg, char *color_fg) {
  gotoxy(x1, y1);
  x2++;
  printf("\e[%s%sm", color_bg, color_fg);
  for (int i = 0; i < x2 - x1; i++) {
    printf("▀");
  }
  printf("\e[0m\n");
  // printf("E\e[0m\n");
}

void printFm(char *str, char *color_bg, char *color_fg) {
  printf("\e[%s%sm", color_bg, color_fg);
  printf("%s", str);
  printf("\e[0m\n");
  // printf("E\e[0m\n");
}

void drawRect(int x1, int y1, int x2, int y2, char *color_bg) {
  // printf("X");
  if (x1 > x2) {
    int temp = x1;
    y1 = x2;
    y2 = temp;
  }
  if (y1 > y2) {
    int temp = y1;
    y1 = y2;
    y2 = temp;
  }
  x2++;
  y2++;
  for (int a = 0; a < y2 - y1; a++) {
    gotoxy(x1, y1 + a);
    // printf("A");
    printFor(" ", x2 - x1, color_bg);
  }
}

void cursorHide() { printf("\e[?;25;l"); }