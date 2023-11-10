#pragma once
#include <qmenubar.h>

class MainMenuBar : public QMenuBar {
public:
	MainMenuBar();

	const QAction* getLoadImageAction() const;
	const QAction* getImportImageAction() const;
	const QAction* getSaveAction() const;
	const QAction* getSaveAsAction() const;

	const QAction* getPaletteExportAction() const;
	const QAction* getPaletteImportAction() const;
	const QAction* getPaletteImportFromImageAction() const;

	const QAction* getSelectionClearAction() const;
	const QAction* getSelectAllAction() const;

	const QAction* getRemoveBackgroundAction() const;
	const QAction* getAnimatePaletteAction() const;
	
	const QAction* getHelpManualAction() const;
	const QAction* getHelpAboutAction() const;

private:
	void setupUI();

	QAction* actnImageLoadImage = nullptr;
	QAction* actnImageImportImage = nullptr;
	QAction* actnImageSave = nullptr;
	QAction* actnImageSaveAs = nullptr;

	QAction* actnPaletteExport = nullptr;
	QAction* actnPaletteImport = nullptr;
	QAction* actnPaletteImportFromImage = nullptr;

	QAction* actnSelectionClear = nullptr;
	QAction* actnSelectionSelectAll = nullptr;

	QAction* actnViewRemoveBackground = nullptr;
	QAction* actnViewAnimatePalette = nullptr;

	QAction* actnHelpManual = nullptr;
	QAction* actnHelpAbout = nullptr;
};