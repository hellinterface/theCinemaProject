#include <stdio.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define COLOR_BACKGROUND_HIGHLIGHT "160;140;240"
#define COLOR_BACKGROUND_FRONT "120;100;200"
#define COLOR_BACKGROUND_BACK "64;51;117"
#define COLOR_BACKGROUND_APP "20;0;40"
#define COLOR_TEXT_BACK "10;10;30"
#define COLOR_TEXT_FRONT "255;255;255"
#define COLOR_SHADOW_FRONT "60;40;120"
#define COLOR_SHADOW_BACK "33;26;66"

void goToPoint(int x, int y) { printf("\e[%d;%dH", (y), (x)); }

void fillLine(char *color_bg) {
  printf("\e[48;2;%s;38;2;" COLOR_TEXT_BACK "m", color_bg);
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

void printFor(char *str, int n, char *color_bg, char *color_fg) {
  printf("\e[48;2;%s;38;2;%sm", color_bg, color_fg);
  for (int i = 0; i < n; i++) {
    printf("%s", str);
  }
  printf("\e[0m");
  // printf("E\e[0m\n");
}

void printShadow(int x1, int y1, int x2, char *color_bg, char *color_fg) {
  goToPoint(x1, y1);
  printFor("▀", x2 + 1 - x1, color_bg, color_fg);
  printf("\e[0m");
  // printf("E\e[0m\n");
}

void printFm(char *str, char *color_bg, char *color_fg) {
  printf("\e[48;2;%s;38;2;%sm", color_bg, color_fg);
  printf("%s", str);
  printf("\e[0m");
  // printf("E\e[0m\n");
}

void printBold(char *str, char *color_bg, char *color_fg) {
  printf("\e[48;2;%s;38;2;%s;1m", color_bg, color_fg);
  printf("%s", str);
  printf("\e[0m");
}

void drawInputBox(int x1, int y1, int x2, char *color_bg, char *color_fg) {
  // printf("X");
  if (x1 > x2) {
    int temp = x1;
    x1 = x2;
    x2 = temp;
  }
  x2++;
  goToPoint(x1, y1 + 0);
  printFor("▄", x2 - x1, color_bg, color_fg);
  goToPoint(x1, y1 + 1);
  printFor(" ", x2 - x1, color_fg, color_bg);
  goToPoint(x1, y1 + 2);
  printFor("▀", x2 - x1, COLOR_SHADOW_FRONT, color_fg);
}

void drawRect(int x1, int y1, int x2, int y2, char *color_fill) {
  // printf("X");
  if (x1 > x2) {
    int temp = x1;
    x1 = x2;
    x2 = temp;
  }
  if (y1 > y2) {
    int temp = y1;
    y1 = y2;
    y2 = temp;
  }
  x2++;
  y2++;
  for (int a = y1; a < y2; a++) {
    goToPoint(x1, a);
    // printf("A");
    printFor(" ", x2 - x1, color_fill, COLOR_TEXT_BACK);
  }
}

/*
	Рисует прямоугольник с тенью.
	x1, y1 - координаты левого верхнего угла
	x2, y2 - координаты правого нижнего угла
	color_fill - цвет заливки прямоугольника
	color_bg - цвет заливки за прямоугольником (нужно для отрисовки тени)
	color_shadow - цвет тени
*/
void drawRectWithShadow(int x1, int y1, int x2, int y2, char *color_fill, char *color_bg, char *color_shadow, int needsDecorations) {
	drawRect(x1, y1, x2, y2, color_fill);
  printShadow(x1, y2+1, x2, color_bg, color_shadow);
	if (needsDecorations != 0) {
  goToPoint(x1, y1);
  printFm("━━┛", color_fill, color_bg);
  goToPoint(x2-2, y2);
  printFm("┏━━", color_fill, color_bg);
	}
}

void cursorHide() { printf("\e[?;25;l"); }