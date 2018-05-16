#include "systemconfig.h"
#include "ui_systemconfig.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

SystemConfig::SystemConfig(SystemData* sysData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemConfig),
    systemData(sysData)
{
    ui->setupUi(this);

    ui_nameEdit      = findChild<QLineEdit*>("nameEdit");
    ui_descEdit      = findChild<QLineEdit*>("descEdit");
    ui_iconButton    = findChild<QPushButton*>("iconButton");

    ui_addRomPathButton = findChild<QToolButton*>("addRomPath");
    ui_delRomPathButton = findChild<QToolButton*>("delRomPath");
    ui_altRomPathTable  = findChild<QTableWidget*>("altRomPathTable");
    ui_altRomPathTable->setColumnWidth(0, 80);
    ui_altRomPathTable->setColumnWidth(1, 218);

    ui_artPath1Edit   = findChild<QLineEdit*>("artPath1Edit");
    ui_artPath1Button = findChild<QToolButton*>("artPath1Button");
    ui_artPath2Edit   = findChild<QLineEdit*>("artPath2Edit");
    ui_artPath2Button = findChild<QToolButton*>("artPath2Button");

    ui_nameEdit->setText(systemData->identity);
    ui_descEdit->setText(systemData->description);
    ui_artPath1Edit->setText(systemData->artPath1);
    ui_artPath2Edit->setText(systemData->artPath2);

    iconPath = sysData->iconpath;

    QIcon icon(iconPath);
    QSize desiredSize(48, 48);
    ui_iconButton->setIcon(icon);
    ui_iconButton->setIconSize(desiredSize);

    for (int i = 0; i < systemData->filepaths.size(); i++)
    {
        ui_altRomPathTable->insertRow(i);
        QTableWidgetItem *altName = new QTableWidgetItem(systemData->filepathIds.at(i));
        ui_altRomPathTable->setItem(i, 0, altName);
        QTableWidgetItem *altPath = new QTableWidgetItem(systemData->filepaths.at(i));
        ui_altRomPathTable->setItem(i, 1, altPath);
    }

    connect(ui_iconButton, SIGNAL(clicked()), this, SLOT(openIconSelect()));
    connect(ui_addRomPathButton, SIGNAL(clicked()), this, SLOT(openRomPathSelect()));
    connect(ui_delRomPathButton, SIGNAL(clicked()), this, SLOT(removeRomPath()));
    connect(ui_artPath1Button, SIGNAL(clicked()), this, SLOT(openArtPath1Select()));
    connect(ui_artPath2Button, SIGNAL(clicked()), this, SLOT(openArtPath2Select()));
}

SystemConfig::~SystemConfig()
{
    delete ui;
}

void SystemConfig::openIconSelect()
{
    QString newIconPath = QFileDialog::getOpenFileName(this, "Select Icon", systemData->iconpath, "Icons (*.png *.ico *.bmp)");
    if (!newIconPath.isEmpty())
    {
        iconPath = newIconPath;
        QIcon icon(iconPath);
        QSize desiredSize(48, 48);
        ui_iconButton->setIcon(icon);
        ui_iconButton->setIconSize(desiredSize);
    }
}

void SystemConfig::openRomPathSelect()
{
    QString romPath = QFileDialog::getExistingDirectory(this, "Select ROM Path", QDir::homePath());
    if (!romPath.isEmpty())
    {
        bool ok;
        QString pathName = QInputDialog::getText(this, "Rom Path Name", "Name for this new rom path:", QLineEdit::Normal, "", &ok);

        if (ok && !pathName.isEmpty())
        {
            int rowToUse = ui_altRomPathTable->rowCount();
            ui_altRomPathTable->insertRow(rowToUse);
            QTableWidgetItem *newName = new QTableWidgetItem(pathName);
            ui_altRomPathTable->setItem(rowToUse, 0, newName);
            QTableWidgetItem *newPath = new QTableWidgetItem(romPath);
            ui_altRomPathTable->setItem(rowToUse, 1, newPath);
        }
    }
}

void SystemConfig::removeRomPath()
{
    int row = ui_altRomPathTable->currentRow();
    if (row >= 0 && row < ui_altRomPathTable->rowCount())
    {
        QMessageBox msgBox;
        msgBox.setText("Are you sure you want to remove this path?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Ok)
        {
            ui_altRomPathTable->removeRow(row);
        }
    }
}

void SystemConfig::openArtPath1Select()
{
    QString artPath1 = QFileDialog::getExistingDirectory(this, "Select Art Path", ui_artPath1Edit->text());
    if (!artPath1.isEmpty())
    {
        ui_artPath1Edit->setText(artPath1);
    }
}

void SystemConfig::openArtPath2Select()
{
    QString artPath2 = QFileDialog::getExistingDirectory(this, "Select Art Path", ui_artPath2Edit->text());
    if (!artPath2.isEmpty())
    {
        ui_artPath2Edit->setText(artPath2);
    }
}

void SystemConfig::accept()
{
    systemData->identity = ui_nameEdit->text();
    systemData->description = ui_descEdit->text();
    systemData->iconpath = iconPath;
    systemData->artPath1 = ui_artPath1Edit->text();
    systemData->artPath2 = ui_artPath2Edit->text();

    systemData->filepaths.clear();
    systemData->filepathIds.clear();
    for (int i = 0; i < ui_altRomPathTable->rowCount(); i++)
    {
        systemData->filepathIds.append(ui_altRomPathTable->item(i, 0)->text());
        systemData->filepaths.append(ui_altRomPathTable->item(i, 1)->text());
    }

    QDialog::accept();
}
