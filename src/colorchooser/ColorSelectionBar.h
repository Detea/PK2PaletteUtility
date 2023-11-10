#pragma once
#include <QWidget>
#include <QColor>
#include <QPen>
#include <QRect>
#include <QMouseEvent>

#include "ColorComponent.h"

class ColorSelectionBar : public QWidget {
	Q_OBJECT

public:
	ColorSelectionBar(ColorComponent component);

	void setColor(const QColor& color);
	void setHue(int hue);
	void setSaturation(int saturation);
	void setValue(int value);

	QColor getColor() const;
	int getHue() const;
	int getSaturation() const;
	int getValue() const;

signals:
	void hueChanged(int newHue);
	void saturationChanged(int newSaturation);
	void valueChanged(int newValue);

	void startChange();		// Gets emitted when the user holds down the left mouse button. Signaling that they have started to change the value.
	void commitChange();	// Gets emitted when the user lets go of the mouse button. Signaling that the value has been changed (committed).

protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void changeColor();

private:
	void handleColorIndicator(QMouseEvent* event);
	void updateColorIndicator();

private:
	ColorComponent colorComponent;
	
	// Gets set, when the user clicks the left mouse button. This value is needed to compare it with the new color value, when the user releases the left mouse button
	int oldValue = 0;

	int gradientWidth = 255;
	int gradientHeight = 30;
	int gradientXOffset = 1;

	QRect colorIndicator{ 0, 0, 8, 8 };
	QPen pen;

	QColor selectedColor;
};