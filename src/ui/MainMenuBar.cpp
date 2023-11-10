#include "MainMenuBar.h"

MainMenuBar::MainMenuBar() {
	setupUI();
}

void MainMenuBar::setupUI() {
	actnImageLoadImage = new QAction("Open", this);
	actnImageLoadImage->setShortcut(QKeySequence("Ctrl+O"));

	actnImageImportImage = new QAction("Convert", this);
	actnImageImportImage->setShortcut(QKeySequence("Ctrl+Shift+O"));

	actnImageSave = new QAction("Save", this);
	actnImageSave->setShortcut(QKeySequence("Ctrl+S"));

	actnImageSaveAs = new QAction("Save As...", this);
	actnImageSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));

	QMenu* mImage = new QMenu("Image", this);
	mImage->addAction(actnImageLoadImage);
	mImage->addAction(actnImageImportImage);
	mImage->addSeparator();
	mImage->addAction(actnImageSave);
	mImage->addAction(actnImageSaveAs);

	actnPaletteExport = new QAction("Export", this);
	actnPaletteExport->setShortcut(QKeySequence("Shift+E"));

	actnPaletteImport = new QAction("Import", this);
	actnPaletteImport->setShortcut(QKeySequence("Shift+I"));

	actnPaletteImportFromImage = new QAction("Import from image...");
	actnPaletteImportFromImage->setShortcut(QKeySequence("Shift+F"));

	QMenu* mPalette = new QMenu("Palette", this);
	mPalette->addAction(actnPaletteExport);
	mPalette->addAction(actnPaletteImport);
	mPalette->addSeparator();
	mPalette->addAction(actnPaletteImportFromImage);

	actnViewRemoveBackground = new QAction("Remove Background", this);
	actnViewRemoveBackground->setCheckable(true);

	actnViewAnimatePalette = new QAction("Animate Palette", this);
	actnViewAnimatePalette->setCheckable(true);

	actnSelectionClear = new QAction("Clear", this);
	actnSelectionClear->setShortcut(QKeySequence("Ctrl+C"));

	actnSelectionSelectAll = new QAction("Select All", this);
	actnSelectionSelectAll->setShortcut(QKeySequence("Ctrl+A"));

	QMenu* mSelection = new QMenu("Selection", this);
	mSelection->addAction(actnSelectionClear);

	QMenu* mView = new QMenu("View", this);
	mView->addAction(actnViewRemoveBackground);
	mView->addAction(actnViewAnimatePalette);

	actnHelpManual = new QAction("Manual", this);
	actnHelpAbout = new QAction("About", this);

	QMenu* mHelp = new QMenu("Help", this);
	mHelp->addAction(actnHelpManual);
	mHelp->addAction(actnHelpAbout);

	addMenu(mImage);
	addMenu(mPalette);
	addMenu(mSelection);
	addMenu(mHelp);
}

const QAction* MainMenuBar::getLoadImageAction() const {
	return actnImageLoadImage;
}

const QAction* MainMenuBar::getSaveAction() const {
	return actnImageSave;
}

const QAction* MainMenuBar::getSaveAsAction() const {
	return actnImageSaveAs;
}

const QAction* MainMenuBar::getPaletteExportAction() const {
	return actnPaletteExport;
}

const QAction* MainMenuBar::getPaletteImportAction() const {
	return actnPaletteImport;
}

const QAction* MainMenuBar::getPaletteImportFromImageAction() const {
	return actnPaletteImportFromImage;
}

const QAction* MainMenuBar::getRemoveBackgroundAction() const {
	return actnViewRemoveBackground;
}

const QAction* MainMenuBar::getAnimatePaletteAction() const {
	return actnViewAnimatePalette;
}

const QAction* MainMenuBar::getHelpManualAction() const {
	return actnHelpManual;
}

const QAction* MainMenuBar::getHelpAboutAction() const {
	return actnHelpAbout;
}

const QAction* MainMenuBar::getImportImageAction() const {
	return actnImageImportImage;
}

const QAction* MainMenuBar::getSelectionClearAction() const {
	return actnSelectionClear;
}

const QAction* MainMenuBar::getSelectAllAction() const {
	return actnSelectionSelectAll;
}