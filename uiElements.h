
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct uiElement {
	int x1;
	int y;
	int x2;
	char* color_bg;
	char* color_fill;
	char* color_text;
	int limit_low;
	int limit_high;
	int asciilim_low;
	int asciilim_high;
	int length;
	char* value;
	void (*focus)(struct uiElement_textInput *self);
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


char* readText(uiElement *element) {
	int x = element->x1+2;
	int y = element->y+1;
	int broken = 0;
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
        break;
      }
    }
		else if (c == 9) { // Tab
			broken = 1;
			break;
		}
    // printf("%i %c / ", c, c);
  }
	if (broken == 1) {
		if (element->length < element->limit_high) {
  		goToPoint(x+element->length, y);
  		printFm(" ", element->color_fill, element->color_text);
		}
		element->next->focus(element->next);
	}
	//strcpy(target, str);
	return element->value;
}

void waitForButtonPress(uiElement *element) {
	int x = element->x1+2;
	int y = element->y+1;
	int broken = 0;
	char c = '1';
	drawInputBox(element->x1, element->y, element->x1+(element->length)+4, COLOR_BACKGROUND_FRONT, COLOR_BACKGROUND_HIGHLIGHT);
	goToPoint(x, y);
	printBold(element->value, COLOR_BACKGROUND_HIGHLIGHT, COLOR_BACKGROUND_APP);
  while (1 == 1) {
    c = getch();
		if (c == 10) { // Enter
			// do an action...
			break;
    }
		else if (c == 9) { // Tab
			broken = 1;
			drawInputBox(element->x1, element->y, element->x1+(element->length)+4, element->color_bg, element->color_fill);
			goToPoint(x, y);
			printFm(element->value, element->color_fill, element->color_text);
			break;
		}
  }
	if (broken == 1) {
		element->next->focus(element->next);
	}
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
	drawInputBox(x1, y, x2, color_bg, color_fill);
	return element;
}

uiElement* uiInit_button(int x, int y, char* color_bg, char* color_fill, char* color_text, char* value) {
	uiElement *element = (uiElement*)malloc(sizeof(uiElement));
	element->x1 = x;
	element->y = y;
	element->color_bg = color_bg;
	element->color_fill = color_fill;
	element->color_text = color_text;
	element->value = value;
	element->length = (int)strlen(value) / 2 - 1;
	element->focus = waitForButtonPress;
	drawInputBox(x, y, x+(element->length)+4, color_bg, color_fill);
	goToPoint(x+2, y+1);
	printFm(value, COLOR_BACKGROUND_BACK, COLOR_TEXT_FRONT);
	return element;
}