#include "types.h"
#include "views.h"
#include "graphics.h"
#include "termiosWrapper.h"
#include "uiElements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* currentView = "КАТАЛОГ";
film *currentFilm = NULL;
film *firstFilm = NULL;

// https://github.com/pikvic/fefu2022/blob/main/cinema.md
// ☆★
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
// https://en.wikipedia.org/wiki/ANSI_escape_code

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

void redrawCatalogue() {
  goToPoint(0, 0);
  fillBackground();
  
	// Header
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);
  goToPoint(4, 2);
  printBold("КАТАЛОГ", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);

  drawRectWithShadow(3, 10, 27, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);
	drawRectWithShadow(54, 10, 78, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);
	
	drawRectWithShadow(28, 8, 53, 27, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);

  goToPoint(5, 12);
  printBold((currentFilm->previous)->title, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  goToPoint(56, 12);
  printBold((currentFilm->next)->title, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  
  goToPoint(30, 10);
  printBold(currentFilm->title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(28, 13);
  printFor("━", 53-28+1, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);

	uiElement* r1 = uiInit_button(29, 19, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Подробнее...");
  
	uiElement* r2 = uiInit_button(29, 22, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Добавить в избранное");

  r1->next = r2;
  r2->next = r1;

  r1->focus(r1, &currentView);
}

int main(void) {
  system("clear");
  goToPoint(0, 0);

  
  FILE *fin_films = fopen("films.txt", "rt");
  while (!feof(fin_films)) {
    film *t = (film *)malloc(sizeof(film));
    t->title = fgetsFlex(fin_films, 128);
    fscanf(fin_films, "%i\n", &(t->year));
    t->country = fgetsFlex(fin_films, 128);
    t->genre = fgetsFlex(fin_films, 128);
    fscanf(fin_films, "%lf\n", &(t->rating));
		//printf("%s  //  ", t->title);
    if (currentFilm == NULL) {
      firstFilm = t;
      currentFilm = t;
    }
    else {
      currentFilm->next = t;
      t->previous = currentFilm;
      currentFilm = t;
    }
  }
	fclose(fin_films);

  firstFilm->previous = currentFilm;
  currentFilm->next = firstFilm;
  currentFilm = firstFilm;

/*
  
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
  goToPoint(0, 0);
  fillBackground();
  cursorHide();

	// Header
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);
  goToPoint(4, 2);
  printFm("РЕГИСТРАЦИЯ/ВХОД", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	
  goToPoint(8, 9);
  printFm("Используйте [TAB] для навигации", COLOR_BACKGROUND_APP, COLOR_TEXT_BACK);

	drawRectWithShadow(0, 14, VIEWPORT_WIDTH-1, 20, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 0);
	
	drawRectWithShadow(48, 8, 75, 26, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);
	
  goToPoint(54, 9);
  printBold("Логин", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	uiElement* t1 = uiInit_textInput(50, 10, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	
  goToPoint(54, 14);
  printBold("Пароль", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	uiElement* t2 = uiInit_textInput(50, 15, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	
  goToPoint(54, 19);
  printBold("Номер карты", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	uiElement* t3 = uiInit_textInput(50, 20, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 57);

	uiElement* t4 = uiInit_button(50, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Назад");
	
	uiElement* t5 = uiInit_button(63, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Создать");

	t1->next = t2;
	t2->next = t3;
	t3->next = t4;
	t4->next = t5;
	t5->next = t1;
		
	t1->previous = t5;
	t2->previous = t1;
	t3->previous = t2;
	t4->previous = t3;
	t5->previous = t4;

	// t1->focus(t1);
	
	// kind of a nice design
  // goToPoint(54, 12);
  // printFm(" TEST ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(52, 13);
  // printFm("12345678901234567890", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);


  sleep(1);

  redrawCatalogue();
	/*
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
	*/
	
  // return 0;
  // https://stackoverflow.com/questions/10463201/getch-and-arrow-codes

	char c = '1';
	c = getch();
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
  printf("CODE: %i", c);
  return 0;
}