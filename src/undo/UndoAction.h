#pragma once
#include <qcolor.h>
#include "ColorComponent.h"

struct UndoAction {
	ColorComponent colorComponent;

	int oldValue;
	int newValue;

	int colorIndex;
};