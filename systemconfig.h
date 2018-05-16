#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include "systemdata.h"
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QTableWidget>

namespace Ui {
class SystemConfig;
}

class SystemConfig : public QDialog
{
    Q_OBJECT

public:
    explicit SystemConfig(SystemData* system, QWidget *parent = 0);
    ~SystemConfig();

    void accept();

public slots:
    void openIconSelect();
    void openRomPathSelect();
    void removeRomPath();
    void openArtPath1Select();
    void openArtPath2Select();

private:
    Ui::SystemConfig *ui;

    QLineEdit* ui_nameEdit;
    QLineEdit* ui_descEdit;

    QPushButton* ui_iconButton;

    QToolButton* ui_addRomPathButton;
    QToolButton* ui_delRomPathButton;

    QTableWidget* ui_altRomPathTable;

    QLineEdit* ui_artPath1Edit;
    QLineEdit* ui_artPath2Edit;
    QToolButton* ui_artPath1Button;
    QToolButton* ui_artPath2Button;

    QString iconPath;

    SystemData* systemData;
};

#endif // SYSTEMCONFIG_H
