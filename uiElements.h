
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Обработка нажатия клавиш стрелок на клавиатуре:
// https://stackoverflow.com/questions/10463201/getch-and-arrow-codes

// "Импорт" переменных из main.c
extern film *currentFilm;
extern navPoint *currentNavPoint;
extern int dialogWindowVisible;

// Структура интерактивного элемента интерфейса (текстовое поле/кнопка)
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
	void (*focus)(struct uiElement *self); // Функция, которая выделяет данный элемент как активный и ждёт ввода
	void (*show)(struct uiElement *self); // Функция, которая рисует элемент (в нормальном состоянии) на экране
	void (*onInput)();
	char* (*getValue)(struct uiElement *self);
  char* (*setValue)(struct uiElement* self, char* value);
	char* (*resetValue)(struct uiElement *self);
	struct uiElement* next;
	struct uiElement* previous;
} uiElement;

// Очистить значение элемента.
void* resetValueOfElement(uiElement *element) {
  element->value[0] = '\0';
	element->length = 0;
}

// Сделать данное текстовое поле активным элементом.
// Отвечает за обработку событий клавиатуры.
char* uiTextInput_onFocus(uiElement *element) {
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
    goToPoint(0, VIEWPORT_HEIGHT+1);
    c = getch();
    // printf("%i %c / ", c, c);
		if (element->length < element->limit_high) {
	    if (c >= element->asciilim_low && c <= element->asciilim_high) {
	      element->value[element->length] = c;
	      element->length++;
				drawRect(x, y, x+element->limit_high, y, element->color_fill);
      	goToPoint(x, y);
      	printFm(element->value, element->color_fill, element->color_text);
				if (element->length < element->limit_high) {
      		goToPoint(x+element->length, y);
      		printFm("|", element->color_fill, element->color_text);
				}
	    }
		}
	  if (c == 127) { // Backspace
	    element->value[element->length - 1] = '\0';
	    element->length--;
			drawRect(x, y, x+element->limit_high, y, element->color_fill);
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
      } 
			else if (c == 'D') {
      }
    }
    // printf("%i %c / ", c, c);
  }
	if (broken == 1) {
		if (element->length < element->limit_high) {
  		goToPoint(x+element->length, y);
  		printFm(" ", element->color_fill, element->color_text);
		}
		broken_elementToSwitchTo->focus(broken_elementToSwitchTo);
	}
	//strcpy(target, str);
	return element->value;
}

// Сделать данную кнопку активным элементом.
// Отвечает за обработку событий клавиатуры.
void uiButton_onFocus(uiElement *element) {
	int x = element->x1+2;
	int y = element->y+1;
	int broken = 0;
	uiElement *broken_elementToSwitchTo = NULL;
	char c = '1';
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, COLOR_BACKGROUND_HIGHLIGHT);
	goToPoint(x, y);
	printBold(element->value, COLOR_BACKGROUND_HIGHLIGHT, COLOR_BACKGROUND_APP);
  while (1 == 1) {
    goToPoint(0, VIEWPORT_HEIGHT+1);
    c = getch();
		if (c == 10) { // Enter
			element->onInput();
			break;
    }
		else if (c == 9) { // Tab
			if (dialogWindowVisible == 0) {
				enterHeaderSwitcher();
				break;
			}
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
    		if (currentNavPoint->title == "КАТАЛОГ") {
        	currentFilm = currentFilm->next;
        	drawCatalogue();
					break;
    		}
    		else if (currentNavPoint->title== "ИЗБРАННОЕ") {
        	currentFilm = currentFilm->next;
        	drawFavourites();
					break;
    		}
      } 
			else if (c == 'D') {
        // LEFT
    		if (currentNavPoint->title == "КАТАЛОГ") {
        	currentFilm = currentFilm->previous;
        	drawCatalogue();
					break;
    		}
        else if (currentNavPoint->title == "ИЗБРАННОЕ") {
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
		broken_elementToSwitchTo->focus(broken_elementToSwitchTo);
	}
}

// Функция, рисующая данное текстовое поле.
void showTextInput(uiElement* element) {
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, element->color_fill);
  goToPoint(element->x1+2, element->y+1);
  printFm(element->value, element->color_fill, element->color_text);
}

// Функция, рисующая данную кнопку.
void showButton(uiElement* element) {
	drawInputBox(element->x1, element->y, element->x2, element->color_bg, element->color_fill);
	goToPoint(element->x1+2, element->y+1);
	printFm(element->value, element->color_fill, COLOR_TEXT_FRONT);
}

// Функция, возвращающая значение элемента.
char* getValue(uiElement* element) {
	return element->value;
}

// Функция, изменяющая значение элемента на нужное.
void setValue(uiElement* element, char* value) {
	strcpy(element->value, value);
	element->length = strlenPlus(element->value);
}

// Функция, создающая элемент "Текстовое поле" для дальнйшей работы.
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
  element->value = (char*)malloc(element->limit_high);
	element->resetValue = resetValueOfElement;
	element->setValue = setValue;
	element->focus = uiTextInput_onFocus;
	element->show = showTextInput;
	element->getValue = getValue;
	element->resetValue(element);
	return element;
}

// Функция, создающая элемент "Кнопка" для дальнйшей работы.
uiElement* uiInit_button(int x1, int y, int x2, char* color_bg, char* color_fill, char* color_text, char* value) {
	uiElement *element = (uiElement*)malloc(sizeof(uiElement));
	element->x1 = x1;
	element->y = y;
	element->color_bg = color_bg;
	element->color_fill = color_fill;
	element->color_text = color_text;
	element->value = value;
	element->length = strlenPlus(value);
	element->focus = uiButton_onFocus;
	element->show = showButton;
  if (x2 > element->length+4) {
    element->x2 = x2;
  }
  else {
    element->x2 = x1 + element->length + 4;
  }
	return element;
}