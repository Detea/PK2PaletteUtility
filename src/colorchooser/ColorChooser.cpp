#include "ColorChooser.h"

#include <qgridlayout.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qtabwidget.h>

ColorChooser::ColorChooser(QWidget* parent) : QWidget(parent) {
	setupUI();
	setupSlots();
}

void ColorChooser::setupUI() {
	QLabel* lblHue = new QLabel("Hue:");
	QLabel* lblSaturation = new QLabel("Saturation:");
	QLabel* lblValue = new QLabel("Value:");

	spinBoxHue = new QSpinBox;
	spinBoxSaturation = new QSpinBox;
	spinBoxValue = new QSpinBox;

	spinBoxHue->setRange(0, 359);
	spinBoxSaturation->setRange(0, 255);
	spinBoxValue->setRange(0, 255);

	csHue = new ColorSelectionBar(ColorComponent::Hue);
	csSaturation = new ColorSelectionBar(ColorComponent::Saturation);
	csValue = new ColorSelectionBar(ColorComponent::Value);

	QGridLayout* glHsv = new QGridLayout;
	glHsv->setHorizontalSpacing(0);
	glHsv->setVerticalSpacing(0);
	glHsv->setSpacing(0);
	glHsv->setContentsMargins(0, 0, 0, 0);

	btnOffsetHue = new QPushButton("Absolute");
	btnOffsetSaturation = new QPushButton("Absolute");
	btnOffsetValue = new QPushButton("Absolute");

	QHBoxLayout* hueHBox = new QHBoxLayout;
	hueHBox->setSpacing(4);
	hueHBox->addWidget(lblHue);

	QBoxLayout* hueBarSpinBoxLayout = new QHBoxLayout;
	hueBarSpinBoxLayout->addWidget(csHue);
	hueBarSpinBoxLayout->addStretch();

	glHsv->addLayout(hueHBox, 0, 0);
	glHsv->addWidget(btnOffsetHue, 0, 1);
	glHsv->addLayout(hueBarSpinBoxLayout, 1, 0);
	glHsv->addWidget(spinBoxHue, 0, 2);

	QHBoxLayout* saturationHBox = new QHBoxLayout;
	saturationHBox->addWidget(lblSaturation);
	saturationHBox->addStretch();

	glHsv->addLayout(saturationHBox, 2, 0);
	glHsv->addWidget(csSaturation, 3, 0);
	glHsv->addWidget(btnOffsetSaturation, 2, 1);
	glHsv->addWidget(spinBoxSaturation, 2, 2);

	QHBoxLayout* valueHBox = new QHBoxLayout;
	valueHBox->addWidget(lblValue);
	valueHBox->addStretch();

	glHsv->addLayout(valueHBox, 4, 0);
	glHsv->addWidget(csValue, 5, 0);
	glHsv->addWidget(btnOffsetValue, 4, 1);
	glHsv->addWidget(spinBoxValue, 4, 2);

	setLayout(glHsv);
}

void ColorChooser::setupSlots() {
	connect(csHue, &ColorSelectionBar::hueChanged, this, &ColorChooser::updateHue);
	connect(csSaturation, &ColorSelectionBar::saturationChanged, this, &ColorChooser::updateSaturation);
	connect(csValue, &ColorSelectionBar::valueChanged, this, &ColorChooser::updateValue);

	connect(spinBoxHue, &QSpinBox::valueChanged, csHue, &ColorSelectionBar::setHue);
	connect(spinBoxSaturation, &QSpinBox::valueChanged, csSaturation, &ColorSelectionBar::setSaturation);
	connect(spinBoxValue, &QSpinBox::valueChanged, csValue, &ColorSelectionBar::setValue);

	connect(csHue, &ColorSelectionBar::startChange, this, &ColorChooser::signalColorChangeStart);
	connect(csHue, &ColorSelectionBar::commitChange, this, &ColorChooser::signalColorChangeCommit);

	connect(csSaturation, &ColorSelectionBar::startChange, this, &ColorChooser::signalColorChangeStart);
	connect(csSaturation, &ColorSelectionBar::commitChange, this, &ColorChooser::signalColorChangeCommit);

	connect(csValue, &ColorSelectionBar::startChange, this, &ColorChooser::signalColorChangeStart);
	connect(csValue, &ColorSelectionBar::commitChange, this, &ColorChooser::signalColorChangeCommit);

	connect(spinBoxHue, &QSpinBox::editingFinished, this, &ColorChooser::signalColorChangeCommit);
	connect(spinBoxSaturation, &QSpinBox::editingFinished, this, &ColorChooser::signalColorChangeCommit);
	connect(spinBoxValue, &QSpinBox::editingFinished, this, &ColorChooser::signalColorChangeCommit);
}

void ColorChooser::signalColorChangeStart() {
	emit startColorChange();
}

void ColorChooser::signalColorChangeCommit() {
	emit commitColorChange(selectedColor);
}

void ColorChooser::saveSelectedColor() {
	lastSelectedColor = selectedColor;
}

void ColorChooser::setColor(const QColor& color) {
	lastSelectedColor = selectedColor;
	selectedColor = color;

	csHue->setColor(color);
	csSaturation->setColor(color);
	csValue->setColor(color);
	
	blockSignals(true);
	spinBoxHue->setValue(color.hue());
	spinBoxSaturation->setValue(color.saturation());
	spinBoxValue->setValue(color.value());
	blockSignals(false);
}

void ColorChooser::updateSelectedColor(QColor& newColor) {
	selectedColor = newColor;
}

void ColorChooser::updateHue(int hue) {
	csSaturation->setHue(hue);
	csValue->setHue(hue);

	spinBoxHue->setValue(hue);

	selectedColor.setHsv(hue, selectedColor.saturation(), selectedColor.value());

	emit hueChanged(hue);
}

void ColorChooser::updateSaturation(int saturation) {
	csHue->setSaturation(saturation);
	csValue->setSaturation(saturation);

	spinBoxSaturation->setValue(saturation);

	selectedColor.setHsv(selectedColor.hue(), saturation, selectedColor.value());

	emit saturationChanged(saturation);
}

void ColorChooser::updateValue(int value) {
	csHue->setValue(value);
	csSaturation->setValue(value);

	spinBoxValue->setValue(value);

	selectedColor.setHsv(selectedColor.hue(), selectedColor.saturation(), value);

	emit valueChanged(value);
}

QPushButton* ColorChooser::getOffsetButtonHue() {
	return btnOffsetHue;
}

QPushButton* ColorChooser::getOffsetButtonSaturation() {
	return btnOffsetSaturation;
}

QPushButton* ColorChooser::getOffsetButtonValue() {
	return btnOffsetValue;
}
