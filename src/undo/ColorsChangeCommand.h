#pragma once
#include <qundostack.h>

#include "PaletteViewWidget.h"
#include "UndoAction.h"

class ColorsChangeCommand : public QUndoCommand {
public:
	ColorsChangeCommand(QList<UndoAction> actionList, PaletteViewWidget* paletteView);

	void undo() override;
	void redo() override;

private:
	QList<UndoAction> actionList;
	PaletteViewWidget* paletteViewWidget;
};

