#pragma once
#include <qimage.h>
#include <qimage.h>
#include <qwidget.h>
#include <QWheelEvent>

class ImageView : public QWidget {
	Q_OBJECT

public:
	ImageView();

	void setImage(QImage& image, bool convertToPalettized = false);
	void setPalette(const QList<QColor>* newPalette);

	void setRemoveBackground(bool remove);

public:
	void doRepaint();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	void removeBackgroundColor();
	void resetBackground();

	void palettizeImage(QImage& image);

private:
	QImage* loadedImage = nullptr;
	const QList<QColor>* palette = nullptr;

	bool removeBackground = false;
};