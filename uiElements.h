#include "graphics.h"
#include "termiosWrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct uiElement_textInput {
	int limit_low;
	int limit_high;
	int length;
	char* value;
	void (*focus)(uiElement_textInput *self);
} uiElement_textInput;

typedef struct uiListElement {
	uiListElement* previous;
	uiListElement* next;
} uiListElement;
