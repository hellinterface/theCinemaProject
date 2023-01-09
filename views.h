#include <stdio.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define COLOR_BACKGROUND_HIGHLIGHT "160;140;240"
#define COLOR_BACKGROUND_DIALOG "140;120;220"
#define COLOR_BACKGROUND_FRONT "120;100;200"
#define COLOR_BACKGROUND_BACK "64;51;117"
#define COLOR_BACKGROUND_APP "20;0;40"
#define COLOR_TEXT_BACK "10;10;30"
#define COLOR_TEXT_FRONT "255;255;255"
#define COLOR_SHADOW_FRONT "60;40;120"
#define COLOR_SHADOW_BACK "33;26;66"

void enterHeaderSwitcher();
void toggleFavouriteStateOfCurrentFilm();
void drawCatalogue();
void drawHeader();
char *fgetsPlus(FILE *file, int len);
int strlenPlus(char *str);