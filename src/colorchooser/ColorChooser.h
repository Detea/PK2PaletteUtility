#pragma once
#include <QWidget>
#include <QSpinBox>
#include <qpushbutton.h>

#include "ColorSelectionBar.h"

class ColorChooser : public QWidget {
	Q_OBJECT

public:
	ColorChooser(QWidget* parent = nullptr);

	void setColor(const QColor& color);

	QPushButton* getOffsetButtonHue();
	QPushButton* getOffsetButtonSaturation();
	QPushButton* getOffsetButtonValue();

signals:
	void startColorChange();
	void commitColorChange(const QColor& newColor);		// This signal gets emitted when the user stops changing the color, either when they release the left mouse button or one of the spinboxes finishes editing

	void hueChanged(int newHue);
	void saturationChanged(int newSaturation);
	void valueChanged(int newValue);

private:
	void setupUI();

	void setupSlots();

	void updateSelectedColor(QColor& newColor);

private slots:
	// These get called when the ColorSelectionBars change and set selectedColor.
	void updateHue(int hue);
	void updateSaturation(int saturation);
	void updateValue(int value);

	void saveSelectedColor();

	void signalColorChangeStart();
	void signalColorChangeCommit();

private:
	ColorSelectionBar* csHue = nullptr;
	ColorSelectionBar* csSaturation = nullptr;
	ColorSelectionBar* csValue = nullptr;

	QPushButton* btnOffsetHue = nullptr;
	QPushButton* btnOffsetSaturation = nullptr;
	QPushButton* btnOffsetValue = nullptr;

	QSpinBox* spinBoxHue = nullptr;
	QSpinBox* spinBoxSaturation = nullptr;
	QSpinBox* spinBoxValue = nullptr;

	QColor lastSelectedColor;
	QColor selectedColor;
};