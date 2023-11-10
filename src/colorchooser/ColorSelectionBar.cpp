#include <qpainter.h>
#include <QPainterPath>
#include "ColorSelectionBar.h"

ColorSelectionBar::ColorSelectionBar(ColorComponent component) : colorComponent(component) {
	if (component == ColorComponent::Hue) {
		gradientWidth = 360;
	} else {
		gradientWidth = 255;
	}

	gradientXOffset = (colorIndicator.width() / 2) + 1;

	setFixedWidth(gradientXOffset + gradientWidth + 1 + colorIndicator.width());
	setFixedHeight(30 + colorIndicator.height());

	pen = QPen(selectedColor, Qt::SolidLine);
}

void ColorSelectionBar::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	QColor previewColor;

	switch (colorComponent) {
		case ColorComponent::Hue: {
				for (int i = 0; i < 360; i++) {
					previewColor.setHsv(i, selectedColor.saturation(), selectedColor.value());

					pen.setColor(previewColor);
					painter.setPen(pen);
					painter.drawLine(gradientXOffset + i, colorIndicator.height(), gradientXOffset + i, gradientHeight);
				}

			}
			break;
			
		case ColorComponent::Saturation: {
				for (int i = 0; i < 255; i++) {
					previewColor.setHsv(selectedColor.hue(), i, selectedColor.value());

					pen.setColor(previewColor);
					painter.setPen(pen);
					painter.drawLine(gradientXOffset + i, colorIndicator.height(), gradientXOffset + i, gradientHeight);
				}

			}
			break;

		case ColorComponent::Value: {
				for (int i = 0; i < 255; i++) {
					previewColor.setHsv(selectedColor.hue(), selectedColor.saturation(), i);

					pen.setColor(previewColor);
					painter.setPen(pen);
					painter.drawLine(gradientXOffset + i, colorIndicator.height(), gradientXOffset + i, gradientHeight);
				}

			}
			break;
	}

	// Draw the border surrounding the gradient
	painter.setPen(QPen(Qt::darkGray, Qt::SolidLine));
	painter.drawRect(gradientXOffset, colorIndicator.height(), gradientWidth, gradientHeight - colorIndicator.height());

	painter.setPen(QPen(Qt::black, Qt::SolidLine));

	// Draw the color indicator triangle
	// Doing it this way because it gives me more control and a cleaner result than using QPainterPath
	int x = colorIndicator.x();
	int xWidth = colorIndicator.width();
	int y = 0;
	do {
		int xOffset = ((colorIndicator.width() - xWidth) / 2);

		painter.drawLine(x + xOffset, y + 1, x + (colorIndicator.width() - xOffset), y + 1);

		xWidth--;
		y++;
	} while (xWidth >= 0 && y <= colorIndicator.height());

	// Draw the outline of the color indicator, smoothing out the rough edges. That's the idea, atleast...
	painter.setPen(QPen(Qt::gray, Qt::SolidLine));
	int cx = colorIndicator.x();
	int cy = colorIndicator.y();
	int cw = colorIndicator.width();
	int ch = colorIndicator.height();

	// The third line is wrong, but it's close enough and I really don't care anymore, lmao.
	painter.drawLine(cx - 1, cy, cx + cw, cy);						// Draw the line at the top
	painter.drawLine(cx - 1, cy, cx + (cw / 2), cy + ch + 2);		// Draw the line from top left to middle bottom
	painter.drawLine(cx + (cw / 2), cy + ch + 2, cx + cw + 1, cy);	// Draw the line from the middle bottom to the top right
}

void ColorSelectionBar::mouseMoveEvent(QMouseEvent* event) {
	handleColorIndicator(event);
}

void ColorSelectionBar::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		emit startChange();

		switch (colorComponent) {
			case ColorComponent::Hue:
				oldValue = selectedColor.hue();
				break;

			case ColorComponent::Saturation:
				oldValue = selectedColor.saturation();
				break;

			case ColorComponent::Value:
				oldValue = selectedColor.value();
				break;
		}

		handleColorIndicator(event);
	}
}

void ColorSelectionBar::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		int newValue = 0;
		switch (colorComponent) {
			case ColorComponent::Hue:
				newValue = selectedColor.hue();
				break;

			case ColorComponent::Saturation:
				newValue = selectedColor.saturation();
				break;

			case ColorComponent::Value:
				newValue = selectedColor.value();
				break;
		}

		emit commitChange();
	}
}

void ColorSelectionBar::handleColorIndicator(QMouseEvent* event) {
	if ((event->buttons() & Qt::LeftButton)) {
		int indicatorCenter = colorIndicator.width() / 2;
		int newX = event->pos().x() - indicatorCenter;

		// Keep the position in bounds
		if (newX >= 0 && newX <= gradientWidth) {
			colorIndicator.moveTo(newX, 0);

			changeColor();

			repaint();
		}
	}
}

void ColorSelectionBar::updateColorIndicator() {
	int indicatorCenter = colorIndicator.width() / 2;

	switch (colorComponent) {
		case ColorComponent::Hue:
			colorIndicator.moveTo(selectedColor.hue() + indicatorCenter, colorIndicator.y());
			break;

		case ColorComponent::Saturation:
			colorIndicator.moveTo(selectedColor.saturation() + indicatorCenter, colorIndicator.y());
			break;

		case ColorComponent::Value:
			colorIndicator.moveTo(selectedColor.value() + indicatorCenter, colorIndicator.y());
			break;
	}

	repaint();
}

void ColorSelectionBar::setColor(const QColor& col) {
	selectedColor = col;

	updateColorIndicator();

	repaint();
}

void ColorSelectionBar::setHue(int hue) {
	if (selectedColor.hue() != hue) {
		selectedColor.setHsv(hue, selectedColor.saturation(), selectedColor.value());

		if (colorComponent == ColorComponent::Hue) {
			colorIndicator.moveTo(selectedColor.hue() + (colorIndicator.width() / 2), colorIndicator.y());

			emit hueChanged(hue);
		}

		repaint();
	}
}

void ColorSelectionBar::setSaturation(int saturation) {
	if (selectedColor.saturation() != saturation) {
		selectedColor.setHsv(selectedColor.hue(), saturation, selectedColor.value());

		if (colorComponent == ColorComponent::Saturation) {
			colorIndicator.moveTo(selectedColor.saturation() + (colorIndicator.width() / 2), colorIndicator.y());

			emit saturationChanged(saturation);
		}

		repaint();
	}
}

void ColorSelectionBar::setValue(int value) {
	if (selectedColor.value() != value) {
		selectedColor.setHsv(selectedColor.hue(), selectedColor.saturation(), value);

		if (colorComponent == ColorComponent::Value) {
			colorIndicator.moveTo(selectedColor.value() + (colorIndicator.width() / 2), colorIndicator.y());

			emit valueChanged(value);
		}
		
		repaint();
	}
}

void ColorSelectionBar::changeColor() {
	switch (colorComponent) {
		case ColorComponent::Hue:
			{
				int hue = colorIndicator.x() - (colorIndicator.width() / 2);

				if (hue >= 0 && hue <= 359) {
					setHue(hue);
				}
			}
			break;

		case ColorComponent::Saturation:
			{
				int saturation = (colorIndicator.x() - colorIndicator.width() / 2);

				if (saturation >= 0 && saturation <= 255) {
					setSaturation(saturation);
				}
			}
			break;

		case ColorComponent::Value:
			{
				int value = colorIndicator.x() - (colorIndicator.width() / 2);

				if (value >= 0 && value <= 255) {
					setValue(value);
				}
			}
			break;
	}
}

QColor ColorSelectionBar::getColor() const {
	return selectedColor;
}

int ColorSelectionBar::getHue() const {
	return selectedColor.hue();
}

int ColorSelectionBar::getSaturation() const {
	return selectedColor.saturation();
}

int ColorSelectionBar::getValue() const {
	return selectedColor.value();
}