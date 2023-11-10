#pragma once
#include <tuple>

namespace PaletteUtils {
	/*
		Reconstructing the color index via mouse position

		Example:
		mouse y, mouse x ---------->
		|-------------------
		||0 |1 |2 |3 | 4| 5|
		|-------------------
		||6 |7 |8 |9 |10|11|
		|-------------------
		||12|13|14|15|16|17|
		v-------------------

		Let's say we have a mouse position of (4 * ENTRY_SIZE, 1 * ENTRY_SIZE).
		mx = 4 * ENTRY_SIZE
		my = 1 * ENTRY_SIZE

		To adjust the mouse positon to grid coordinates we divide it by ENTRY_SIZE.

		ENTRY_AMOUNT = 6

		Then we do this:
		my * ENTRY_AMOUNT + mx

		my * ENTRY_AMOUNT to get the "correct" y position in the grid
		+ mx to get to the "correct" x position.

		For this above example the color index would be 10.
	*/
	inline int indexFromPosition(int x, int y, int entrySize, int entryAmount) {
		int xPos = x / entrySize;
		int yPos = y / entrySize;
		int colorIndex = yPos * entryAmount + xPos;

		return colorIndex;
	}

	inline std::tuple<int, int> positionFromIndex(int index, int rows, int cols, int entrySize) {
		int xPos = index % rows;
		int yPos = index / cols;

		xPos *= entrySize;
		yPos *= entrySize;

		return { xPos, yPos };
	}

	inline int clampHSVRelative(int color, int offset, int maximumValue, bool subtract = false) {
		if (subtract) offset *= -1;

		int result = color + offset;

		if (result > maximumValue) {
			result = result - maximumValue - 1;
		} else if (result < 0) {
			result = maximumValue - result;
		}

		if (result > maximumValue) result = maximumValue;

		return result;
	}

	inline int clampHSV(int oldValue, int newValue, int maximumValue) {
		// Calculate the "relative value". The difference between the old and new value...
		int minValue = std::min(oldValue, newValue);
		int maxValue = std::max(oldValue, newValue);
		int valueDifference = maxValue - minValue;

		// Wrap around, if necessary
		if (valueDifference < 0) {
			valueDifference = maximumValue - valueDifference;
		} else if (valueDifference > maximumValue) {
			valueDifference = valueDifference - maximumValue;
		}

		int returnValue = oldValue;
		if (newValue > returnValue) {	// Increase value
			returnValue += valueDifference;

			// Keep the value in bounds
			if (returnValue > maximumValue) {
				returnValue = returnValue - maximumValue;
			}
		} else if (newValue < returnValue) {	// Decrease value
			returnValue -= valueDifference;

			if (returnValue < 0) {
				returnValue = maximumValue - returnValue;
			}
		}

		return returnValue;
	}

	inline int calculateColorOffset(int value1, int value2) {
		int minHue = std::min(value1, value2);
		int maxHue = std::max(value1, value2);

		return maxHue - minHue;
	}
}