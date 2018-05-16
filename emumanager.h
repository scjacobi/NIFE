#ifndef EMUMANAGER_H
#define EMUMANAGER_H

#include "emulatordata.h"
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QListView>
#include <QDialogButtonBox>

namespace Ui {
class EmuManager;
}

class EmuManager : public QDialog
{
    Q_OBJECT

public:
    explicit EmuManager(QWidget *parent = 0);
    ~EmuManager();

public slots:
    void switchEmulatorSelected();
    void openEmuPathSelect();
    void openIconSelect();
    void saveEmu();
    void removeEmu();
    void validateEmuName(const QString& newName);

private:
    Ui::EmuManager *ui;

    QListView* ui_emuListView;

    QLineEdit* ui_nameEdit;
    QLineEdit* ui_descEdit;
    QLineEdit* ui_emuPathEdit;

    QPushButton* ui_iconButton;
    QToolButton* ui_emuPathButton;
    QPushButton* ui_updateButton;
    QPushButton* ui_removeButton;
    QDialogButtonBox* ui_buttonBox;

    QString iconPath;

    int currentEmuRow;
};

#endif // EMUMANAGER_H
