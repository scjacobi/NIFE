#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "systemdata.h"
#include "systemconfig.h"
#include "emulatordata.h"
#include "emulatorconfig.h"
#include "kbeventlistview.h"
#include "nifesettings.h"
#include <QMainWindow>
#include <QListView>
#include <QSplitter>
#include <QTabWidget>
#include <QVector>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent* event);

public slots:
    void switchSystemSelected();

    void showSystemContextMenu(const QPoint &pos);
    void addSystem();
    void removeSystem();
    void editSystem();
    void upSystem();
    void downSystem();

    void showEmulatorContextMenu(const QPoint &pos);
    void addEmu();
    void removeEmu();
    void editEmu();
    void upEmu();
    void downEmu();

    void openEmuMgrConfig();
    void filterRomView();
    int toolBarLaunchEmulator();
    int launchEmulator(const QModelIndex& selectedRomIndex);

    void resizeArt();
    void updateArt();
    void toggleArt();

private:
    Ui::MainWindow* ui;

    ManipulateListView* sysView;
    ManipulateListView* emuView;

    QTabWidget* romTabs;

    QLineEdit* filterText;
    QPushButton* doFilter;
    QHBoxLayout* filterLayout;
    QVBoxLayout* romLayout;
    QWidget* romWidget;

    QPixmap art1Pixmap;
    QPixmap art2Pixmap;
    QLabel* art1Image;
    QLabel* art2Image;
    QVBoxLayout* artLayout;
    QWidget* artWidget;

    QSplitter* horizSplitter;
    QSplitter* vertSplitter;
    QSplitter* artHorizSplitter;

    SystemConfig* sysConfig;
    EmulatorConfig* emuConfig;

    QFileSystemModel* filemodel;

    NifeSettings settings;

    int currentSystemRow;
    QList<int> artworkSizes;

    SystemData getCurrentSystem() { return SysDataMgr::getInstance().getSysData()[currentSystemRow]; }

    void updateAndSaveCurrentSystem(SystemData system)
    {
        SysDataMgr::getInstance().getSysData()[currentSystemRow] = system;
        SysDataMgr::getInstance().saveSysData();
    }
};

#endif // MAINWINDOW_H
