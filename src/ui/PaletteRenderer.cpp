#include "PaletteRenderer.h"
#include "PaletteUtils.h"

#include <qpainter.h>

PaletteRenderer::PaletteRenderer() {
	// ENTRY_GAP is used to offset the entire palette here. This is dumb. There should be an ENTRY_OFFSET variable, but it's convenient.
	setFixedSize((ENTRY_SIZE_WITH_GAP * ENTRY_AMOUNT) + ENTRY_GAP, (ENTRY_SIZE_WITH_GAP * ENTRY_AMOUNT) + ENTRY_GAP);

	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);
}

void PaletteRenderer::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setPen(pen);

	pen.setStyle(Qt::SolidLine);

	// Fill the background
	painter.fillRect(0, 0, width(), height(), Qt::black);

	// Draw the colored squares
	if (!palette.isEmpty()) {
		for (int x = 0; x < ENTRY_AMOUNT; x++) {
			for (int y = 0; y < ENTRY_AMOUNT; y++) {

				// ENTRY_GAP is added to the position of each entry, because they are offset. They aren't supposed to be drawn at (0, 0), because the palette should have a border.
				painter.fillRect((x * ENTRY_SIZE_WITH_GAP) + ENTRY_GAP, (y * ENTRY_SIZE_WITH_GAP) + ENTRY_GAP, ENTRY_SIZE, ENTRY_SIZE, QBrush(palette.at(x + ENTRY_AMOUNT * y))); // Optimizaton: Might want to cache a QBrush and set use setColor()? Probably doesn't matter.
			}
		}

		// Draw the highlights around the selected squares
		painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
		pen.setColor(Qt::white);
		pen.setStyle(Qt::DashLine);

		for (int index : selectionList) {
			auto [entryX, entryY] = PaletteUtils::positionFromIndex(index, ENTRY_AMOUNT, ENTRY_AMOUNT, ENTRY_SIZE_WITH_GAP);
			painter.drawRect(entryX, entryY, ENTRY_SIZE_WITH_GAP, ENTRY_SIZE_WITH_GAP);	// ENTRY_SIZE_WITH_GAP is used, because the selection rect surrounds the entry rect.
		}
	}
}

void PaletteRenderer::handleMouseEvents(QMouseEvent* event) {
	if (!palette.isEmpty()) {
		mousePosition = event->pos();

		if (mousePosition.x() >= 0 && mousePosition.x() < width() - 1 && mousePosition.y() >= 0 && mousePosition.y() <= height()) { // I subtract 1 from width to fix a bug that I have no idea about why it occurs. This might be a hacky fix... but it works!!
			int colorIndex = PaletteUtils::indexFromPosition(mousePosition.x(), mousePosition.y(), ENTRY_SIZE_WITH_GAP, ENTRY_AMOUNT);

			if (event->buttons() & Qt::LeftButton) {
				addIndexToSelection(colorIndex);
			} else if (event->buttons() & Qt::RightButton) {
				removeIndexFromSelection(colorIndex);
			}

			repaint();
		}
	}
}

void PaletteRenderer::mousePressEvent(QMouseEvent* event) {
	// The selection gets reset everytime the user clicks on the palette, unless they are holding down CTRL, which is the multiple selection mode.
	if (!multiSelect) {
		if (event->buttons() & Qt::LeftButton) {
			selectionList.clear();
		}
	}

	handleMouseEvents(event);

	// Emit the signal when the user clicks on a color and doesn't hold down the CTRL key, to immediately update the UI.
	if (!multiSelect) {
		if (selectionList.size() == 1) {
			emit selectionChanged(selectionList);
		}
	}
}

void PaletteRenderer::mouseReleaseEvent(QMouseEvent* event) {
	// Send the signal after the user is done dragging. This only updates the UI after the dragging, not during, leading to a less messy presentation.
	if (selectionList.size() > 1) {
		emit selectionChanged(selectionList);
	}
}

void PaletteRenderer::mouseMoveEvent(QMouseEvent* event) {
	/*
		This seems pretty hacky to me...
		
		But this needs to be done for the keyboard to work. If the user doesn't hold down CTRL before dragging multi select won't work.

		Sidenote: This sometimes might not work? Sometimes when the program starts this doesn't work. Most of the time it does. Might be something else...
	*/
	if (!hasFocus()) setFocus();

	if (multiSelect) handleMouseEvents(event);
}

void PaletteRenderer::setColorsInPalette(const QList<int>& indexList, const QList<QColor>& colorsList) {
	for (int i = 0; i < indexList.size(); i++) {
		palette[indexList[i]] = colorsList[i];
	}

	repaint();
}

void PaletteRenderer::setSelectedIndex(int index) {
	selectionList.clear();
	selectionList.push_back(index);

	repaint();

	emit selectionChanged(selectionList);
}

void PaletteRenderer::setPalette(const QList<QColor>& list) {
	palette = list;
}

QList<int> PaletteRenderer::getSelectedColors() const {
	return selectionList;
}

void PaletteRenderer::addIndexToSelection(int index) {
	if (!selectionList.contains(index)) {
		selectionList.push_back(index);
	}
}

void PaletteRenderer::removeIndexFromSelection(int index) {
	selectionList.removeAll(index);
}

void PaletteRenderer::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key::Key_Control) {
		multiSelect = true;
	}
}

void PaletteRenderer::keyReleaseEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key::Key_Control) {
		multiSelect = false;
	}
}

void PaletteRenderer::clearSelection() {
	selectionList.clear();

	repaint();

	emit selectionChanged(selectionList);
}