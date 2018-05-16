#ifndef EMULATORCONFIG_H
#define EMULATORCONFIG_H

#include "emulatordata.h"
#include "systemdata.h"
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QComboBox>

namespace Ui {
class EmulatorConfig;
}

class EmulatorConfig : public QDialog
{
    Q_OBJECT

public:
    explicit EmulatorConfig(SystemData* sys, int index, QWidget *parent = 0);
    ~EmulatorConfig();

    void accept();

public slots:
    void openEmuMgrWindow();

private:
    Ui::EmulatorConfig *ui;

    QPushButton* ui_emuMgrButton;
    QComboBox* ui_emulatorNames;
    QLineEdit* ui_paramsEdit;
    QDialogButtonBox* ui_buttonBox;

    SystemData* system;
    int indexToUpdate;

    void refreshEmuComboBox();
};

#endif // EMULATORCONFIG_H
