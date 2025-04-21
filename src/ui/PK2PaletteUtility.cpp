#include "PK2PaletteUtility.h"

#include <qfiledialog.h>
#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qscrollarea.h>
#include <qdesktopservices.h>
#include <qicon.h>

#include <filesystem>

PK2PaletteUtility::PK2PaletteUtility(QWidget *parent) : QMainWindow(parent) {
	setupUI();
	setupSlots();
}

void PK2PaletteUtility::setupUI() {
	QScrollArea* saImageView = new QScrollArea(this);
	saImageView->setWidget(&imageView);
	saImageView->setBackgroundRole(QPalette::Dark);
	saImageView->setAlignment(Qt::AlignCenter);

	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(2);
	hlayout->addWidget(saImageView);
	hlayout->addWidget(&paletteView);

	QWidget* w = new QWidget(this);
	w->setLayout(hlayout);
	setCentralWidget(w);

	mainMenu = new MainMenuBar();
	setMenuBar(mainMenu);

	setMinimumSize(1024, 768);
	setWindowTitle(WINDOW_TITLE.data());

	setWindowIcon(QIcon(":pu_icon64x64.png"));
}

void PK2PaletteUtility::setupSlots() {
	connect(mainMenu->getLoadImageAction(), &QAction::triggered, this, &PK2PaletteUtility::loadFileFromMenu);
	connect(mainMenu->getImportImageAction(), &QAction::triggered, this, &PK2PaletteUtility::importImage);
	connect(mainMenu->getRemoveBackgroundAction(), &QAction::triggered, this, &PK2PaletteUtility::removeBackgroundColor);
	
	connect(mainMenu->getSaveAction(), &QAction::triggered, this, &PK2PaletteUtility::saveFile);
	connect(mainMenu->getSaveAsAction(), &QAction::triggered, this, &PK2PaletteUtility::saveFileAs);

	connect(paletteView.getPaletteRenderer(), &PaletteRenderer::selectionChanged, this, &PK2PaletteUtility::setSelectedColors);
	connect(mainMenu->getSelectionClearAction(), &QAction::triggered, this, &PK2PaletteUtility::clearSelection);

	connect(mainMenu->getPaletteExportAction(), &QAction::triggered, &paletteView, &PaletteViewWidget::exportPalette);
	connect(mainMenu->getPaletteImportAction(), &QAction::triggered, &paletteView, &PaletteViewWidget::importPalette);
	connect(mainMenu->getPaletteImportFromImageAction(), &QAction::triggered, &paletteView, &PaletteViewWidget::importPaletteFromImage);

	connect(&paletteView, &PaletteViewWidget::paletteChanged, &imageView, &ImageView::doRepaint);
	connect(&paletteView, &PaletteViewWidget::paletteChanged, this, &PK2PaletteUtility::updateSaveReminder);

	connect(mainMenu->getHelpManualAction(), &QAction::triggered, this, &PK2PaletteUtility::openManual);
	connect(mainMenu->getHelpAboutAction(), &QAction::triggered, this, &PK2PaletteUtility::showAboutDialog);
}

void PK2PaletteUtility::loadFileFromMenu() {
	QString filename = QFileDialog::getOpenFileName(this, "Open a BMP image file...", "", tr("8 bit image file (*.bmp)"));
	
	if (!filename.isEmpty()) {
		loadFile(filename);
	}
}

void PK2PaletteUtility::loadFile(const QString& filename) {
	image = QImage(filename);

	if (image.format() == QImage::Format_Indexed8) {
		loadedFile = filename;

		std::filesystem::path file(filename.toLatin1().data());
		
		imageView.setImage(image);
		paletteView.setImage(image, QString::fromLatin1(file.filename().string()));

		updateWindowTitle(filename);
	} else {
		QMessageBox::critical(this, "Wrong image format!", "Image must use a 256 color palette!");
	}
}

void PK2PaletteUtility::saveFile() {
	if (!loadedFile.isEmpty()) {
		image.save(loadedFile);
		changesUnsaved = false;

		updateWindowTitle(loadedFile);
	}
}

void PK2PaletteUtility::saveFileAs() {
	if (!loadedFile.isEmpty()) {
		QString newFile = QFileDialog::getSaveFileName(this, "Save image file as...", "", "8 bit image file (*.bmp)");

		loadedFile = newFile;

		saveFile();
	}
}

void PK2PaletteUtility::importImage() {
	if (!loadedFile.isEmpty()) {
		QString filename = QFileDialog::getOpenFileName(this, "Open an image file...", "", "Image file (*.png *.jpg *.jpeg)");

		if (!filename.isEmpty()) {
			loadedFile = filename;

			image = QImage(filename);

			imageView.setPalette(paletteView.getPalette());
			imageView.setImage(image, true);

			updateWindowTitle(filename);
		}
	} else {
		QMessageBox::warning(this, "No palette loaded!", "No image or palette has been loaded!");
	}
}

void PK2PaletteUtility::updateWindowTitle(const QString& newTitle) {
	QString title = newTitle;
	if (changesUnsaved) title += "*";

	setWindowTitle(title + " - " + WINDOW_TITLE.data());
}

void PK2PaletteUtility::closeEvent(QCloseEvent* event) {
	event->ignore();

	if (changesUnsaved) {
		QMessageBox msg(this);
		msg.setText("The palette has been modified.");
		msg.setInformativeText("Do you want to save your changes?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msg.setDefaultButton(QMessageBox::Save);
		msg.setMinimumSize(250, 150);

		int result = msg.exec();

		switch (result) {
			case QMessageBox::Save:
				saveFile();
				[[fallthrough]];

			case QMessageBox::Discard:
				QMainWindow::closeEvent(event);
				break;
		}
	} else {
		QMainWindow::closeEvent(event);
	}
}

void PK2PaletteUtility::removeBackgroundColor() {
	imageView.setRemoveBackground(mainMenu->getRemoveBackgroundAction()->isChecked());
}

void PK2PaletteUtility::setSelectedColors(const QList<int>& list) {
	selectedColors = list;
}

void PK2PaletteUtility::clearSelection() {
	paletteView.getPaletteRenderer()->clearSelection();
}

void PK2PaletteUtility::updateSaveReminder() {
	if (!loadedFile.isEmpty()) {
		if (!changesUnsaved) changesUnsaved = true;

		updateWindowTitle(loadedFile);
	}
}

void PK2PaletteUtility::showAboutDialog() {
	QString aboutText = "<center><b>Pekka Kana 2 Palette Utility</b>";
	aboutText += "<br />";
	aboutText += "Version: ";
	aboutText += QString::fromUtf8(VERSION_STRING);
	aboutText += "<hr />";
	aboutText += "This project is open source:<br />";
	aboutText += "<a href = \"https://github.com/Detea/PK2PaletteUtility\">https://github.com/Detea/PK2PaletteUtility</a></center>";
	aboutText += "<hr />";
	aboutText += "Application icon made by MAKYUNI";

	QMessageBox mbAbout(this);
	mbAbout.setWindowTitle("About");
	mbAbout.setTextFormat(Qt::RichText);
	mbAbout.setText(aboutText);
	mbAbout.exec();
}

void PK2PaletteUtility::openManual() {
	QDesktopServices::openUrl(QUrl(R"(https://detea.github.io/pekapu)"));
}

PK2PaletteUtility::~PK2PaletteUtility()
{}
