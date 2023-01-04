#include "graphics.h"
#include "termiosWrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VIEWPORT_WIDTH 80
#define VIEWPORT_HEIGHT 30

#define COLOR_BACKGROUND_FRONT "120;100;200"
#define COLOR_BACKGROUND_BACK "64;51;117"
#define COLOR_BACKGROUND_APP "20;0;40"
#define COLOR_TEXT_BACK "10;10;30"
#define COLOR_TEXT_FRONT "255;255;255"
#define COLOR_SHADOW_FRONT "60;40;120"
#define COLOR_SHADOW_BACK "33;26;66"

// https://github.com/pikvic/fefu2022/blob/main/cinema.md
// ☆★
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
// https://en.wikipedia.org/wiki/ANSI_escape_code

typedef struct {
  char* name;      // min = 3, max = 20
  char* password;   // min = 6, max = 20
  char* cardNumber; // 16 digits
  int favListLength;
  int isAdmin; // 1 or 0
} user;

typedef struct {
  char *title; // max = 64
  int year;
  char *country; // max = 64
  char *genre;   // max = 64
  double rating;
} film;

typedef struct {
  film *previous;
  film *self;
  film *next;
} filmListItem;

char *fgetsFlex(FILE *file, int len) {
  char *returnValue;
  char str1[len], str2[len], currentChar;
  fgets(str1, len, file);
  for (int i = 0; i < len; i++) {
    currentChar = str1[i];
    if (currentChar != '\n') {
      str2[i] = currentChar;
    } else {
      // printf("D>%i", i);
      returnValue = (char *)malloc(i);
      for (int n = 0; n < i; n++) {
        returnValue[n] = str2[n];
      }
      break;
    }
  }
  return returnValue;
}
char* readLogin(int x, int y, char *color_bg, char *color_text, int low_limit, int high_limit, int charlim_low, int charlim_high) {
	int currentLength = 0;
	char c = '1';
  char *str = (char *)malloc(high_limit);
  for (int i = 0; i < 20; i++) {
    str[i] = ' ';
  }
  while (1 == 1) {
    c = getch();
    // printf("%i %c / ", c, c);
		if (currentLength < high_limit) {
	    if (c >= charlim_low && c <= charlim_high) {
	      str[currentLength] = c;
	      currentLength++;
      	goToPoint(x, y);
      	printFm(str, color_bg, color_text);
				if (currentLength < high_limit) {
      	goToPoint(x+currentLength, y);
      	printFm("|", color_bg, color_text);
				}
	    }
		}
	  if (c == 127) { // Backspace
	    str[currentLength - 1] = ' ';
	    currentLength--;
      goToPoint(x, y);
      printFm(str, color_bg, color_text);
      goToPoint(x+currentLength, y);
      printFm("|", color_bg, color_text);
	  }
		else if(c == 10) { // Enter
      if (currentLength > low_limit-1) {
        break;
      }
    }
    // printf("%i %c / ", c, c);
  }
	//strcpy(target, str);
	return str;
}

int main(void) {
  system("clear");
  printf("");
  fillBackground();
  goToPoint(0, 0);
  fillLine(COLOR_BACKGROUND_FRONT);
  fillLine(COLOR_BACKGROUND_FRONT);
  fillLine(COLOR_BACKGROUND_FRONT);
  printShadow(0, 4, VIEWPORT_WIDTH - 1, COLOR_BACKGROUND_APP,
              COLOR_SHADOW_FRONT);
  cursorHide();

  goToPoint(4, 2);
  printFm("КАТАЛОГ", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);

  drawRect(4, 8, 26, 26, COLOR_BACKGROUND_BACK);
  printShadow(4, 26, 26, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK);

  drawRect(55, 8, 77, 26, COLOR_BACKGROUND_BACK);
  printShadow(55, 26, 77, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK);

  drawRect(27, 6, 54, 28, COLOR_BACKGROUND_FRONT);
  printShadow(27, 29, 54, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT);


  goToPoint(8, 12);

  goToPoint(0, 31);
/*
  FILE *fin_films = fopen("films.txt", "rt");
  while (!feof(fin_films)) {
    film *t = (film *)malloc(sizeof(film));
    t->title = fgetsFlex(fin_films, 128);
    fscanf(fin_films, "%i\n", &(t->year));
    t->country = fgetsFlex(fin_films, 128);
    t->genre = fgetsFlex(fin_films, 128);
    fscanf(fin_films, "%lf\n", &(t->rating));
		printf("%s  //  ", t->title);
		break;
  }
	fclose(fin_films);

  
  FILE *fin_users = fopen("users.txt", "rt");
  while (!feof(fin_users)) {
    user *temp = (user *)malloc(sizeof(user));
    fgets(temp->name, 20, fin_users);
    fgets(temp->password, 20, fin_users);
    fgets(temp->cardNumber, 16, fin_users);
    fscanf(fin_users, " %i\n", &(temp->isAdmin));
		printf("%s", temp->password);
		break;
  }
  
	fclose(fin_users);
  */
	// system("clear");
 //  // printf("");
 //  goToPoint(0, 0);
 //  fillBackground();
 //  goToPoint(0, 0);
 //  fillLine(COLOR_BACKGROUND_FRONT);
 //  fillLine(COLOR_BACKGROUND_FRONT);
 //  fillLine(COLOR_BACKGROUND_FRONT);
 //  printShadow(0, 4, VIEWPORT_WIDTH - 1, COLOR_BACKGROUND_APP,
 //              COLOR_SHADOW_FRONT);
 //  cursorHide();

 //  goToPoint(4, 2);
 //  printFm("ВХОД", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  
 //  drawRect(0, 12,VIEWPORT_WIDTH - 1 , 18, COLOR_BACKGROUND_BACK);
 //  printShadow(4, 26, 26, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK);

 
  	system("clear");
  // printf("");
  goToPoint(0, 0);
  fillBackground();
  cursorHide();
	
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);

  goToPoint(4, 2);
  printFm("РЕГИСТРАЦИЯ/ВХОД", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);

	drawRectWithShadow(0, 14, VIEWPORT_WIDTH-1, 18, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 0);
	
	drawRectWithShadow(48, 10, 75, 26, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);
	
  goToPoint(54, 11);
  printBold("Логин", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	drawInputBox(50, 12, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	
  goToPoint(54, 16);
  printBold("Пароль", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	drawInputBox(50, 17, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	
  goToPoint(54, 21);
  printBold("Номер карты", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	drawInputBox(50, 22, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);

	// kind of a nice design
  // goToPoint(54, 12);
  // printFm(" TEST ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(52, 13);
  //printFm("12345678901234567890", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

  
	FILE *fout_users = fopen("users.txt", "a");

	user* t = (user*)malloc(sizeof(user));

	t->name = readLogin(52, 13, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, 3, 20, 48, 122);

  goToPoint(30, 19);
  printFm("Password:", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	
	t->password = readLogin(30, 20, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, 3, 20, 48, 122);

  goToPoint(30, 22);
  printFm("Card:", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	
	t->cardNumber = readLogin(30, 23, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, 16, 16, 48, 57);

	t->isAdmin = 0;

	printf(">>> %s", t->password);

	fprintf(fout_users, "%s\n%s\n%s\n%i\n", t->name, t->password, t->cardNumber, t->isAdmin);

	fclose(fout_users);
	
  return 0;
  // https://stackoverflow.com/questions/10463201/getch-and-arrow-codes

	char c = '1';
  if (c == 27) {
    c = getch();
    c = getch();
    if (c == 'A') {
      // UP
    } else if (c == 'B') {
      // DOWN
    } else if (c == 'C') {
      // RIGHT
    } else if (c == 'D') {
      // LEFT
    }
  }
  // printf("CODE: %c", c);
  return 0;
}