#pragma once

#include <QtWidgets/QMainWindow>
#include "ImageView.h"
#include "PaletteViewWidget.h"
#include "MainMenuBar.h"

#include <string_view>

class PK2PaletteUtility : public QMainWindow {
    Q_OBJECT

public:
    PK2PaletteUtility(QWidget *parent = nullptr);
    ~PK2PaletteUtility();

    void loadFile(const QString& filename);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUI();
    void setupSlots();

    void updateWindowTitle(const QString& newTitle);

private slots:
    void loadFileFromMenu();
    void saveFile();

    void saveFileAs();

    void importImage();
    void removeBackgroundColor();

    void setSelectedColors(const QList<int>& list);

    void clearSelection();

    void updateSaveReminder();

    void openManual();
    void showAboutDialog();

private:
    static constexpr std::string_view WINDOW_TITLE = "Pekka Kana 2 Palette Utility";
    static constexpr std::string_view VERSION_STRING = "1.0";

    ImageView imageView;
    PaletteViewWidget paletteView;

    MainMenuBar* mainMenu;

    QString loadedFile;

    QList<int> selectedColors;

    QImage image;

    bool changesUnsaved = false;
};
