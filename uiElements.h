
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern film *currentFilm;

typedef struct uiElement {
	int x1; // X-координата слева
	int y; // Y-координата сверху
	int x2; // X-координата справа
	char* color_bg; // Цвет фона за элементом
	char* color_fill; // Цвет фоновой заливки элемента
	char* color_text; // Цвет текста в элементе
	int limit_low; // Текстовое поле: минимальное количество символов
	int limit_high; // Текстовое поле: максимальное количество символов
	int asciilim_low; // Текстовое поле: минимальный ASCII-код вводимых символов
	int asciilim_high; // Текстовое поле: максимальнйы ASCII-код вводимых символов
	int length; // 
	char* value; // Текст в элементе
	void (*focus)(struct uiElement *self, char **currentView); // Функция, которая выделяет данный элемент как активный и ждёт ввода
	void (*show)(struct uiElement *self); // Функция, которая рисует элемент (в нормальном состоянии) на экране
	void (*onInput)(); // struct uiElement *self, char **currentView
	void (*onBlur)(struct uiElement *self, char **currentView);
	char* (*getValue)(struct uiElement *self);
	struct uiElement* next;
	struct uiElement* previous;
} uiElement;


/*
void chain(int n, uiListElement a, ...) {
   va_list valist;
   va_start(valist, n);
   for (i = 0; i < num; i++) {
      sum += va_arg(valist, int);
   }
   va_end(valist);
}
*/


char* readText(uiElement *element, char** currentView) {
	int x = element->x1+2;
	int y = element->y+1;
	int broken = 0;
	uiElement *broken_elementToSwitchTo = NULL;
	char c = '1';
	
  goToPoint(x, y);
  printFm(element->value, element->color_fill, element->color_text);
	if (element->length < element->limit_high) {
  	goToPoint(x+element->length, y);
  	printFm("|", element->color_fill, element->color_text);
	}
		
  while (1 == 1) {
    c = getch();
    // printf("%i %c / ", c, c);
		if (element->length < element->limit_high) {
	    if (c >= element->asciilim_low && c <= element->asciilim_high) {
	      element->value[element->length] = c;
	      element->length++;
      	goToPoint(x, y);
      	printFm(element->value, element->color_fill, element->color_text);
				if (element->length < element->limit_high) {
      		goToPoint(x+element->length, y);
      		printFm("|", element->color_fill, element->color_text);
				}
	    }
		}
	  if (c == 127) { // Backspace
	    element->value[element->length - 1] = ' ';
	    element->length--;
      goToPoint(x, y);
      printFm(element->value, element->color_fill, element->color_text);
      goToPoint(x+element->length, y);
      printFm("|", element->color_fill, element->color_text);
	  }
		else if (c == 10) { // Enter
      if (element->length > element->limit_low-1) {
				broken = 1;
				broken_elementToSwitchTo = element->next;
        break;
      }
    }
		else if (c == 9) { // Tab
			enterHeaderSwitcher();
			break;
		}
    else if (c == 27) {
      c = getch();
      c = getch();
      if (c == 'A') {
        // UP
				broken = 1;
				broken_elementToSwitchTo = element->previous;
				break;
      } 
			else if (c == 'B') {
        // DOWN
				broken = 1;
				broken_elementToSwitchTo = element->next;
				break;
      } 
			else if (c == 'C') {
        // RIGHT
    		if (*currentView == "КАТАЛОГ") {
        	currentFilm = currentFilm->next;
        	drawCatalogue();
					break;
    		}
      } 
			else if (c == 'D') {
        // LEFT
    		if (*currentView == "КАТАЛОГ") {
        	currentFilm = currentFilm->previous;
        	drawCatalogue();
					break;
    		}
      }
    }
    // printf("%i %c / ", c, c);
  }
	if (broken == 1) {
		if (element->length < element->limit_high) {
  		goToPoint(x+element->length, y);
  		printFm(" ", element->color_fill, element->color_text);
		}
		broken_elementToSwitchTo->focus(broken_elementToSwitchTo, currentView);
	}
	//strcpy(target, str);
	return element->value;
}

void waitForButtonPress(uiElement *element, char** currentView) {
	int x = element->x1+2;
	int y = element->y+1;
	int broken = 0;
	uiElement *broken_elementToSwitchTo = NULL;
	char c = '1';
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, COLOR_BACKGROUND_HIGHLIGHT);
	goToPoint(x, y);
	printBold(element->value, COLOR_BACKGROUND_HIGHLIGHT, COLOR_BACKGROUND_APP);
  while (1 == 1) {
    c = getch();
		if (c == 10) { // Enter
			// do an action...
			element->onInput();
			break;
    }
		else if (c == 9) { // Tab
			enterHeaderSwitcher();
			break;
		}
    else if (c == 27) {
      c = getch();
      c = getch();
      if (c == 'A') {
        // UP
				broken = 1;
				broken_elementToSwitchTo = element->previous;
				break;
      } 
			else if (c == 'B') {
        // DOWN
				broken = 1;
				broken_elementToSwitchTo = element->next;
				break;
      } 
			else if (c == 'C') {
        // RIGHT
    		if (*currentView == "КАТАЛОГ") {
        	currentFilm = currentFilm->next;
        	drawCatalogue();
					break;
    		}
    		else if (*currentView == "ИЗБРАННОЕ") {
        	currentFilm = currentFilm->next;
        	drawFavourites();
					break;
    		}
      } 
			else if (c == 'D') {
        // LEFT
    		if (*currentView == "КАТАЛОГ") {
        	currentFilm = currentFilm->previous;
        	drawCatalogue();
					break;
    		}
        else if (*currentView == "ИЗБРАННОЕ") {
        	currentFilm = currentFilm->previous;
        	drawFavourites();
					break;
    		}
      }
    }
  }
	if (broken == 1) {
		drawInputBox(element->x1, element->y, element->x2, element->color_bg, element->color_fill);
		goToPoint(x, y);
		printFm(element->value, element->color_fill, element->color_text);
		broken_elementToSwitchTo->focus(broken_elementToSwitchTo, currentView);
	}
}

void showTextInput(uiElement* element) {
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, element->color_fill);
}
void showButton(uiElement* element) {
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, element->color_fill);
	goToPoint(element->x1+2, element->y+1);
	printFm(element->value, element->color_fill, COLOR_TEXT_FRONT);
}

char* getValue(uiElement* element) {
	int position = strlenPlus(element->value)-1;
  for (int i = position; i >= 0; i--) {
    if (element->value[i] != ' ') break;
		position--;
  }
	position++;
  char *returnValue = (char*)malloc(position+1);
  for (int i = 0; i < position; i++) {
    returnValue[i] = element->value[i];
  }
	returnValue[position] = '\0';
	return returnValue;
}

uiElement* uiInit_textInput(int x1, int y, int x2, char* color_bg, char* color_fill, char* color_text, int limit_low, int limit_high, int asciilim_low, int asciilim_high) {
	uiElement *element = (uiElement*)malloc(sizeof(uiElement));
	element->x1 = x1;
	element->y = y;
	element->x2 = x2;
	element->color_bg = color_bg;
	element->color_fill = color_fill;
	element->color_text = color_text;
	element->limit_low = limit_low;
	element->limit_high = limit_high;
	element->asciilim_low = asciilim_low;
	element->asciilim_high = asciilim_high;
	element->length = 0;
  element->value = (char *)malloc(element->limit_high);
  for (int i = 0; i < element->limit_high; i++) {
    element->value[i] = ' ';
  }
	element->focus = readText;
	element->show = showTextInput;
	element->getValue = getValue;
	return element;
}

uiElement* uiInit_button(int x1, int y, int x2, char* color_bg, char* color_fill, char* color_text, char* value) {
	uiElement *element = (uiElement*)malloc(sizeof(uiElement));
	element->x1 = x1;
	element->y = y;
	element->color_bg = color_bg;
	element->color_fill = color_fill;
	element->color_text = color_text;
	element->value = value;
	element->length = (int)strlen(value) / 2 - 1;
	element->focus = waitForButtonPress;
	element->show = showButton;
  if (x2 > element->length+4) {
    element->x2 = x2;
  }
  else {
    element->x2 = x1 + element->length + 4;
  }
	return element;
}