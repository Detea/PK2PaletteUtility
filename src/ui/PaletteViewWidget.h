#pragma once
#include <qwidget.h>
#include <qimage.h>
#include <qspinbox.h>
#include <qpushbutton.h>

#include <qundostack.h>

#include "PaletteRenderer.h"
#include "ColorSelectionBar.h"
#include "ColorChooser.h"
#include "UndoAction.h"

class PaletteViewWidget : public QWidget {
	Q_OBJECT

public:
	enum class ColorChangeOffset {
		Absolute,
		Relative
	};

	PaletteViewWidget();

	void setImage(QImage& image, const QString& filename);
	
	PaletteRenderer* getPaletteRenderer() const;

	const QList<QColor>* getPalette() const;
	
	void undoPaletteChanges(const QList<UndoAction>& actionList);
	void redoPaletteChanges(const QList<UndoAction>& actionList);

public slots:
	void clearSelection();

	void setHueOffset(ColorChangeOffset offset);
	void setSaturationOffset(ColorChangeOffset offset);
	void setValueOffset(ColorChangeOffset offset);

	void exportPalette();
	void importPalette();
	void importPaletteFromImage();

private slots:
	void updateHue(int newHue);
	void updateSaturation(int newSaturation);
	void updateValue(int newValue);

	void updateSelectedColorFromSpinBox();

	void changeHueOffset();
	void changeSaturationOffset();
	void changeValueOffset();

	void commitSelectionColorsChanges(const QColor& newColor);

signals:
	void paletteChanged();

private:
	void setupUI();
	void setupSlots();
	
	void updatePalette(QList<QColor>& newPalette);
	void updateSelection(const QList<int>& list);

	int adjustColor(ColorChangeOffset offset, int oldValue, int newValue, int maxValue, int medianValue = -1);

private:
	PaletteRenderer* renderer = nullptr;

	ColorChooser* colorChooser = nullptr;

	QSpinBox* sbIndex = nullptr;

	QLineEdit* leHexValue = nullptr;

	QList<int> selectedIndicesList;

	QImage* loadedImage = nullptr;

	QList<QColor> colorPalette;

	int selectionMedianValue = 0;

	ColorChangeOffset hueOffset = ColorChangeOffset::Absolute;
	ColorChangeOffset saturationOffset = ColorChangeOffset::Absolute;
	ColorChangeOffset valueOffset = ColorChangeOffset::Absolute;

	QString imageFilename = "";

	QUndoStack* undoStack = nullptr;
	QAction* undoAction = nullptr;
	QAction* redoAction = nullptr;
};

