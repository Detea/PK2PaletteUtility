#include "ImageView.h"
#include <qpainter.h>
#include <qimage.h>
#include <qrect.h>
#include <QWheelEvent>


ImageView::ImageView() {
}

void ImageView::setImage(QImage& image, bool convertToPalettized) {
	if (convertToPalettized) {
		palettizeImage(image);
	} else {
		loadedImage = &image;
	}

	if (removeBackground) {
		removeBackgroundColor();
	}

	setFixedSize(image.width() + 2, image.height() + 2);
	updateGeometry();
}

void ImageView::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	if (loadedImage) {
		painter.setPen(QPen(Qt::lightGray));
		painter.drawRect(0, 0, loadedImage->width() + 1, loadedImage->height() + 1);

		painter.drawImage(1, 1, *loadedImage);
	}
}

void ImageView::doRepaint() {
	repaint();
}

void ImageView::setRemoveBackground(bool remove) {
	removeBackground = remove;

	if (removeBackground) {
		removeBackgroundColor();
	} else {
		resetBackground();
	}

	repaint();
}

void ImageView::removeBackgroundColor() {
	if (loadedImage) {
		const QRgb alphaColor = loadedImage->colorTable().at(255);
		loadedImage->setAlphaChannel(loadedImage->createMaskFromColor(alphaColor, Qt::MaskOutColor));
	}

	repaint();
}

void ImageView::resetBackground() {
}

void ImageView::setPalette(const QList<QColor>* newPalette) {
	palette = newPalette;
}

void ImageView::palettizeImage(QImage& image) {
	if (palette) {
		QList<QRgb> rgbList;
		for (QColor c : *palette) {
			rgbList.push_back(c.rgb());
		}

		*loadedImage = image.convertToFormat(QImage::Format_Indexed8, rgbList);
	}
}