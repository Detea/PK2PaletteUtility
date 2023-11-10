#pragma once
#include <qlist.h>
#include <qcolor.h>
#include <qfile.h>

#include <string>

class PaletteFileParser {
public:
	/*
		Reads a palette text file created by PaletteUtility.

		Returns true on success, false on failure.

		Call getErrorMessage() to get the error message, when this method returns false.
	*/
	bool read(QFile& file, QList<QColor>& targetList);

	const QString& getErrorMessage();

private:
	bool parse(QList<QColor>& targetList);

	char currentChar();
	void advance();
	char peek();

	int readNumber();

	// Checks if the RGB value is in range (0 - 255)
	bool checkRGB(int value);

	// Checks if the red, green and blue values are in range and returns true if they are. If any one the values isn't in range returns false and sets errorMessage.
	bool rgbValuesInRange(int red, int green, int blue);

	bool isNumber(char c);

private:
	QString errorMessage = "";
	std::string fileContents;

	int currentPos = 0;
	int currentLine = 1;
	int currentPosInLine = 1;
};