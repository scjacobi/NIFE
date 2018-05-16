#include "emulatorconfig.h"
#include "ui_emulatorconfig.h"
#include "emumanager.h"
#include <QMessageBox>

EmulatorConfig::EmulatorConfig(SystemData* sys, int index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmulatorConfig),
    system(sys),
    indexToUpdate(index)
{
    ui->setupUi(this);

    ui_emuMgrButton  = findChild<QPushButton*>("emuMgrButton");
    ui_emulatorNames = findChild<QComboBox*>("emulatorBox");
    ui_paramsEdit    = findChild<QLineEdit*>("paramsEdit");
    ui_buttonBox     = findChild<QDialogButtonBox*>("buttonBox");

    if (EmuDataMgr::getInstance().getEmuData().size() > 0)
    {
        ui_emulatorNames->addItems(EmuDataMgr::getInstance().getEmuData().keys());
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("It looks like you don't have any emulators configured.\nPlease click on the Emulator Manager button to add one.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    if (indexToUpdate >= 0)
    {
        ui_emulatorNames->setCurrentText(system->emuNames[indexToUpdate]);
        ui_paramsEdit->setText(system->emuParams[indexToUpdate]);
    }

    connect(ui_emuMgrButton, SIGNAL(clicked(bool)), this, SLOT(openEmuMgrWindow()));
    connect(ui_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

EmulatorConfig::~EmulatorConfig()
{
    delete ui;
}

void EmulatorConfig::openEmuMgrWindow()
{
    EmuManager emuMgr(this);
    emuMgr.exec();
    refreshEmuComboBox();
}

void EmulatorConfig::refreshEmuComboBox()
{
    ui_emulatorNames->clear();
    if (EmuDataMgr::getInstance().getEmuData().size() > 0)
    {
        ui_emulatorNames->addItems(EmuDataMgr::getInstance().getEmuData().keys());
    }
}

void EmulatorConfig::accept()
{
    QString name = ui_emulatorNames->currentText();
    QString params = ui_paramsEdit->text();

    if (indexToUpdate >= 0)
    {
        system->emuNames[indexToUpdate] = name;
        system->emuParams[indexToUpdate] = params;
    }
    else
    {
        system->emuNames.append(name);
        system->emuParams.append(params);
    }

    QDialog::accept();
}
