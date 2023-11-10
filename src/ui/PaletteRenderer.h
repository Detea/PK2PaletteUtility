#pragma once
#include <qwidget.h>
#include <QMouseEvent>
#include <qfont.h>
#include <qpen.h>
#include <qlist.h>

class PaletteRenderer : public QWidget {
	Q_OBJECT

public:
	PaletteRenderer();

	// Returns a list of the selected indices in the color palette
	QList<int> getSelectedColors() const;

	void setPalette(const QList<QColor>& list);
	void setColorsInPalette(const QList<int>& indexList, const QList<QColor>& colorsList);
	
	void setSelectedIndex(int index);

public slots:
	void clearSelection();

signals:
	void selectionChanged(const QList<int>& selectionList);

protected:
	void paintEvent(QPaintEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

private:
	void handleMouseEvents(QMouseEvent* event);

	void addIndexToSelection(int index);
	void removeIndexFromSelection(int index);

private:
	static constexpr int ENTRY_AMOUNT = 16;

	static constexpr int ENTRY_SIZE = 22;
	static constexpr int ENTRY_GAP = 1;
	static constexpr int ENTRY_SIZE_WITH_GAP = ENTRY_SIZE + ENTRY_GAP;

	QPen pen;

	QList<QColor> palette;

	QList<int> selectionList;

	QPoint mousePosition{ -1, -1 };

	bool multiSelect = false;
};

