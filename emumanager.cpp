#include "emumanager.h"
#include "ui_emumanager.h"
#include "listviewdelegate.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>

EmuManager::EmuManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmuManager)
{
    ui->setupUi(this);

    ui_emuListView   = findChild<QListView*>("emuListView");
    ui_nameEdit      = findChild<QLineEdit*>("nameEdit");
    ui_descEdit      = findChild<QLineEdit*>("descEdit");
    ui_emuPathEdit   = findChild<QLineEdit*>("emuPathEdit");
    ui_iconButton    = findChild<QPushButton*>("iconButton");
    ui_emuPathButton = findChild<QToolButton*>("emuPathButton");
    ui_updateButton  = findChild<QPushButton*>("updateButton");
    ui_removeButton  = findChild<QPushButton*>("removeButton");
    ui_buttonBox     = findChild<QDialogButtonBox*>("buttonBox");

    // Wire up the emu view to the emu model.  We'll fill in the emu model when we switch systems.
    QStandardItemModel* emuModel = new QStandardItemModel();
    ListviewDelegate* listdelegate = new ListviewDelegate();
    ui_emuListView->setItemDelegate(listdelegate);
    ui_emuListView->setModel(emuModel);
    ui_emuListView->setAlternatingRowColors(true);
    ui_emuListView->setStyleSheet("alternate-background-color: gainsboro;background-color: white;");
    ui_emuListView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Populate the emuView with the loaded emulators.
    foreach (EmulatorData emu, EmuDataMgr::getInstance().getEmuData())
    {
        QIcon icon(emu.iconpath);
        QStandardItem* item = new QStandardItem();
        item->setData(emu.name,ListviewDelegate::headerTextRole);
        item->setData(emu.description,ListviewDelegate::subHeaderTextrole);
        item->setData(icon,ListviewDelegate::IconRole);
        item->setEditable(false);
        emuModel->appendRow(item);
    }

    if (EmuDataMgr::getInstance().getEmuData().size() > 0)
    {
        const EmulatorData emuData = EmuDataMgr::getInstance().getEmuData().first();

        ui_nameEdit->setText(emuData.name);
        ui_descEdit->setText(emuData.description);
        ui_emuPathEdit->setText(emuData.emupath);

        iconPath = emuData.iconpath;

        QIcon icon(iconPath);
        QSize desiredSize(48, 48);
        ui_iconButton->setIcon(icon);
        ui_iconButton->setIconSize(desiredSize);
    }

    validateEmuName(ui_nameEdit->text());

    connect(ui_emuListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this,  SLOT(switchEmulatorSelected()));
    connect(ui_nameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateEmuName(const QString &)));
    connect(ui_emuPathButton, SIGNAL(clicked()), this, SLOT(openEmuPathSelect()));
    connect(ui_iconButton, SIGNAL(clicked()), this, SLOT(openIconSelect()));
    connect(ui_updateButton, SIGNAL(clicked()), this, SLOT(saveEmu()));
    connect(ui_removeButton, SIGNAL(clicked()), this, SLOT(removeEmu()));
    connect(ui_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

EmuManager::~EmuManager()
{
    delete ui;
}

void EmuManager::switchEmulatorSelected()
{
    currentEmuRow = ui_emuListView->currentIndex().row();
    QString emuName = (QString)ui_emuListView->model()->data(ui_emuListView->currentIndex(), ListviewDelegate::headerTextRole).toString();

    const EmulatorData emuData = EmuDataMgr::getInstance().getEmuData()[emuName];

    ui_nameEdit->setText(emuData.name);
    ui_descEdit->setText(emuData.description);
    ui_emuPathEdit->setText(emuData.emupath);

    iconPath = emuData.iconpath;

    QIcon icon(iconPath);
    QSize desiredSize(48, 48);
    ui_iconButton->setIcon(icon);
    ui_iconButton->setIconSize(desiredSize);
}

void EmuManager::validateEmuName(const QString& newName)
{
    if (EmuDataMgr::getInstance().getEmuData().contains(newName))
    {
        ui_updateButton->setText("Update Emu");
        ui_removeButton->setEnabled(true);
    }
    else
    {
        ui_updateButton->setText("Save As New");
        ui_removeButton->setDisabled(true);
    }
}

void EmuManager::openEmuPathSelect()
{
    QString newEmuPath = QFileDialog::getOpenFileName(this, "Select Emulator Path", ui_emuPathEdit->text());
    if (!newEmuPath.isEmpty())
    {
        ui_emuPathEdit->setText(newEmuPath);
    }
}

void EmuManager::openIconSelect()
{
    QString newIconPath = QFileDialog::getOpenFileName(this, "Select Icon", iconPath, "Icons (*.png *.ico *.bmp)");
    if (!newIconPath.isEmpty())
    {
        iconPath = newIconPath;
        QIcon icon(iconPath);
        QSize desiredSize(48, 48);
        ui_iconButton->setIcon(icon);
        ui_iconButton->setIconSize(desiredSize);
    }
}

void EmuManager::saveEmu()
{
    EmulatorData emuData;
    emuData.name = ui_nameEdit->text();
    emuData.description = ui_descEdit->text();
    emuData.emupath = ui_emuPathEdit->text();
    emuData.iconpath = iconPath;

    QIcon icon(emuData.iconpath);
    QStandardItem* item = new QStandardItem();
    item->setData(emuData.name,ListviewDelegate::headerTextRole);
    item->setData(emuData.description,ListviewDelegate::subHeaderTextrole);
    item->setData(icon,ListviewDelegate::IconRole);
    item->setEditable(false);

    QStandardItemModel* emuModel = (QStandardItemModel*)ui_emuListView->model();
    emuModel->appendRow(item);

    EmuDataMgr::getInstance().getEmuData().insert(emuData.name, emuData);
    EmuDataMgr::getInstance().saveEmuData();

    validateEmuName(emuData.name);
}

void EmuManager::removeEmu()
{
    QString emuName = (QString)ui_emuListView->model()->data(ui_emuListView->currentIndex(), ListviewDelegate::headerTextRole).toString();
    EmulatorData emuData = EmuDataMgr::getInstance().getEmuData()[emuName];

    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to remove emulator " + emuData.name + "?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok)
    {
         EmuDataMgr::getInstance().getEmuData().remove(emuData.name);
         EmuDataMgr::getInstance().saveEmuData();

    }
    ui_emuListView->model()->removeRow(currentEmuRow);

    validateEmuName(emuData.name);
}
