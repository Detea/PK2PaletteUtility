#include "PaletteViewWidget.h"
#include <qpainter.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qgridlayout.h>
#include <QTabWidget>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <algorithm>
#include <fstream>

#include "PaletteUtils.h"
#include "PaletteFileParser.h"

#include "ColorsChangeCommand.h"

PaletteViewWidget::PaletteViewWidget() {
	renderer = new PaletteRenderer;

	setupUI();
	setupSlots();
}

void PaletteViewWidget::setupUI() {
	renderer = new PaletteRenderer;

	colorChooser = new ColorChooser(this);

	QHBoxLayout* hbIndex = new QHBoxLayout;
	QLabel* lblIndex = new QLabel("Index:");
	sbIndex = new QSpinBox;
	sbIndex->setRange(0, 255);

	lblIndex->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sbIndex->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	hbIndex->addWidget(lblIndex);
	hbIndex->addWidget(sbIndex);
	hbIndex->addStretch();

	QVBoxLayout* vboxButtons = new QVBoxLayout;
	vboxButtons->setContentsMargins(0, 10, 0, 0);
	vboxButtons->setSpacing(30);

	QHBoxLayout* hboxColorChooser = new QHBoxLayout;
	hboxColorChooser->addWidget(colorChooser);
	hboxColorChooser->addLayout(vboxButtons);

	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(renderer);
	vbox->setAlignment(renderer, Qt::AlignHCenter);
	vbox->addLayout(hbIndex);
	vbox->addLayout(hboxColorChooser);

	QVBoxLayout* vbMain = new QVBoxLayout;
	vbMain->setContentsMargins(0, 0, 0, 0);
	vbMain->addLayout(vbox);
	vbMain->setAlignment(Qt::AlignTop | Qt::AlignLeft | Qt::AlignRight);

	undoStack = new QUndoStack(this);

	undoAction = undoStack->createUndoAction(this);
	undoAction->setShortcut(QKeySequence::Undo);

	redoAction = undoStack->createRedoAction(this);
	redoAction->setShortcut(QKeySequence::Redo);

	addAction(undoAction);
	addAction(redoAction);

	setMaximumWidth(400);

	setLayout(vbMain);
}

void PaletteViewWidget::setupSlots() {
	connect(renderer, &PaletteRenderer::selectionChanged, this, &PaletteViewWidget::updateSelection);

	connect(colorChooser, &ColorChooser::hueChanged, this, &PaletteViewWidget::updateHue);
	connect(colorChooser, &ColorChooser::saturationChanged, this, &PaletteViewWidget::updateSaturation);
	connect(colorChooser, &ColorChooser::valueChanged, this, &PaletteViewWidget::updateValue);

	connect(colorChooser, &ColorChooser::commitColorChange, this, &PaletteViewWidget::commitSelectionColorsChanges);

	connect(sbIndex, &QSpinBox::valueChanged, this, &PaletteViewWidget::updateSelectedColorFromSpinBox);

	connect(colorChooser->getOffsetButtonHue(), &QPushButton::pressed, this, &PaletteViewWidget::changeHueOffset);
	connect(colorChooser->getOffsetButtonSaturation(), &QPushButton::pressed, this, &PaletteViewWidget::changeSaturationOffset);
	connect(colorChooser->getOffsetButtonValue(), &QPushButton::pressed, this, &PaletteViewWidget::changeValueOffset);
}

void PaletteViewWidget::undoPaletteChanges(const QList<UndoAction>& actionList) {
	QList<QColor> changedColors;
	QList<int> affectedColors;

	for (const UndoAction& action : actionList) {
		QColor& color = colorPalette[action.colorIndex];

		switch (action.colorComponent) {
			case ColorComponent::Hue:
				color.setHsv(action.oldValue, color.saturation(), color.value());
				break;

			case ColorComponent::Saturation:
				color.setHsv(color.hue(), action.oldValue, color.value());
				break;

			case ColorComponent::Value:
				color.setHsv(color.hue(), color.saturation(), action.oldValue);
				break;
		}

		loadedImage->setColor(action.colorIndex, colorPalette[action.colorIndex].rgb());

		changedColors.push_back(color);
		affectedColors.push_back(action.colorIndex);
	}

	renderer->setColorsInPalette(affectedColors, changedColors);
	
	emit paletteChanged();

	// Update the indicators in the colorChooser
	colorChooser->setColor(colorPalette.at(selectionMedianValue));
}

void PaletteViewWidget::redoPaletteChanges(const QList<UndoAction>& actionList) {
	// Prevent the first ColorsChangeCommand::redo() that Qt fires when you push a new command. No idea why they do that...
	if (undoStack->count() != undoStack->index()) {
		QList<QColor> changedColors;
		QList<int> affectedColors;
		
		for (const UndoAction& action : actionList) {
			QColor& color = colorPalette[action.colorIndex];

			switch (action.colorComponent) {
				case ColorComponent::Hue:
					color.setHsv(action.newValue, color.saturation(), color.value());
					break;

				case ColorComponent::Saturation:
					color.setHsv(color.hue(), action.newValue, color.value());
					break;

				case ColorComponent::Value:
					color.setHsv(color.hue(), color.saturation(), action.newValue);
					break;
			}

			loadedImage->setColor(action.colorIndex, colorPalette[action.colorIndex].rgb());

			changedColors.push_back(color);
			affectedColors.push_back(action.colorIndex);
		}

		renderer->setColorsInPalette(affectedColors, changedColors);

		emit paletteChanged();

		colorChooser->setColor(colorPalette.at(selectionMedianValue));
	}
}

void PaletteViewWidget::commitSelectionColorsChanges(const QColor& newColor) {
	int undoActionsAmount = 1;

	QList<UndoAction> undoActionList;

	for (int i = 0; i < selectedIndicesList.size(); i++) {
		int affectedColor = selectedIndicesList.at(i);

		QColor oldColor = colorPalette.at(affectedColor);
		
		if (oldColor.hue() != newColor.hue()) {
			int newHue = adjustColor(hueOffset, oldColor.hue(), newColor.hue(), 360, colorPalette.at(selectionMedianValue).hue());

			undoActionList.push_back(UndoAction{ ColorComponent::Hue, oldColor.hue(), newHue, affectedColor });

			colorPalette[affectedColor].setHsv(newHue, oldColor.saturation(), oldColor.value());
		} else if (oldColor.saturation() != newColor.saturation()) {
			int newSaturation = adjustColor(saturationOffset, oldColor.saturation(), newColor.saturation(), 255, colorPalette.at(selectionMedianValue).saturation());

			undoActionList.push_back(UndoAction{ ColorComponent::Saturation, oldColor.saturation(), newSaturation, affectedColor });

			colorPalette[affectedColor].setHsv(oldColor.hue(), newSaturation, oldColor.value());
		} else if (oldColor.value() != newColor.value()) {
			int newValue = adjustColor(valueOffset, oldColor.value(), newColor.value(), 255, colorPalette.at(selectionMedianValue).value());

			undoActionList.push_back(UndoAction{ ColorComponent::Value, oldColor.value(), newValue, affectedColor });

			colorPalette[affectedColor].setHsv(oldColor.hue(), oldColor.saturation(), newValue);
		}
	}

	if (!undoActionList.isEmpty()) {
		QUndoCommand* colorsChangeCommand = new ColorsChangeCommand(undoActionList, this);

		undoStack->push(colorsChangeCommand);
	}

	// Seems pretty hacky, but needed to ensure that this widget regains focus after the user changes the value in one of the colorChooser spinboxes, so that undo/redo works
	if (!hasFocus()) setFocus();
}

void PaletteViewWidget::updateHue(int newHue) {
	QList<QColor> newColors;

	if (selectedIndicesList.size() > 1) {
		for (int i = 0; i < selectedIndicesList.size(); i++) {
			QColor color = colorPalette.at(selectedIndicesList[i]);

			int newHueVal = adjustColor(hueOffset, color.hue(), newHue, 360, colorPalette.at(selectionMedianValue).hue());

			color.setHsv(newHueVal, color.saturation(), color.value());

			loadedImage->setColor(selectedIndicesList[i], color.rgb());

			newColors.push_back(color);
		}
	} else if (selectedIndicesList.size() == 1) {
		QColor color = colorPalette[selectedIndicesList.at(0)];
		
		// Update the internal color palette
		color.setHsv(newHue, color.saturation(), color.value());

		loadedImage->setColor(selectedIndicesList[0], color.rgb());

		newColors.push_back(color);
	}

	renderer->setColorsInPalette(selectedIndicesList, newColors);

	emit paletteChanged();
}

void PaletteViewWidget::updateSaturation(int newSaturation) {
	QList<QColor> newColors;

	if (selectedIndicesList.size() > 1) {
		for (int i = 0; i < selectedIndicesList.size(); i++) {
			QColor color = colorPalette.at(selectedIndicesList[i]);

			int newSaturationVal = adjustColor(saturationOffset, color.saturation(), newSaturation, 255, colorPalette.at(selectionMedianValue).saturation());

			color.setHsv(color.hue(), newSaturationVal, color.value());

			loadedImage->setColor(selectedIndicesList[i], color.rgb());

			newColors.push_back(color);
		}
	} else if (selectedIndicesList.size() == 1) {
		QColor color = colorPalette[selectedIndicesList.at(0)];

		color.setHsv(color.hue(), newSaturation, color.value());

		loadedImage->setColor(selectedIndicesList[0], color.rgb());

		newColors.push_back(color);
	}

	renderer->setColorsInPalette(selectedIndicesList, newColors);

	emit paletteChanged();
}

void PaletteViewWidget::updateValue(int newValue) {
	QList<QColor> newColors;

	if (selectedIndicesList.size() > 1) {
		for (int i = 0; i < selectedIndicesList.size(); i++) {
			QColor color = colorPalette.at(selectedIndicesList[i]);

			int newValueVal = adjustColor(valueOffset, color.value(), newValue, 255, colorPalette.at(selectionMedianValue).value());

			color.setHsv(color.hue(), color.saturation(), newValueVal);

			loadedImage->setColor(selectedIndicesList[i], color.rgb());

			newColors.push_back(color);
		}
	} else if (selectedIndicesList.size() == 1) {
		QColor color = colorPalette[selectedIndicesList.at(0)];

		color.setHsv(color.hue(), color.saturation(), newValue);

		loadedImage->setColor(selectedIndicesList[0], color.rgb());

		newColors.push_back(color);
	}

	renderer->setColorsInPalette(selectedIndicesList, newColors);

	emit paletteChanged();
}

int PaletteViewWidget::adjustColor(ColorChangeOffset offsetType, int oldValue, int newValue, int maxValue, int medianValue) {
	int newValueVal = oldValue;
	int offset = PaletteUtils::calculateColorOffset(medianValue, newValue);
	bool subtract = medianValue > newValue;

	if (offsetType == ColorChangeOffset::Absolute) {
		newValueVal = PaletteUtils::clampHSVRelative(medianValue, offset, maxValue, subtract);
	} else if (offsetType == ColorChangeOffset::Relative) {
		newValueVal = PaletteUtils::clampHSVRelative(oldValue, offset, maxValue, subtract);
	}

	return newValueVal;
}

void PaletteViewWidget::updateSelection(const QList<int>& list) {
	if (!colorPalette.isEmpty()) {
		selectedIndicesList = list;

		if (selectedIndicesList.size() == 1) {
			sbIndex->setEnabled(true);
			sbIndex->setValue(selectedIndicesList.at(0));

			selectionMedianValue = selectedIndicesList.at(0);

			colorChooser->setColor(colorPalette.at(selectionMedianValue));
		} else if (selectedIndicesList.size() > 1) {
			sbIndex->setEnabled(false);

			// If the selection is greater than one, we find the median color and use that as a reference/orientation for the user
			selectionMedianValue = 0;

			QList<int> sortedList = selectedIndicesList;
			std::sort(sortedList.begin(), sortedList.end());

			int middle1 = sortedList.size() / 2;

			// If the size of the list is even we take the two numbers in the middle, add them together and then divide by 2
			if (sortedList.size() % 2 == 0) {
				int middle2 = middle1 + 1;

				if (sortedList.size() == 2) {
					middle1 = 0;
					middle2 = 1;
				}

				selectionMedianValue = (sortedList.at(middle1) + sortedList.at(middle2)) / 2;
			} else {
				selectionMedianValue = sortedList.at(middle1);
			}

			colorChooser->setColor(colorPalette.at(selectionMedianValue));
		}
	}
}

// These three methods are pretty ugly, they SHOULD be in the the ColorChooser class, but whatever.
void PaletteViewWidget::changeHueOffset() {
	hueOffset = hueOffset == ColorChangeOffset::Absolute ? ColorChangeOffset::Relative : ColorChangeOffset::Absolute;

	colorChooser->getOffsetButtonHue()->setText(colorChooser->getOffsetButtonHue()->text() == "Absolute" ? "Relative" : "Absolute");
}

void PaletteViewWidget::changeSaturationOffset() {
	saturationOffset = saturationOffset == ColorChangeOffset::Absolute ? ColorChangeOffset::Relative : ColorChangeOffset::Absolute;

	colorChooser->getOffsetButtonSaturation()->setText(colorChooser->getOffsetButtonSaturation()->text() == "Absolute" ? "Relative" : "Absolute");
}

void PaletteViewWidget::changeValueOffset() {
	valueOffset = valueOffset == ColorChangeOffset::Absolute ? ColorChangeOffset::Relative : ColorChangeOffset::Absolute;

	colorChooser->getOffsetButtonValue()->setText(colorChooser->getOffsetButtonValue()->text() == "Absolute" ? "Relative" : "Absolute");
}

void PaletteViewWidget::updateSelectedColorFromSpinBox() {
	renderer->setSelectedIndex(sbIndex->value());
}

void PaletteViewWidget::clearSelection() {
	renderer->clearSelection();
}

void PaletteViewWidget::updatePalette(QList<QColor>& newPalette) {
	if (loadedImage) {
		for (int i = 0; i < newPalette.size(); i++) {
			const QColor& color = newPalette.at(i);

			// TODO Should document that updatePalette affects colorPalette. That seems like an important detail.
			colorPalette.push_back(color);

			loadedImage->setColor(i, color.rgb());
		}

		renderer->setPalette(colorPalette);

		emit paletteChanged();
	}
}

void PaletteViewWidget::exportPalette() {
	if (!imageFilename.endsWith(".txt")) {
		imageFilename = imageFilename.mid(0, imageFilename.size() - 4);
		imageFilename += ".txt";
	}

	QString outputFile = QFileDialog::getSaveFileName(this, "Export image palette...", imageFilename, "Palette text file (*.txt)");

	if (!outputFile.isEmpty()) {
		QFile out(outputFile);

		if (out.open(QIODevice::WriteOnly | QIODevice::Text)) {
			std::ofstream outStream(out.fileName().toStdString().c_str());
			outStream << "; " << QString(imageFilename).toStdString() << " - Pekka Kana 2 image palette\n";

			for (int i = 0; i < colorPalette.size(); i++) {
				QColor color = colorPalette.at(i);

				outStream << color.red() << " " << color.green() << " " << color.blue() << "\n";
			}

			outStream.flush();
		} else {
			QMessageBox::critical(this, "Unable to write palette to file!", "Couldn't open file: " + outputFile + "!");
		}
	}
}

void PaletteViewWidget::importPalette() {
	QString selectedFile = QFileDialog::getOpenFileName(this, "Import image palette...", "", "Palette text file (*.txt)");

	if (!selectedFile.isEmpty()) {
		QFile file(selectedFile);

		QList<QColor> newPalette;

		PaletteFileParser parser;
		if (parser.read(file, newPalette)) {
			if (newPalette.size() == 256) {
				colorPalette.clear();

				updatePalette(newPalette);
			} else {
				QString message = "Wrong amount of colors in palette: " 
					+ QString::number(newPalette.size())
					+ "\n"
					+ "Expected colors: " + QString::number(256);

				QMessageBox::warning(this, "Wrong palette size!", message);
			}
		} else {
			QMessageBox::warning(this, "Error!", parser.getErrorMessage());
		}
	}

}

void PaletteViewWidget::importPaletteFromImage() {
	if (loadedImage) {
		QString selectedFile = QFileDialog::getOpenFileName(this, "Import image palette...", "", "8 bit image file (*.bmp)");

		if (!selectedFile.isEmpty()) {
			QImage image(selectedFile);

			QList<uint> newPalette = image.colorTable();

			colorPalette.clear();

			for (int i = 0; i < newPalette.size(); i++) {
				colorPalette.push_back(QColor(image.colorTable()[i]));
			}

			renderer->setPalette(colorPalette);
			loadedImage->setColorTable(newPalette);
			
			emit paletteChanged();
		}
	} else {
		QMessageBox::warning(this, "No image loaded!", "No image has been loaded!");
	}
}


void PaletteViewWidget::setImage(QImage& image, const QString& filename) {
	colorPalette.clear();

	loadedImage = &image;

	for (int color : image.colorTable()) {
		colorPalette.push_back(QColor(color));
	}

	imageFilename = filename;

	clearSelection();

	renderer->setPalette(colorPalette);
}

const QList<QColor>* PaletteViewWidget::getPalette() const {
	return &colorPalette;
}

PaletteRenderer* PaletteViewWidget::getPaletteRenderer() const {
	return renderer;
}

void PaletteViewWidget::setHueOffset(ColorChangeOffset offset) {
	hueOffset = offset;
}

void PaletteViewWidget::setSaturationOffset(ColorChangeOffset offset) {
	saturationOffset = offset;
}

void PaletteViewWidget::setValueOffset(ColorChangeOffset offset) {
	valueOffset = offset;
}