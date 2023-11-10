#include "PaletteFileParser.h"
#include <qdebug.h>
#include <fstream>

bool PaletteFileParser::read(QFile& file, QList<QColor>& targetList) {
	bool success = true;

	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		while (!file.atEnd()) {
			std::string str = QString(file.readLine()).toStdString();
			fileContents += str;
		}

		success = parse(targetList);
	} else {
		errorMessage = "Unable to open file: " + file.fileName();

		success = false;
	}

	return success;
}

bool PaletteFileParser::parse(QList<QColor>& targetList) {
	while (currentPos < fileContents.length()) {
		// Skip comments
		if (isNumber(currentChar())) {
			int red = readNumber();
			int green = readNumber();
			int blue = readNumber();

			if (!rgbValuesInRange(red, green, blue)) {
				return false;
			} else {
				targetList.push_back(QColor(red, green, blue));
			}
		} else if (currentChar() == ';') {
			do {
				advance();
			} while (peek() != '\n');
			advance();
		} else if (currentChar() == '\n') {
			advance();
		} else if (currentChar() == ' ') {
			advance();
		} else {
			errorMessage = "Line: " + QString::number(currentLine);
			errorMessage += ", Position: " + QString::number(currentPosInLine);
			errorMessage += " Unexpected character: ";
			errorMessage += currentChar();

			return false;
		}
	}

	return true;
}

char PaletteFileParser::currentChar() {
	char character = '\0';

	if (currentPos + 1 < fileContents.length()) {
		character = fileContents[currentPos];
	}

	return character;
}

void PaletteFileParser::advance() {
	currentPosInLine++;

	if (currentChar() == '\n') {
		currentLine++;
		currentPosInLine = 0;
	}

	currentPos++;
}

char PaletteFileParser::peek() {
	char peekChar = '\0';

	if (currentPos + 1 < fileContents.length()) {
		peekChar = fileContents[currentPos + 1];
	}

	return peekChar;
}

int PaletteFileParser::readNumber() {
	// Read the first number
	std::string numbers = "";
	
	while (isNumber(currentChar())) {
		numbers += currentChar();
		advance();
	}

	advance();

	int number = -1;
	try {
		number = std::stoi(numbers);
	} catch (std::invalid_argument const& ex) {
		qDebug() << ex.what();
		
		errorMessage = "Line: " + QString::number(currentLine);
		errorMessage += ", Position: " + QString::number(currentPosInLine) + ": ";
		errorMessage += "Unable to create number from string";
	} catch (std::out_of_range const& ex) {
		qDebug() << ex.what();

		errorMessage = "Line: " + QString::number(currentLine);
		errorMessage += ", Position: " + QString::number(currentPosInLine) + ": ";
		errorMessage = "Number out of valid range.";
	}

	return number;
}

bool PaletteFileParser::checkRGB(int value) {
	return value >= 0 && value <= 255;
}

bool PaletteFileParser::rgbValuesInRange(int red, int green, int blue) {
	if (!checkRGB(red)) {
		errorMessage = "Line: " + QString::number(currentLine) + " Red value (" + QString::number(red) + ") out of range (0 - 255)!";
		
		return false;
	}

	if (!checkRGB(green)) {
		errorMessage = "Line: " + QString::number(currentLine) + " Green value (" + QString::number(green) + ") out of range (0 - 255)!";

		return false;
	}

	if (!checkRGB(blue)) {
		errorMessage = "Line: " + QString::number(currentLine) + " Blue value (" + QString::number(blue) + ") out of range (0 - 255)!";

		return false;
	}

	return true;
}

bool PaletteFileParser::isNumber(char c) {
	return c >= '0' && c <= '9';
}

const QString& PaletteFileParser::getErrorMessage() {
	return errorMessage;
}