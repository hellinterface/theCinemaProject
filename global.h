#include <stdio.h>

// Ширина и высота экрана (окна, в котором будет происходить рисовка)
#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

// Цвета в текстовом формате, которые можно отображать в консоли.
#define COLOR_BACKGROUND_HIGHLIGHT "160;140;240"
#define COLOR_BACKGROUND_DIALOG "140;120;220"
#define COLOR_BACKGROUND_FRONT "120;100;200"
#define COLOR_BACKGROUND_BACK "64;51;117"
#define COLOR_BACKGROUND_APP "20;0;40"
#define COLOR_TEXT_BACK "10;10;30"
#define COLOR_TEXT_FRONT "255;255;255"
#define COLOR_SHADOW_FRONT "60;40;120"
#define COLOR_SHADOW_BACK "33;26;66"

typedef struct user user; // Тип "Пользователь"
typedef struct film film; // Тип "Фильм"
typedef struct navPointRoll navPoint; // Тип "Точка перехода"

// Тип "Пользователь"
struct user {
  char *name;       // min = 3, max = 20
  char *password;   // min = 6, max = 20
  char *cardNumber; // 16 digits
  int favListLength;
  int isAdmin; // 1 or 0
};

// Тип "Фильм"
struct film {
  char *title; // max = 64
  int year;
  char *country; // max = 64
  char *genre;   // max = 64
  double rating;
  int isFavourite;
  film *previous;
  film *next;
};

// Тип "Точка перехода"
struct navPointRoll {
	char* title;
	void (*switchTo)();
	navPoint *previous;
	navPoint *next;
};

int switchToFavorite_next();
int switchToFavorite_previous();
void goToPreviousPage();
void onLogIn();
void logOff();
void resetAllTextInputsBesidesCurrent();
void drawAddFilmView();
void appExit();
void drawLogInView();
void drawSignUpView();
void drawSettings();
void drawDetailedView();
void drawFavourites();
void enterHeaderSwitcher();
void toggleFavouriteStateOfCurrentFilm();
void drawCatalogue();
void drawHeader();
char *fgetsPlus(FILE *file, int len);
int strlenPlus(char *str);