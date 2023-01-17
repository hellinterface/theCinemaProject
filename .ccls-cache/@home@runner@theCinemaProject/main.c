#include "global.h"
#include "graphics.h"
#include "termiosWrapper.h"
#include "uiElements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

film* currentFilm = NULL;
film* firstFilm = NULL;

user* userDatabase[64];
int currentUserID = -1; // -1 - пользователь не выбран
int totalUsers;
int dialogWindowVisible = 0;

// Элементы: страница входа в аккаунт
uiElement* input_login_username;
uiElement* input_login_password;
uiElement* button_login_ok;
uiElement* button_login_toSignUpPage;

// Элементы: страница регистрации
uiElement* input_signup_username;
uiElement* input_signup_password;
uiElement* input_signup_cardNumber;
uiElement* button_signup_ok;
uiElement* button_signup_toLogInPage;

// Элементы: страница каталога/избранного
uiElement* button_catalogue_toDetailsPage;
uiElement* button_catalogue_toggleFavouriteState;
uiElement* button_catalogue_adminRemoveFilm;

// Элементы: страница детального просмотра
uiElement* button_details_toCatalogue;
uiElement* button_details_toggleFavouriteState;
uiElement* button_details_adminRemoveFilm;

// Элементы: страница настроек
uiElement* button_settings_goBack;
uiElement* button_settings_clearFavourites;
uiElement* button_settings_toSignUpPage;

// Элементы: страница добавления фильма
uiElement* button_addFilm_goBack;
uiElement* button_addFilm_ok;
uiElement* input_addFilm_title;
uiElement* input_addFilm_year;
uiElement* input_addFilm_country;
uiElement* input_addFilm_genre;
uiElement* input_addFilm_rating;

uiElement* button_dialogWindow_OK;

// Точки навигации
navPoint* navPoint_catalogue;
navPoint* navPoint_favourites;
navPoint* navPoint_details;
navPoint* navPoint_settings;
navPoint* navPoint_logIn;
navPoint* navPoint_signUp;
navPoint* navPoint_exit;
navPoint* navPoint_addFilm;

// Переменные точки навигации
navPoint* currentNavPoint;
navPoint* previousNavPoint;
navPoint* headerSwitcher_selectedNavPoint;

// Функция инициализации точек навигации
void initNavPoints() {
	navPoint_catalogue = (navPoint*)malloc(sizeof(navPoint));
	navPoint_details = (navPoint*)malloc(sizeof(navPoint));
	navPoint_favourites = (navPoint*)malloc(sizeof(navPoint));
	navPoint_settings = (navPoint*)malloc(sizeof(navPoint));
	navPoint_logIn = (navPoint*)malloc(sizeof(navPoint));
	navPoint_signUp = (navPoint*)malloc(sizeof(navPoint));
	navPoint_exit = (navPoint*)malloc(sizeof(navPoint));
	navPoint_addFilm = (navPoint*)malloc(sizeof(navPoint));

	navPoint_catalogue->switchTo = drawCatalogue;
	navPoint_favourites->switchTo = drawFavourites;
	navPoint_details->switchTo = drawDetailedView;
	navPoint_settings->switchTo = drawSettings;
	navPoint_logIn->switchTo = drawLogInView;
	navPoint_signUp->switchTo = drawSignUpView;
	navPoint_exit->switchTo = appExit;
	navPoint_addFilm->switchTo = drawAddFilmView;
	
	navPoint_catalogue->title = "КАТАЛОГ";
	navPoint_favourites->title = "ИЗБРАННОЕ";
	navPoint_details->title = "ДЕТАЛИ";
	navPoint_settings->title = "НАСТРОЙКИ";
	navPoint_logIn->title = "ВХОД";
	navPoint_signUp->title = "РЕГИСТРАЦИЯ";
	navPoint_exit->title = "ЗАКРЫТЬ ПРИЛОЖЕНИЕ";
	navPoint_addFilm->title = "ДОБАВИТЬ ФИЛЬМ";
}

// Переключение заголовочного меню в нормальный режим 
void navPoints_normalMode() {
	navPoint_catalogue->next = navPoint_favourites;
	navPoint_favourites->next = navPoint_details;
	navPoint_details->next = navPoint_settings;
	navPoint_settings->next = navPoint_logIn;
	navPoint_logIn->next = navPoint_exit;
	navPoint_exit->next = navPoint_catalogue;
	
	navPoint_catalogue->previous = navPoint_exit;
	navPoint_favourites->previous = navPoint_catalogue;
	navPoint_details->previous = navPoint_favourites;
	navPoint_settings->previous = navPoint_details;
	navPoint_logIn->previous = navPoint_settings;
	navPoint_exit->previous = navPoint_logIn;
	
	if (userDatabase[currentUserID]->isAdmin == 1) {
		navPoint_exit->next = navPoint_addFilm;
		navPoint_addFilm->next = navPoint_catalogue;
		
		navPoint_catalogue->previous = navPoint_addFilm;
		navPoint_addFilm->previous = navPoint_exit;
	}
}

// Переключение заголовочного меню в режим входа/регистрации
void navPoints_loggedOffMode() {
	navPoint_logIn->next = navPoint_exit;
	navPoint_signUp->next = navPoint_exit;
	navPoint_exit->next = currentNavPoint;
	
	navPoint_logIn->previous = navPoint_exit;
	navPoint_signUp->previous = navPoint_exit;
	navPoint_exit->previous = currentNavPoint;
}

// https://github.com/pikvic/fefu2022/blob/main/cinema.md

// Улучшенная функция fgets, которая возвращает строку переменного размера (char*)
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
      returnValue = (char*)malloc(i+1);
      for (int n = 0; n < i; n++) {
        returnValue[n] = str2[n];
      }
			returnValue[i] = '\0';
      break;
    }
  }
  return returnValue;
}

// Улучшенная функция strlen, которая верно работает с кириллицей
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

// Функция рисовки заголовка (хедера)
void drawHeader(char *title) {
	drawRectWithShadow(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 0);
  goToPoint(6, 2);
  printBold(title, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	if (currentUserID != -1) {
		char username[21];
		strcpy(username, userDatabase[currentUserID]->name);
  	goToPoint(VIEWPORT_WIDTH-4-strlen(username), 2);
  	printFm(username, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
	}
}

// Функция рисовки (обновления) меню в заголовке и обрабатывания ввода в нём
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
			headerSwitcher_selectedNavPoint->switchTo();
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

// Функция переключения на заголовочное меню
void enterHeaderSwitcher() {
	headerSwitcher_selectedNavPoint = currentNavPoint;
	drawHeaderSwitcher();
}

// Функция рисовки страницы регистрации
void drawSignUpView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_signUp) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_signUp;
	}
	if (currentUserID == -1) {
		navPoint_exit->next = currentNavPoint;
		navPoint_exit->previous = currentNavPoint;
	}
	drawHeader(currentNavPoint->title);
	resetAllTextInputsBesidesCurrent();
	
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

	input_signup_username->focus(input_signup_username);
}

// Функция, выполняемая при нажатии на кнопку "ОК" диалогового окна
void buttonPress_dialogWindowOk(uiElement *element) {
	drawHeader(currentNavPoint->title);
	currentNavPoint->switchTo(currentNavPoint);
}

// Функция рисовки диалогового окна в углу экрана
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

	dialogWindowVisible = 1;
	
	button_dialogWindow_OK->show(button_dialogWindow_OK);
	button_dialogWindow_OK->focus(button_dialogWindow_OK);
}

// Функция рисовки страницы входа
void drawLogInView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_logIn) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_logIn;
	}
	drawHeader(currentNavPoint->title);
	resetAllTextInputsBesidesCurrent();
	
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

	input_login_username->focus(input_login_username);
}

// Функция рисовки страницы каталога
void drawCatalogue() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_catalogue) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_catalogue;
	}
	drawHeader(currentNavPoint->title);

  drawRectWithShadow(3, 10, 27, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);
	drawRectWithShadow(54, 10, 78, 25, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, COLOR_SHADOW_BACK, 1);

	drawRectWithShadow(5, 19, 25, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(5, 22, 25, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(56, 19, 75, 20, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	drawRectWithShadow(56, 22, 75, 23, COLOR_SHADOW_BACK, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP, 0);
	
	drawRectWithShadow(28, 8, 53, 27, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_SHADOW_FRONT, 1);

  goToPoint(5, 12);
  printBoldLimited((currentFilm->previous)->title, 27-3-4, 5, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  goToPoint(3, 15);
  printFor("━", 53-28, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP);
	
  goToPoint(56, 12);
  printBoldLimited((currentFilm->next)->title, 27-3-4, 56, 12, COLOR_BACKGROUND_BACK, COLOR_TEXT_BACK);
  goToPoint(54, 15);
  printFor("━", 53-28, COLOR_BACKGROUND_BACK, COLOR_BACKGROUND_APP);
  
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

	if (userDatabase[currentUserID]->isAdmin) {
		button_catalogue_adminRemoveFilm->show(button_catalogue_adminRemoveFilm);
	}
	
  button_catalogue_toDetailsPage->focus(button_catalogue_toDetailsPage);
}

int switchToFavorite_next() {
  int found = 2;
  film *first = currentFilm;
  while (currentFilm->isFavourite == 0 && found != 0) {
    if (first == currentFilm) {
      found--;
    }  
    currentFilm = currentFilm->next;
  }
  if (found == 0) {
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
    drawOverlay_dialogWindow("Фильмов в избранном нет");
  }
	return found;
}

int switchToFavorite_previous() {
  int found = 2;
  film *first = currentFilm;
  while (currentFilm->isFavourite == 0 && found != 0) {
    if (first == currentFilm) {
      found--;
    }  
    currentFilm = currentFilm->previous;
  }
  if (found == 0) {
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
    drawOverlay_dialogWindow("Фильмов в избранном нет");
  }
	return found;
}

// Функция рисовки страницы просмотра избранного
void drawFavourites() {
  
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_favourites) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_favourites;
	}
	drawHeader(currentNavPoint->title);

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

	if (userDatabase[currentUserID]->isAdmin) {
		button_catalogue_adminRemoveFilm->show(button_catalogue_adminRemoveFilm);
	}
	
  button_catalogue_toDetailsPage->focus(button_catalogue_toDetailsPage);
}

// Функция рисовки страницы детального просмотра
void drawDetailedView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_details) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_details;
	}
	drawHeader(currentNavPoint->title);

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
	
	if (userDatabase[currentUserID]->isAdmin) {
		button_details_adminRemoveFilm->show(button_details_adminRemoveFilm);
	}
	
	button_details_toCatalogue->focus(button_details_toCatalogue);
}


// Функция рисовки страницы добавления фильма (режим администратора)
void drawAddFilmView() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_addFilm) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_addFilm;
	}
	drawHeader(currentNavPoint->title);
	
  goToPoint(10, 5);
  printFm("Название фильма", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

  goToPoint(10, 10);
  printFm("Год", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(10, 15);
  printFm("Страна", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(10, 20);
  printFm("Жанры", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
		
  goToPoint(10, 25);
  printFm("Рейтинг", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

	
	input_addFilm_title->show(input_addFilm_title);
	input_addFilm_year->show(input_addFilm_year);
	input_addFilm_country->show(input_addFilm_country);
	input_addFilm_genre->show(input_addFilm_genre);
	input_addFilm_rating->show(input_addFilm_rating);
	button_addFilm_ok->show(button_addFilm_ok);
	button_addFilm_goBack->show(button_addFilm_goBack);
	
	input_addFilm_title->focus(input_addFilm_title);
}

// Функция выхода из приложения
void appExit() {
	system("clear");
	drawRect(0, 0, VIEWPORT_WIDTH-1, 3, COLOR_BACKGROUND_FRONT);
  goToPoint(6, 2);
  printFm("Приложение закрыто.", COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT);
  goToPoint(0, 4);
	exit(0);
}

// Функция рисовки страницы настроек
void drawSettings() {
	system("clear");
  goToPoint(0, 0);
  fillBackground();
	dialogWindowVisible = 0;
	if (currentNavPoint != navPoint_settings) {
		previousNavPoint = currentNavPoint;
		currentNavPoint = navPoint_settings;
	}
	drawHeader(currentNavPoint->title);
	
  goToPoint(4, 6);
  printFm("Имя пользователя: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm("123", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

  goToPoint(4, 8);
  printFm("Пароль: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm("123", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(4, 10);
  printFm("Номер карты: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm("123", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
	
  goToPoint(4, 12);
  printFm("Фильмов в списке избранного: ", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);
  printFm("123", COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT);

	button_settings_toSignUpPage->show(button_settings_toSignUpPage);
	button_settings_clearFavourites->show(button_settings_clearFavourites);
	button_settings_goBack->show(button_settings_goBack);
	
	button_settings_goBack->focus(button_settings_goBack);
}

// Функция, выполняемая при нажатии на кнопку "Войти"
void buttonPress_login() {
	//drawOverlay_dialogWindow();
	int i = 0;
	int found = -1;
	char* inputValue_name = input_login_username->getValue(input_login_username);
	char* inputValue_password = input_login_password->getValue(input_login_password);
	while (userDatabase[i] != NULL) {
		if (strcmp(userDatabase[i]->name, inputValue_name) == 0) {
			found = i;
			currentUserID = i;
			if (strcmp(userDatabase[i]->password, inputValue_password) == 0) {
				onLogIn();
				navPoint_catalogue->switchTo(navPoint_catalogue);
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

// Функция, выполняемая при нажатии на кнопку "Сохранить" на экране регистрации
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
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
		drawOverlay_dialogWindow("В пароле нет букв в верхнем регистре.");
	}
	else if (passcheck_hasLowercase == 0) {
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
		drawOverlay_dialogWindow("В пароле нет букв в нижнем регистре.");
	}
	else if (passcheck_hasNumbers == 0) {
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
		drawOverlay_dialogWindow("В пароле нет цифр.");
	}
	else {
		if (previousNavPoint == navPoint_settings) {
			char oldFilename[120] = "favourites/favourite_";
			strcat(oldFilename, userDatabase[currentUserID]->name);
			strcat(oldFilename, ".txt");
			
			userDatabase[currentUserID]->name = input_signup_username->getValue(input_signup_username);
			userDatabase[currentUserID]->password = password;
			userDatabase[currentUserID]->cardNumber = input_signup_cardNumber->getValue(input_signup_cardNumber);
			
			FILE *fout_users = fopen("users.txt", "wt");
			for (int i = 0; i < totalUsers; i++) {
				fprintf(fout_users, "%s\n%s\n%s\n%i\n", userDatabase[i]->name, userDatabase[i]->password, userDatabase[i]->cardNumber, userDatabase[i]->isAdmin);
			}
			fclose(fout_users);
			
			char newFilename[120] = "favourites/favourite_";
			strcat(newFilename, userDatabase[currentUserID]->name);
			strcat(newFilename, ".txt");
			rename(oldFilename, newFilename);

			button_dialogWindow_OK->onInput = goToPreviousPage;
			drawOverlay_dialogWindow("Изменения сохранены.");
		}
		else {
			FILE *fout_users = fopen("users.txt", "a");
			user* t = (user*)malloc(sizeof(user));
			t->name = input_signup_username->getValue(input_signup_username);
			t->password = password;
			t->cardNumber = input_signup_cardNumber->getValue(input_signup_cardNumber);
			t->isAdmin = 0;
			fprintf(fout_users, "%s\n%s\n%s\n%i\n", t->name, t->password, t->cardNumber, t->isAdmin);
			fclose(fout_users);
			currentUserID = totalUsers;
			totalUsers++;
			
			char filename[120] = "favourites/favourite_";
			strcat(filename, t->name);
			strcat(filename, ".txt");
  		FILE *f = fopen(filename, "a");
			fclose(f);
			
			button_dialogWindow_OK->onInput = onLogIn;
			drawOverlay_dialogWindow("Аккаунт создан.");
		}
	}
}

// Функция чтения списка фильмов (films.txt)
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

// Функция чтения списка пользователей (users.txt)
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

// Функция поиска фильма с нужным названием
film *findFilm(char *title) {
	film* t = firstFilm;
	if (strcmp(t->title, title) == 0) return t;
	t = t->next;
	while (t != firstFilm) {
		if (strcmp(t->title, title) == 0) return t;
		t = t->next;
	}
	return NULL;
}

// Функция чтения списка избранного пользователя с указанным ID
void readFavouriteList(int userID) {
	char filename[120] = "favourites/favourite_";
	strcat(filename, userDatabase[userID]->name);
	strcat(filename, ".txt");
  FILE *fin_favouriteFilms = fopen(filename, "rt");

	film *current = firstFilm->next;
	firstFilm->isFavourite = 0;
  while (current != firstFilm) {
		current->isFavourite = 0; 
    current = current->next;
  }
	
	if (fin_favouriteFilms == NULL) {
  	fin_favouriteFilms = fopen(filename, "a");
		fclose(fin_favouriteFilms);
		return;
	}
	
  while (!feof(fin_favouriteFilms)) {
    film *t = (film*)malloc(sizeof(film));
    char *title = fgetsPlus(fin_favouriteFilms, 128);
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

// Записать в файл списка избранного список избранного, который находится в программе.
void updateFavouriteList(int userID) {
	char filename[120] = "favourites/favourite_";
	strcat(filename, userDatabase[userID]->name);
	strcat(filename, ".txt");
  FILE *fin_favouriteFilms = fopen(filename, "wt");

	film *current = firstFilm->next;
	if (firstFilm->isFavourite == 1) {
		fprintf(fin_favouriteFilms, "%s\n%i\n%s\n%s\n%.1lf\n", firstFilm->title, firstFilm->year, firstFilm->country, firstFilm->genre, firstFilm->rating);
	};
  while (current != firstFilm) {
		if (current->isFavourite == 1) {
		fprintf(fin_favouriteFilms, "%s\n%i\n%s\n%s\n%.1lf\n", current->title, current->year, current->country, current->genre, current->rating);
		}
    current = current->next;
  }
	
	fclose(fin_favouriteFilms);
}

void clearCurrentUserFavourites() {
  
}

// Добавление фильма в список фильмов.
void buttonPress_addFilm_ok() {
	input_addFilm_title->getValue(input_addFilm_title);
	double parsedRating = atof(input_addFilm_rating->getValue(input_addFilm_rating));
	if (parsedRating > 10 || parsedRating == 0) {
		button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
		drawOverlay_dialogWindow("Неверный рейтинг.");
		return;
	}

	int parsedYear = atoi(input_addFilm_year->getValue(input_addFilm_year));
	FILE *fout_films = fopen("films.txt", "a");
	film* t = (film*)malloc(sizeof(film));
	t->title = input_addFilm_title->getValue(input_addFilm_title);
	t->year = parsedYear;
	t->country = input_addFilm_country->getValue(input_addFilm_country);
	t->genre = input_addFilm_genre->getValue(input_addFilm_genre);
	t->rating = parsedRating;
	fprintf(fout_films, "%s\n%i\n%s\n%s\n%.1lf\n", t->title, t->year, t->country, t->genre, t->rating);
	fclose(fout_films);
	t->previous = firstFilm->previous;
	t->next = firstFilm;
	firstFilm->previous->next = t;
	firstFilm->previous = t;
	currentFilm = t;
	button_dialogWindow_OK->onInput = drawCatalogue;
	drawOverlay_dialogWindow("Фильм добавлен.");
}

// it works!
void buttonPress_removeFilm() {
	FILE *fout_films = fopen("films.txt", "wt");
	currentFilm->previous->next = currentFilm->next;
	currentFilm->next->previous = currentFilm->previous;
	if (currentFilm == firstFilm) firstFilm = currentFilm->next;
	film *deletedFilm = currentFilm;
	currentFilm = deletedFilm->previous;
	free(deletedFilm);
	int done = 0;
	film *t = firstFilm;
	while (done == 0) {
		if (t->next == firstFilm) done = 1;
		fprintf(fout_films, "%s\n%i\n%s\n%s\n%lf\n", t->title, t->year, t->country, t->genre, t->rating);
		t = t->next;
	}
	fclose(fout_films);
	for (int i = 0; i < totalUsers; i++) {
		readFavouriteList(i);
		updateFavouriteList(i);
	}
	readFavouriteList(currentUserID);
	if (currentNavPoint == navPoint_details) {
		previousNavPoint->switchTo(previousNavPoint);
	}
	else {
		currentNavPoint->switchTo(currentNavPoint);
	}
}

// Функция переключения состояния isFavourite активного фильма
void toggleFavouriteStateOfCurrentFilm() {
	char* text = "Убрать из избранного";
	if (currentFilm->isFavourite == 1) {
		currentFilm->isFavourite = 0;
		text = "Добавить в избранное";
	}
	else {
		currentFilm->isFavourite = 1;
	}
	updateFavouriteList(currentUserID);
	int textLength = strlenPlus(text);
	if (currentNavPoint == navPoint_details) {
		button_details_toggleFavouriteState->value = text;
		button_details_toggleFavouriteState->length = textLength;
		button_details_toggleFavouriteState->focus(button_details_toggleFavouriteState);
	}
	else if (currentNavPoint == navPoint_catalogue) {
		button_catalogue_toggleFavouriteState->value = text;
		button_catalogue_toggleFavouriteState->length = textLength;
		button_catalogue_toggleFavouriteState->focus(button_catalogue_toggleFavouriteState);
	}
	else if (currentNavPoint == navPoint_favourites) {
		navPoint_favourites->switchTo(navPoint_favourites);
	}
}

// Функция переключения на предыдущую страницу
void goToPreviousPage() {
  previousNavPoint->switchTo(previousNavPoint);
}

void resetAllTextInputsBesidesCurrent() {
	if (currentNavPoint != navPoint_logIn) {
		input_login_username->resetValue(input_login_username);
		input_login_password->resetValue(input_login_password);
	}
	if (currentNavPoint != navPoint_signUp) {
		input_signup_username->resetValue(input_signup_username);
		input_signup_password->resetValue(input_signup_password);
		input_signup_cardNumber->resetValue(input_signup_cardNumber);
	}
	if (currentNavPoint != navPoint_addFilm) {
		input_addFilm_title->resetValue(input_addFilm_title);
		input_addFilm_year->resetValue(input_addFilm_year);
		input_addFilm_country->resetValue(input_addFilm_country);
		input_addFilm_genre->resetValue(input_addFilm_genre);
		input_addFilm_rating->resetValue(input_addFilm_rating);
	}
}

// Открыть экран редактирования данных пользователя
void editCurrentUserData() {
  input_signup_username->setValue(input_signup_username, userDatabase[currentUserID]->name);
  input_signup_password->setValue(input_signup_password, userDatabase[currentUserID]->password);
  input_signup_cardNumber->setValue(input_signup_cardNumber, userDatabase[currentUserID]->cardNumber);
  navPoint_signUp->switchTo(navPoint_signUp);
}

void initAllElements() {
	// Вход //////////////////////////////////////////////////
	input_login_username = uiInit_textInput(50, 10, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 'E');
	input_login_password = uiInit_textInput(50, 15, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 'E');
	button_login_ok = uiInit_button(50, 19, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Войти");
	button_login_toSignUpPage = uiInit_button(50, 22, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Создать аккаунт");
	
	button_login_ok->onInput = buttonPress_login;
	button_login_toSignUpPage->onInput = drawSignUpView;
	
	// Регистрация //////////////////////////////////////////////////
	input_signup_username = uiInit_textInput(50, 10, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 'E');
	input_signup_password = uiInit_textInput(50, 15, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 3, 20, 'E');
	input_signup_cardNumber = uiInit_textInput(50, 20, 73, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, 16, 16, 'D');
	button_signup_toLogInPage = uiInit_button(50, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Назад");
	button_signup_ok = uiInit_button(63, 23, 0, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Сохранить");
	
	button_signup_ok->onInput = buttonPress_signup_createUser;
	button_signup_toLogInPage->onInput = drawLogInView;
	
	// Каталог //////////////////////////////////////////////////
button_catalogue_toDetailsPage = uiInit_button(29, 19, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Подробнее...");
	button_catalogue_toggleFavouriteState = uiInit_button(29, 22, 52, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Добавить в избранное");
	button_catalogue_adminRemoveFilm = uiInit_button(59, 27, 75, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Удалить фильм");
	
	button_catalogue_toDetailsPage->onInput = drawDetailedView;
	button_catalogue_toggleFavouriteState->onInput = toggleFavouriteStateOfCurrentFilm;
	button_catalogue_adminRemoveFilm->onInput = buttonPress_removeFilm;
	
	// Детальный просмотр //////////////////////////////////////////////////
	button_details_toCatalogue = uiInit_button(29, 19, 52, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Назад");
	button_details_toggleFavouriteState = uiInit_button(29, 22, 52, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Добавить в избранное");
	button_details_adminRemoveFilm = uiInit_button(59, 27, 75, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Удалить фильм");
	
	button_details_toCatalogue->onInput = goToPreviousPage;
	button_details_toggleFavouriteState->onInput = toggleFavouriteStateOfCurrentFilm;
	button_details_toggleFavouriteState->onInput = buttonPress_removeFilm;
	
	// Настройки //////////////////////////////////////////////////
	button_settings_toSignUpPage = uiInit_button(4, 14, 30, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Изменить данные");
	button_settings_clearFavourites = uiInit_button(4, 17, 30, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Очистить избранное");
	button_settings_goBack = uiInit_button(4, 20, 30, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_FRONT, COLOR_TEXT_FRONT, "Назад");

  button_settings_toSignUpPage->onInput = editCurrentUserData;
  button_settings_clearFavourites->onInput = clearCurrentUserFavourites;
	button_settings_goBack->onInput = goToPreviousPage;
  
	// Добавление фильма //////////////////////////////////////////////////
	input_addFilm_title = uiInit_textInput(6, 6, 73, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, 1, 36, 'A');
	input_addFilm_year = uiInit_textInput(6, 11, 73, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, 4, 4, 'D');
	input_addFilm_country = uiInit_textInput(6, 16, 73, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, 1, 36, 'A');
	input_addFilm_genre = uiInit_textInput(6, 21, 73, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, 1, 36, 'A');
	input_addFilm_rating = uiInit_textInput(6, 26, 28, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, 3, 4, 'F');
	button_addFilm_ok = uiInit_button(30, 26, 0, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Сохранить");
	button_addFilm_goBack = uiInit_button(44, 26, 0, COLOR_BACKGROUND_APP, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT, "Назад");
	
	button_addFilm_ok->onInput = buttonPress_addFilm_ok;
	button_addFilm_goBack->onInput = goToPreviousPage;
	
	// Диалоговое окно //////////////////////////////////////////////////
	button_dialogWindow_OK = uiInit_button(VIEWPORT_WIDTH-5-3, 1, VIEWPORT_WIDTH-3, COLOR_BACKGROUND_DIALOG, COLOR_BACKGROUND_APP, COLOR_TEXT_FRONT, "OK");
	button_dialogWindow_OK->onInput = buttonPress_dialogWindowOk;
	button_dialogWindow_OK->previous = button_dialogWindow_OK;
	button_dialogWindow_OK->next = button_dialogWindow_OK;
}

void linkLogInSignUpElements() {
	// Вход //////////////////////////////////////////////////
	input_login_username->next = input_login_password;
	input_login_password->next = button_login_ok;
	button_login_ok->next = button_login_toSignUpPage;
	button_login_toSignUpPage->next = input_login_username;

	input_login_username->previous = button_login_toSignUpPage;
	input_login_password->previous = input_login_username;
	button_login_ok->previous = input_login_password;
	button_login_toSignUpPage->previous = button_login_ok;
	
	// Регистрация //////////////////////////////////////////////////
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
}

// Функция инициализации и связывания всех элементов в приложении
void linkMainUIElements() {
	// Каталог //////////////////////////////////////////////////////
	button_catalogue_toDetailsPage->next = button_catalogue_toggleFavouriteState;
	button_catalogue_toggleFavouriteState->next = button_catalogue_toDetailsPage;
	button_catalogue_toDetailsPage->previous = button_catalogue_toggleFavouriteState;
	button_catalogue_toggleFavouriteState->previous = button_catalogue_toDetailsPage;
	
	if (userDatabase[currentUserID]->isAdmin == 1) {
		button_catalogue_toggleFavouriteState->next = button_catalogue_adminRemoveFilm;
		button_catalogue_adminRemoveFilm->next = button_catalogue_toDetailsPage;
		button_catalogue_adminRemoveFilm->previous = button_catalogue_toggleFavouriteState;
		button_catalogue_toDetailsPage->previous = button_catalogue_adminRemoveFilm;
	}
	
	// Детальный просмотр //////////////////////////////////////////////////////
	button_details_toCatalogue->next = button_details_toggleFavouriteState;
	button_details_toggleFavouriteState->next = button_details_toCatalogue;
	
	button_details_toCatalogue->previous = button_details_toggleFavouriteState;
	button_details_toggleFavouriteState->previous = button_details_toCatalogue;

	
	// Настройки //////////////////////////////////////////////////////
	button_settings_toSignUpPage->next = button_settings_clearFavourites;
	button_settings_clearFavourites->next = button_settings_goBack;
	button_settings_goBack->next = button_settings_toSignUpPage;
	
	button_settings_toSignUpPage->previous = button_settings_goBack;
	button_settings_clearFavourites->previous = button_settings_toSignUpPage;
	button_settings_goBack->previous = button_settings_clearFavourites;
	
	// Добавление фильма //////////////////////////////////////////////////////

	input_addFilm_title->next = input_addFilm_year;
	input_addFilm_year->next = input_addFilm_country;
	input_addFilm_country->next = input_addFilm_genre;
	input_addFilm_genre->next = input_addFilm_rating;
	input_addFilm_rating->next = button_addFilm_ok;
	button_addFilm_ok->next = button_addFilm_goBack;
	button_addFilm_goBack->next = input_addFilm_title;
	
	input_addFilm_title->previous = button_addFilm_goBack;
	input_addFilm_year->previous = input_addFilm_title;
	input_addFilm_country->previous = input_addFilm_year;
	input_addFilm_genre->previous = input_addFilm_country;
	input_addFilm_rating->previous = input_addFilm_genre;
	button_addFilm_ok->previous = input_addFilm_rating;
	button_addFilm_goBack->previous = button_addFilm_ok;
	
}

int enterKeyDebug() {
	while (1 == 1) {
		char c = '1';
		c = getch();
  	printf("CODE: %i\n", c);
	}
}

void onLogIn() {
	navPoints_normalMode();
	readFavouriteList(currentUserID);
	linkMainUIElements();
	navPoint_logIn->title = "ВЫЙТИ ИЗ АККАУНТА";
	navPoint_signUp->title = "ИЗМЕНЕНИЕ ДАННЫХ";
	//navPoint_addFilm->switchTo(navPoint_addFilm);
	navPoint_catalogue->switchTo(navPoint_catalogue);
}

void logOff() {
	currentUserID = -1;
	navPoints_loggedOffMode();
	navPoint_logIn->title = "ВХОД";
	navPoint_signUp->title = "РЕГИСТРАЦИЯ";
	navPoint_logIn->switchTo(navPoint_logIn);
}

int main(void) {
  system("clear");
  cursorHide();
  goToPoint(0, 0);

	// enterKeyDebug();
	
	initNavPoints();
	initAllElements();
	linkLogInSignUpElements();
	readFilmList();
  firstFilm->previous = currentFilm;
  currentFilm->next = firstFilm;
  currentFilm = firstFilm;
	readUserList();

	currentUserID = 0;
	
	onLogIn();
	
	logOff();

  return 0;
}