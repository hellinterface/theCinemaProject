#include "types.h"
#include "views.h"
#include "graphics.h"
#include "termiosWrapper.h"
#include "uiElements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

char* currentView = "КАТАЛОГ";
film* currentFilm = NULL;
film* firstFilm = NULL;

user* userDatabase[64];
int currentUserIndex;
int totalUsers;

uiElement* input_login_username;
uiElement* input_login_password;
uiElement* button_login_ok;
uiElement* button_login_toSignUpPage;

uiElement* input_signup_username;
uiElement* input_signup_password;
uiElement* input_signup_cardNumber;
uiElement* button_signup_ok;
uiElement* button_signup_toLogInPage;

uiElement* button_catalogue_toDetailsPage;
uiElement* button_catalogue_toggleFavouriteState;

uiElement* button_details_toCatalogue;
uiElement* button_details_toggleFavouriteState;

uiElement* button_settings_goBack;
uiElement* button_settings_toSignUpPage;

uiElement* button_admin_addFilm;
uiElement* button_admin_removeFilm;

uiElement* button_addFilm_toCatalogue;
uiElement* button_addFilm_ok;

navPointRoll* navPoint_currentLogInOrSignOut;
navPointRoll* navPoint_catalogue;
navPointRoll* navPoint_favourites;
navPointRoll* navPoint_details;
navPointRoll* navPoint_settings;
navPointRoll* navPoint_logout;
navPointRoll* navPoint_exit;
navPointRoll* navPoint_addFilm;

navPointRoll* currentNavPoint;
navPointRoll* previousNavPoint;
navPointRoll* headerSwitcher_selectedNavPoint;


void initNavPoints() {
	navPoint_currentLogInOrSignOut = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_catalogue = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_details = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_favourites = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_settings = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_logout = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_exit = (navPointRoll*)malloc(sizeof(navPointRoll));
	navPoint_addFilm = (navPointRoll*)malloc(sizeof(navPointRoll));

	navPoint_currentLogInOrSignOut->switchTo = NULL;
	navPoint_catalogue->switchTo = NULL;
	navPoint_favourites->switchTo = NULL;
	navPoint_details->switchTo = NULL;
	navPoint_settings->switchTo = NULL;
	navPoint_logout->switchTo = NULL;
	navPoint_exit->switchTo = NULL;
	navPoint_addFilm->switchTo = NULL;
	
	navPoint_currentLogInOrSignOut->title = "ВХОД";
	navPoint_catalogue->title = "КАТАЛОГ";
	navPoint_favourites->title = "ИЗБРАННОЕ";
	navPoint_details->title = "ДЕТАЛИ";
	navPoint_settings->title = "НАСТРОЙКИ";
	navPoint_logout->title = "ВЫЙТИ ИЗ АККАУНТА";
	navPoint_exit->title = "ЗАКРЫТЬ ПРИЛОЖЕНИЕ";
	navPoint_addFilm->title = "ДОБАВИТЬ ФИЛЬМ";
	
	navPoint_currentLogInOrSignOut->next = navPoint_exit;
	navPoint_exit->next = navPoint_currentLogInOrSignOut;
	
	navPoint_currentLogInOrSignOut->previous = navPoint_exit;
	navPoint_exit->previous = navPoint_currentLogInOrSignOut;
}

void navPoints_normalMode() {
	navPoint_catalogue->next = navPoint_favourites;
	navPoint_favourites->next = navPoint_details;
	navPoint_details->next = navPoint_settings;
	navPoint_settings->next = navPoint_logout;
	navPoint_logout->next = navPoint_exit;
	navPoint_exit->next = navPoint_catalogue;
	
	navPoint_catalogue->previous = navPoint_exit;
	navPoint_favourites->previous = navPoint_catalogue;
	navPoint_details->previous = navPoint_favourites;
	navPoint_settings->previous = navPoint_details;
	navPoint_logout->previous = navPoint_settings;
	navPoint_exit->previous = navPoint_logout;
	
	if (userDatabase[currentUserIndex]->isAdmin == 1) {
		navPoint_exit->next = navPoint_addFilm;
		navPoint_addFilm->next = navPoint_catalogue;
		
		navPoint_catalogue->previous = navPoint_addFilm;
		navPoint_addFilm->previous = navPoint_exit;
	}
}

// https://github.com/pikvic/fefu2022/blob/main/cinema.md
// ☆★
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
// https://en.wikipedia.org/wiki/ANSI_escape_code

char *fgetsPlus(FILE *file, int len) {
  char *returnValue;
  char str1[len], str2[len], currentChar;
  fgets(str1, len, file);
  for (int i = 0; i < len; i++) {
    currentChar = str1[i];
    if (currentChar != '\n') {
      str2[i] = currentChar;
    } else {
      // printf("D>%i", i);
      returnValue = (char *)malloc(i+1);
      for (int n = 0; n < i; n++) {
        returnValue[n] = str2[n];
      }
			returnValue[i] = '\0';
      break;
    }
  }
  return returnValue;
}

int strlenPlus(char *str) {
	int result = 0;
	int total = strlen(str);
	int i = 0;
	while (i < total) {
		if (str[i] < 0) i++;
		result++;
		i++;
	}
	return result;
}

void drawHeader(char *title) {
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);
  goToPoint(6, 2);
  printBold(title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	char username[21];
	strcpy(username, userDatabase[currentUserIndex]->name);
  goToPoint(VIEWPORT_WIDTH-4-strlen(username), 2);
  printFm(username, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
}

void drawHeaderSwitcher() {
	char c = '1';
  while (1 == 1) {
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH/3, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);
  goToPoint(3, 1);
  printFm(headerSwitcher_selectedNavPoint->previous->title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(3, 2);
  printBold(" > ", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  printBold(headerSwitcher_selectedNavPoint->title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(3, 3);
  printFm(headerSwitcher_selectedNavPoint->next->title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
    c = getch();
		if (c == 10) { // Enter
			// switchToSelected
			break;
    }
		else if (c == 9) { // Tab
			currentNavPoint->switchTo();
			break;
		}
    else if (c == 27) {
      c = getch();
      c = getch();
      if (c == 'A' || c == 'D') {
        // UP OR LEFT
				headerSwitcher_selectedNavPoint = headerSwitcher_selectedNavPoint->previous;
      } 
			else if (c == 'B' || c == 'C') {
        // DOWN OR RIGHT
				headerSwitcher_selectedNavPoint = headerSwitcher_selectedNavPoint->next;
      }
    }
  }
}

void enterHeaderSwitcher() {
	headerSwitcher_selectedNavPoint = currentNavPoint;
	drawHeaderSwitcher();
}

// Функция рисовки страницы регистрации
void drawSignUpView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	previousNavPoint = currentNavPoint;
	currentNavPoint = navPoint_currentLogInOrSignOut;
	currentNavPoint->title = "РЕГИСТРАЦИЯ";
  currentView = "РЕГИСТРАЦИЯ";
	drawHeader("РЕГИСТРАЦИЯ");
	
  goToPoint(8, 9);
  printFm("Используйте [TAB] для навигации", COLOR_BACKGROUND_APP, COLOR_TEXT_BACK);

	drawRectWithShadow(0, 14, VIEWPORT_WIDTH-1, 20, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 0);
	
	drawRectWithShadow(48, 8, 75, 26, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);
	
  goToPoint(54, 9);
  printBold("Имя пользователя", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(54, 14);
  printBold("Пароль", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(54, 19);
  printBold("Номер карты", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);

	input_signup_username->show(input_signup_username);
	input_signup_password->show(input_signup_password);
	input_signup_cardNumber->show(input_signup_cardNumber);
	button_signup_ok->show(button_signup_ok);
	button_signup_toLogInPage->show(button_signup_toLogInPage);

	input_signup_username->focus(input_signup_username, &currentView);
}

void buttonPress_dialogWindowOk(uiElement *element) {
	drawHeader(currentView);
	free(element);
}

// Функция рисовки диалогового окна
void drawOverlay_dialogWindow(char *message) {
	int width = strlenPlus(message)+9;
	if (width > VIEWPORT_WIDTH/3*2) {
		width = VIEWPORT_WIDTH/3*2;
	}

  goToPoint(4, 4);

	int x1 = (VIEWPORT_WIDTH)-width-5;
	int x2 = (VIEWPORT_WIDTH);
	int y1 = 0;
	int y2 = 3;
	
	drawRectWithShadow(x1, y1, x2, y2, COLOR_BACKGROUND_DIALOG, COLOR_SHADOW_FRONT, COLOR_BACKGROUND_DIALOG, 0);
  goToPoint(x1+3, y1+2);
  //printFm(message, COLOR_BACKGROUND_DIALOG, COLOR_BACKGROUND_APP);
  printFmLimited(message, VIEWPORT_WIDTH/3*2-9, x1+3, y1+2, COLOR_BACKGROUND_DIALOG, COLOR_BACKGROUND_APP);
	
	uiElement* t1 = uiInit_button(x2-5-3, y1+1, x2-3, COLOR_BACKGROUND_DIALOG, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, "OK");

	// is in dialog window -> disable tab
	
	t1->next = t1;
	t1->previous = t1;
	t1->onInput = buttonPress_dialogWindowOk;
	t1->show(t1);
	t1->focus(t1, &currentView);
}

// Функция рисовки страницы входа
void drawLogInView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	previousNavPoint = currentNavPoint;
	currentNavPoint = navPoint_currentLogInOrSignOut;
	currentNavPoint->title = "ВХОД";
  currentView = "ВХОД";
	drawHeader("ВХОД");
	
  goToPoint(8, 9);
  printFm("Используйте [TAB] для навигации", COLOR_BACKGROUND_APP, COLOR_TEXT_BACK);

	drawRectWithShadow(0, 14, VIEWPORT_WIDTH-1, 20, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 0);
	
	drawRectWithShadow(48, 8, 75, 26, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);
	
  goToPoint(54, 9);
  printBold("Имя пользователя", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(54, 14);
  printBold("Пароль", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);

	input_login_username->show(input_login_username);
	input_login_password->show(input_login_password);
	button_login_ok->show(button_login_ok);
	button_login_toSignUpPage->show(button_login_toSignUpPage);

	input_login_username->focus(input_login_username, &currentView);
}

// Функция рисовки страницы каталога
void drawCatalogue() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	previousNavPoint = currentNavPoint;
	currentNavPoint = navPoint_catalogue;
  currentView = "КАТАЛОГ";
	drawHeader("КАТАЛОГ");
	
	navPoints_normalMode();

  drawRectWithShadow(3, 10, 27, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);
	drawRectWithShadow(54, 10, 78, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);

	drawRectWithShadow(5, 19, 25, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(5, 22, 25, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(56, 19, 75, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(56, 22, 75, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(28, 8, 53, 27, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);

  goToPoint(5, 12);
  printBoldLimited((currentFilm->previous)->title, 27-3-4, 5, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  goToPoint(56, 12);
  printBoldLimited((currentFilm->next)->title, 27-3-4, 56, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  
  goToPoint(30, 10);
  printBoldLimited(currentFilm->title, 27-3-4, 30, 10, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(28, 13);
  printFor("━", 53-28+1, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	
	char ratingString[4];
	snprintf(ratingString, 4, "%f", currentFilm->rating);
	
  goToPoint(35, 15);
  printFm("Рейтинг: ", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  printFm(ratingString, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(31, 16);
	int ratingWhole = floor(currentFilm->rating);
  printFor("★ ", ratingWhole, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
  printFor("☆ ", 10-ratingWhole, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	

	char* text = "Убрать из избранного";
	if (currentFilm->isFavourite == 0) {
		text = "Добавить в избранное";
	}
	int textLength = strlenPlus(text);
	button_catalogue_toggleFavouriteState->value = text;
	button_catalogue_toggleFavouriteState->length = textLength;
	
	button_catalogue_toDetailsPage->show(button_catalogue_toDetailsPage);
	button_catalogue_toggleFavouriteState->show(button_catalogue_toggleFavouriteState);

	if (userDatabase[currentUserIndex]->isAdmin) {
		//button_admin_removeFilm->show(button_admin_removeFilm);
	}
	
  button_catalogue_toDetailsPage->focus(button_catalogue_toDetailsPage, &currentView);
}

// Функция рисовки страницы избранного
void drawFavourites() {

  int found = 1;
  currentFilm = firstFilm->next;
  while (currentFilm->isFavourite == 0) {
    if (firstFilm == currentFilm) {
      found = 0;
      break;
    }  
    currentFilm = currentFilm->next;
  }

  if (found == 0) {
    drawOverlay_dialogWindow("Фильмов в избранном нет");
  }
  
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	previousNavPoint = currentNavPoint;
	currentNavPoint = navPoint_favourites;
  currentView = "ИЗБРАННОЕ";
	drawHeader("ИЗБРАННОЕ");
	
	navPoints_normalMode();

  drawRectWithShadow(3, 10, 27, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);
	drawRectWithShadow(54, 10, 78, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);

	drawRectWithShadow(5, 19, 25, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(5, 22, 25, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(56, 19, 75, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(56, 22, 75, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(28, 8, 53, 27, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);

  film *previousFav = currentFilm->previous;
  while (previousFav->isFavourite == 0) {
    previousFav = previousFav->previous;
  }

  film *nextFav = currentFilm->next;
  while (nextFav->isFavourite == 0) {
    nextFav = nextFav->next;
  }
  
  goToPoint(5, 12);
  printBoldLimited((previousFav)->title, 27-3-4, 5, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  goToPoint(56, 12);
  printBoldLimited((nextFav)->title, 27-3-4, 56, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  
  goToPoint(30, 10);
  printBoldLimited(currentFilm->title, 27-3-4, 30, 10, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(28, 13);
  printFor("━", 53-28+1, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	
	char ratingString[4];
	snprintf(ratingString, 4, "%f", currentFilm->rating);
	
  goToPoint(35, 15);
  printFm("Рейтинг: ", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  printFm(ratingString, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(31, 16);
	int ratingWhole = floor(currentFilm->rating);
  printFor("★ ", ratingWhole, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
  printFor("☆ ", 10-ratingWhole, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP);
	

	char* text = "Убрать из избранного";
	if (currentFilm->isFavourite == 0) {
		text = "Добавить в избранное";
	}
	int textLength = strlenPlus(text);
	button_catalogue_toggleFavouriteState->value = text;
	button_catalogue_toggleFavouriteState->length = textLength;
	
	button_catalogue_toDetailsPage->show(button_catalogue_toDetailsPage);
	button_catalogue_toggleFavouriteState->show(button_catalogue_toggleFavouriteState);

	if (userDatabase[currentUserIndex]->isAdmin) {
		//button_admin_removeFilm->show(button_admin_removeFilm);
	}
	
  button_catalogue_toDetailsPage->focus(button_catalogue_toDetailsPage, &currentView);
}

// Функция рисовки страницы детального просмотра
void drawDetailedView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	previousNavPoint = currentNavPoint;
	currentNavPoint = navPoint_details;
  currentView = "ДЕТАЛИ";
	drawHeader("ДЕТАЛИ");

	navPoints_normalMode();

	char ratingString[4];
	snprintf(ratingString, 4, "%f", currentFilm->rating);
	
  goToPoint(4, 6);
  printFm("Название фильма: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm(currentFilm->title, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

  goToPoint(4, 8);
  printFm("Рейтинг: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm(ratingString, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(4, 10);
  printFm("Страна: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm(currentFilm->country, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(4, 12);
  printFm("Жанры: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm(currentFilm->genre, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

	char* text = "Убрать из избранного";
	if (currentFilm->isFavourite == 0) {
		text = "Добавить в избранное";
	}
	int textLength = strlenPlus(text);
	button_details_toggleFavouriteState->value = text;
	button_details_toggleFavouriteState->length = textLength;

	button_details_toCatalogue->show(button_details_toCatalogue);
	button_details_toggleFavouriteState->show(button_details_toggleFavouriteState);
	
	button_details_toCatalogue->focus(button_details_toCatalogue, &currentView);
}

void buttonPress_login() {
	//drawOverlay_dialogWindow();
	int i = 0;
	int found = -1;
	char* inputValue_name = input_login_username->getValue(input_login_username);
	char* inputValue_password = input_login_password->getValue(input_login_password);
	while (userDatabase[i] != NULL) {
		if (strcmp(userDatabase[i]->name, inputValue_name) == 0) {
			found = i;
			currentUserIndex = i;
			if (strcmp(userDatabase[i]->password, inputValue_password) == 0) {
				drawCatalogue();
			}
			else {
				drawOverlay_dialogWindow("Неверный пароль.");
			}
			break;
		}
		else {
			i++;
			continue;
		}
	}
	if (found != -1) {
	}
	else {
		drawOverlay_dialogWindow("Пользователя с таким именем не существует.");
	}
}

void buttonPress_signup_createUser() {
	char *password = input_signup_password->getValue(input_signup_password);

	int passcheck_hasUppercase = 0;
	int passcheck_hasLowercase = 0;
	int passcheck_hasNumbers = 0;

	for (int i = 0; i < strlen(password); i++) {
		char c = password[i];
		if (passcheck_hasUppercase == 0 && c <= 57 && c >= 48) {
			passcheck_hasUppercase = 1;
		}
		if (passcheck_hasLowercase == 0 && c <= 122 && c >= 97) {
			passcheck_hasLowercase = 1;
		}
		if (passcheck_hasNumbers == 0 && c <= 122 && c >= 97) {
			passcheck_hasNumbers = 1;
		}
	}
	

	if (passcheck_hasUppercase == 0) {
		drawOverlay_dialogWindow("В пароле нет букв в верхнем регистре.");
	}
	else if (passcheck_hasLowercase == 0) {
		drawOverlay_dialogWindow("В пароле нет букв в нижнем регистре.");
	}
	else if (passcheck_hasNumbers == 0) {
		drawOverlay_dialogWindow("В пароле нет цифр.");
	}
	else {
		FILE *fout_users = fopen("users.txt", "a");
		user* t = (user*)malloc(sizeof(user));
		t->name = input_signup_username->getValue(input_signup_username);
		t->password = password;
		t->cardNumber = input_signup_cardNumber->getValue(input_signup_cardNumber);
		t->isAdmin = 0;
		fprintf(fout_users, "\n%s\n%s\n%s\n%i", t->name, t->password, t->cardNumber, t->isAdmin);
		fclose(fout_users);
		currentUserIndex = totalUsers;
		totalUsers++;
		drawOverlay_dialogWindow("Аккаунт создан.");
		drawCatalogue();
	}
	

	//t->name = input_signup_username->getValue(input_signup_username);
	//t->password = 
	//t->cardNumber = 
	//->isAdmin = 0;

	//printf(">>> %s", t->password);

	//fprintf(fout_users, "%s\n%s\n%s\n%i\n", t->name, t->password, t->cardNumber, t->isAdmin);

}

void readFilmList() {
  FILE *fin_films = fopen("films.txt", "rt");
  while (!feof(fin_films)) {
    film *t = (film *)malloc(sizeof(film));
    t->title = fgetsPlus(fin_films, 128);
    fscanf(fin_films, "%i\n", &(t->year));
    t->country = fgetsPlus(fin_films, 128);
    t->genre = fgetsPlus(fin_films, 128);
    fscanf(fin_films, "%lf\n", &(t->rating));
    t->isFavourite = 0;
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
}

void readUserList() {
  FILE *fin_users = fopen("users.txt", "rt");
	int i = 0;
  while (!feof(fin_users)) {
    user *temp = (user*)malloc(sizeof(user));
		temp->name = fgetsPlus(fin_users, 64);
		temp->password = fgetsPlus(fin_users, 64);
		temp->cardNumber = fgetsPlus(fin_users, 64);
    fscanf(fin_users, " %i\n", &(temp->isAdmin));
		userDatabase[i] = temp;
		i++;
  }
	totalUsers = i;
	fclose(fin_users);
}

film* findFilm(char* title) {
	film* t = firstFilm;
	if (strcmp(t->title, title) == 0) return t;
	t = t->next;
	while (t != firstFilm) {
		if (strcmp(t->title, title) == 0) return t;
		t = t->next;
	}
	return NULL;
}

void readFavoriteList(int userID) {
	char filename[80] = "favorite_";
	strcat(filename, userDatabase[userID]->name);
	strcat(filename, ".txt");
  FILE *fin_favouriteFilms = fopen(filename, "rt");
  while (!feof(fin_favouriteFilms)) {
    film *t = (film*)malloc(sizeof(film));
    char  *title = fgetsPlus(fin_favouriteFilms, 128);
		int temp1;
		double temp2;
    fscanf(fin_favouriteFilms, "%i\n", &temp1);
    fgetsPlus(fin_favouriteFilms, 128);
    fgetsPlus(fin_favouriteFilms, 128);
    fscanf(fin_favouriteFilms, "%lf\n", &temp2);
		film *foundFilm = findFilm(title);
    if (foundFilm != NULL) {
    	foundFilm->isFavourite = 1;
    }
  }
	fclose(fin_favouriteFilms);
}

void toggleFavouriteStateOfCurrentFilm() {
	char* text = "Убрать из избранного";
	if (currentFilm->isFavourite == 1) {
		currentFilm->isFavourite = 0;
		text = "Добавить в избранное";
	}
	else {
		currentFilm->isFavourite = 1;
	}
	int textLength = strlenPlus(text);
	if (strcmp(currentView, "ДЕТАЛИ") == 0) {
		button_details_toggleFavouriteState->value = text;
		button_details_toggleFavouriteState->length = textLength;
		button_details_toggleFavouriteState->focus(button_details_toggleFavouriteState, &currentView);
	}
	else if (strcmp(currentView, "КАТАЛОГ") == 0) {
		button_catalogue_toggleFavouriteState->value = text;
		button_catalogue_toggleFavouriteState->length = textLength;
		button_catalogue_toggleFavouriteState->focus(button_catalogue_toggleFavouriteState, &currentView);
	}
}

void linkAllElements() {

	// Вход //////////////////////////////////////////////////////
	
	input_login_username = uiInit_textInput(50, 10, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	input_login_password = uiInit_textInput(50, 15, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	button_login_ok = uiInit_button(50, 19, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Войти");
	button_login_toSignUpPage = uiInit_button(50, 22, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Создать аккаунт");
	
	button_login_ok->onInput = buttonPress_login;
	button_login_toSignUpPage->onInput = drawSignUpView;
	
	input_login_username->next = input_login_password;
	input_login_password->next = button_login_ok;
	button_login_ok->next = button_login_toSignUpPage;
	button_login_toSignUpPage->next = input_login_username;

	input_login_username->previous = button_login_toSignUpPage;
	input_login_password->previous = input_login_username;
	button_login_ok->previous = input_login_password;
	button_login_toSignUpPage->previous = button_login_ok;

	// Регистрация //////////////////////////////////////////////////////
	
	input_signup_username = uiInit_textInput(50, 10, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	input_signup_password = uiInit_textInput(50, 15, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 48, 122);
	input_signup_cardNumber = uiInit_textInput(50, 20, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 16, 16, 48, 57);
	button_signup_toLogInPage = uiInit_button(50, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Назад");
	button_signup_ok = uiInit_button(63, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Создать");
	
	button_signup_ok->onInput = buttonPress_signup_createUser;
	button_signup_toLogInPage->onInput = drawLogInView;
	
	input_signup_username->next = input_signup_password;
	input_signup_password->next = input_signup_cardNumber;
	input_signup_cardNumber->next = button_signup_ok;
	button_signup_ok->next = button_signup_toLogInPage;
	button_signup_toLogInPage->next = input_signup_username;
	
	input_signup_username->previous = button_signup_toLogInPage;
	input_signup_password->previous = input_signup_username;
	input_signup_cardNumber->previous = input_signup_password;
	button_signup_ok->previous = input_signup_cardNumber;
	button_signup_toLogInPage->previous = button_signup_ok;
	
	// Каталог //////////////////////////////////////////////////////
	
	button_catalogue_toDetailsPage = uiInit_button(29, 19, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Подробнее...");
	button_catalogue_toggleFavouriteState = uiInit_button(29, 22, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Добавить в избранное");
	
	button_catalogue_toDetailsPage->next = button_catalogue_toggleFavouriteState;
	button_catalogue_toggleFavouriteState->next = button_catalogue_toDetailsPage;
	
	button_catalogue_toDetailsPage->previous = button_catalogue_toggleFavouriteState;
	button_catalogue_toggleFavouriteState->previous = button_catalogue_toDetailsPage;

	button_catalogue_toDetailsPage->onInput = drawDetailedView;
	button_catalogue_toggleFavouriteState->onInput = toggleFavouriteStateOfCurrentFilm;
	
	// IF ADMIN
	/*
	button_catalogue_toggleFavouriteState->next = button_admin_removeFilm;
	button_admin_removeFilm->next = button_admin_addFilm;
	button_admin_addFilm->next = button_catalogue_toDetailsPage;
	
	button_admin_removeFilm->previous = button_catalogue_toggleFavouriteState;
	button_admin_addFilm->previous = button_admin_removeFilm;
	button_catalogue_toDetailsPage->previous = button_admin_addFilm;
	*/
	// Детальный просмотр //////////////////////////////////////////////////////
	
	button_details_toCatalogue = uiInit_button(29, 19, 52, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Назад");
	button_details_toggleFavouriteState = uiInit_button(29, 22, 52, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Добавить в избранное");
	
	button_details_toCatalogue->next = button_details_toggleFavouriteState;
	button_details_toggleFavouriteState->next = button_details_toCatalogue;
	
	button_details_toCatalogue->previous = button_details_toggleFavouriteState;
	button_details_toggleFavouriteState->previous = button_details_toCatalogue;

	button_details_toCatalogue->onInput = drawCatalogue;
	button_details_toggleFavouriteState->onInput = toggleFavouriteStateOfCurrentFilm;
	
	// Настройки //////////////////////////////////////////////////////
	
/*
	button_settings_goBack->next = button_settings_toSignUpPage;
	button_settings_toSignUpPage->next = button_settings_goBack;
	
	button_settings_goBack->previous = button_settings_toSignUpPage;
	button_settings_toSignUpPage->previous = button_settings_goBack;
	
	// Добавление фильма //////////////////////////////////////////////////////
	
	button_addFilm_toCatalogue->next = button_addFilm_ok;
	button_addFilm_ok->next = button_addFilm_toCatalogue;
	
	button_addFilm_toCatalogue->previous = button_addFilm_ok;
	button_addFilm_ok->previous = button_addFilm_toCatalogue;
*/
}

int enterKeyDebug() {
	while (1 == 1) {
		char c = '1';
		c = getch();
  	printf("CODE: %i\n", c);
	}
}

int main(void) {
  setlocale(LC_ALL, "ru-RU");
  system("clear");
  goToPoint(0, 0);

	initNavPoints();

	// enterKeyDebug();
	
	linkAllElements();
	
	readFilmList();
  firstFilm->previous = currentFilm;
  currentFilm->next = firstFilm;
  currentFilm = firstFilm;

	readUserList();

  system("clear");
  cursorHide();

	readFavoriteList(currentUserIndex);

  //drawCatalogue();

  drawFavourites();
  
	return 0;
	drawLogInView();

	drawOverlay_dialogWindow("test");
	
  sleep(1);
	
  drawCatalogue();
	
  // https://stackoverflow.com/questions/10463201/getch-and-arrow-codes
	
  return 0;
}