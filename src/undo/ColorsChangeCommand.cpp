#include "ColorsChangeCommand.h"

ColorsChangeCommand::ColorsChangeCommand(QList<UndoAction> actionList, PaletteViewWidget* paletteView) : actionList(actionList), paletteViewWidget(paletteView) {
}

void ColorsChangeCommand::undo() {
	if (paletteViewWidget) {
		paletteViewWidget->undoPaletteChanges(actionList);
	}
}

void ColorsChangeCommand::redo() {
	if (paletteViewWidget) {
		paletteViewWidget->redoPaletteChanges(actionList);
	}
}