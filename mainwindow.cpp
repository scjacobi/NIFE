#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "listviewdelegate.h"
#include "emumanager.h"
#include <QStringListModel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Start the window and load in previous geometry settings
    ui->setupUi(this);
    settings.selectedSystem = 0;
    settings.showArtwork = 0;
    settings.mainWidth = -1;
    readNifeSettings(settings);
    if (settings.mainWidth > 0)
    {
        setGeometry(settings.mainX, settings.mainY, settings.mainWidth, settings.mainHeight);
    }
    QSize winSize = this->size();

    // Set up the tool bar
    QToolBar *fileToolBar = findChild<QToolBar*>("mainToolBar");
    const QIcon launchIcon = QIcon::fromTheme("media-playback-start");
    QAction *launchAct = new QAction(launchIcon, "Launch", this);
    launchAct->setStatusTip(tr("Launch selected game with select emulator"));
    connect(launchAct, &QAction::triggered, this, &MainWindow::toolBarLaunchEmulator);
    fileToolBar->addAction(launchAct);

    const QIcon configIcon = QIcon::fromTheme("system-file-manager");
    QAction *configAct = new QAction(configIcon, "Emulator Manager", this);
    configAct->setStatusTip(tr("Open the emulator config manager window"));
    connect(configAct, &QAction::triggered, this, &MainWindow::openEmuMgrConfig);
    fileToolBar->addAction(configAct);

    const QIcon artIcon = QIcon::fromTheme("image-x-generic");
    QAction *artAct = new QAction(artIcon, "Toggle Art", this);
    artAct->setStatusTip(tr("Toggle between showing and hiding artwork"));
    connect(artAct, &QAction::triggered, this, &MainWindow::toggleArtSetting);
    fileToolBar->addAction(artAct);

    sysView = new ManipulateListView(this);
    emuView = new ManipulateListView(this);

    sysLayout = new QVBoxLayout();
    emuLayout = new QVBoxLayout();
    romLayout = new QVBoxLayout();

    filterLayout = new QHBoxLayout();
    filterText = new QLineEdit(this);
    doFilter = new QPushButton(this);

    // Attach the filter edit text and the filter button to a horizontal layout.
    doFilter->setText("Filter ROMs");
    connect(doFilter, SIGNAL(clicked(bool)), this, SLOT(filterRomView()));
    filterLayout->addWidget(filterText);
    filterLayout->addWidget(doFilter);

    // Attach rom views to a set of tabs
    romTabs = new QTabWidget(this);

    // Add the rom views to the filter layout, as a widget
    romLayout->addWidget(romTabs);
    romLayout->addLayout(filterLayout);
    romWidget = new QWidget(this);
    romWidget->setLayout(romLayout);

    QLabel* systemLabel = new QLabel("<b>Systems:</b>", this);
    sysLayout->addWidget(systemLabel);
    sysLayout->addWidget(sysView);
    sysWidget = new QWidget(this);
    sysWidget->setLayout(sysLayout);

    QLabel* emuLabel = new QLabel("<b>Emulators:</b>", this);
    emuLayout->addWidget(emuLabel);
    emuLayout->addWidget(emuView);
    emuWidget = new QWidget(this);
    emuWidget->setLayout(emuLayout);

    // Use a vertical splitter to combine the system view on top with with the emu view on the bottom.
    vertSplitter = new QSplitter(Qt::Vertical, this);
    vertSplitter->addWidget(sysWidget);
    vertSplitter->addWidget(emuWidget);
    vertSplitter->setSizes(QList<int>() << winSize.height()*2/3 << winSize.height()/3);

    // User a horizontal splitter to combine the two sys/emu views with the rom/filter widget.
    horizSplitter = new QSplitter(Qt::Horizontal, this);
    horizSplitter->addWidget(vertSplitter);
    horizSplitter->addWidget(romWidget);
    horizSplitter->setSizes(QList<int>() << winSize.width()/4 << winSize.width()*2/4);

    // Put two labels (for pixmaps) in a vertical layout, and put the layout in a widget for a splitter.
    artLayout = new QVBoxLayout();
    art1Image = new QLabel();
    art2Image = new QLabel();
    art1Image->setAlignment(Qt::AlignHCenter);
    art2Image->setAlignment(Qt::AlignHCenter);
    art1Image->setAlignment(Qt::AlignCenter);
    art2Image->setAlignment(Qt::AlignCenter);
    artLayout->addWidget(art1Image);
    artLayout->addWidget(art2Image);
    artWidget = new QWidget(this);
    artWidget->setLayout(artLayout);
    artWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    artHorizSplitter = new QSplitter(Qt::Horizontal, this);
    artHorizSplitter->addWidget(horizSplitter);
    artHorizSplitter->addWidget(artWidget);
    artHorizSplitter->resize(winSize);
    artHorizSplitter->setSizes(QList<int>() << winSize.width()*3/4 << winSize.width()/4);
    connect(artHorizSplitter, SIGNAL(splitterMoved(int,int)),this, SLOT(resizeArt()));

    // Apply the horizantal splitter as the central widget.
    setCentralWidget(artHorizSplitter);

    // Wire up the system view to the system model.
    QStandardItemModel* sysModel = new QStandardItemModel();
    ListviewDelegate* listdelegate = new ListviewDelegate();
    sysView->setItemDelegate(listdelegate);
    sysView->setModel(sysModel);
    sysView->setAlternatingRowColors(true);
    sysView->setStyleSheet("alternate-background-color: gainsboro;background-color: white;");
    sysView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect signals to respond to.  No need to connect "clicked", it's handled by selection changed.
    connect(sysView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSystemContextMenu(QPoint)));
    connect(sysView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this,  SLOT(switchSystemSelected()));
    connect(sysView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editSystem()));
    connect(sysView, SIGNAL(deletePressed(QModelIndex)), this, SLOT(removeSystem()));
    connect(sysView, SIGNAL(pageUpPressed(QModelIndex)), this, SLOT(upSystem()));
    connect(sysView, SIGNAL(pageDownPressed(QModelIndex)), this, SLOT(downSystem()));

    // Fill in the system model with all of the systems passed in from main.
    foreach (SystemData system, SysDataMgr::getInstance().getSysData())
    {
        QIcon icon(system.iconpath);
        QStandardItem* item = new QStandardItem();
        item->setData(system.identity,ListviewDelegate::headerTextRole);
        item->setData(system.description,ListviewDelegate::subHeaderTextrole);
        item->setData(icon,ListviewDelegate::IconRole);
        item->setEditable(false);
        sysModel->appendRow(item);
    }

    // Wire up the emu view to the emu model.  We'll fill in the emu model when we switch systems.
    QStandardItemModel* emuModel = new QStandardItemModel();
    emuView->setItemDelegate(listdelegate);
    emuView->setModel(emuModel);
    emuView->setAlternatingRowColors(true);
    emuView->setStyleSheet("alternate-background-color: gainsboro;background-color: white;");
    emuView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect signals to respond to.  No need to connect "clicked", it's handled by selection changed.
    connect(emuView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showEmulatorContextMenu(QPoint)));
    connect(emuView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editEmu()));
    connect(emuView, SIGNAL(deletePressed(QModelIndex)), this, SLOT(removeEmu()));
    connect(emuView, SIGNAL(pageUpPressed(QModelIndex)), this, SLOT(upEmu()));
    connect(emuView, SIGNAL(pageDownPressed(QModelIndex)), this, SLOT(downEmu()));

    // Designate a file model.
    filemodel = new QFileSystemModel;
    filemodel->setFilter(QDir::Files);
    filemodel->setRootPath("/");

    sysView->setCurrentIndex(sysView->model()->index(settings.selectedSystem, 0));
    showArt(settings.showArtwork);

    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    ListviewDelegate* listdelegate = (ListviewDelegate*)sysView->itemDelegate();
    QStandardItemModel* sysModel = (QStandardItemModel*)sysView->model();
    QStandardItemModel* emuModel = (QStandardItemModel*)emuView->model();

    delete ui;

    delete horizSplitter;
//    delete vertSplitter;
//    delete romWidget;
//    delete romTabs;
//    delete romLayout;
//    delete doFilter;
//    delete filterText;
//    delete filterLayout;
//    delete emuView;
//    delete sysView;
    delete sysModel;
    delete emuModel;
    delete listdelegate;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QRect geom = geometry();
    settings.mainX = geom.x();
    settings.mainY = geom.y();
    settings.mainWidth = geom.width();
    settings.mainHeight = geom.height();
    writeNifeSettings(settings);

    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    resizeArt();
    QMainWindow::resizeEvent(event);
}

void MainWindow::switchSystemSelected()
{
    int i;

    // Update the record of the current row.
    currentSystemRow = sysView->currentIndex().row();
    settings.selectedSystem = currentSystemRow;

    // Get the current row's system data
    SystemData system = getCurrentSystem();

    bool emulatorsRemoved = false;
    for (int x = 0; x < system.emuNames.size(); x++)
    {
        QString emuName = system.emuNames[x];
        if (!EmuDataMgr::getInstance().getEmuData().contains(emuName))
        {
            system.emuNames.remove(x);
            system.emuParams.remove(x);
            x--;
            emulatorsRemoved = true;
        }
    }
    if (emulatorsRemoved)
    {
        updateAndSaveCurrentSystem(system);
    }


    // Populate the emuView with the loaded emulators.
    QStandardItemModel* model = (QStandardItemModel*)emuView->model();
    model->clear();
    foreach (QString emuName, system.emuNames)
    {
        EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(emuName);
        QIcon icon(emu.iconpath);
        QStandardItem* item = new QStandardItem();
        item->setData(emu.name,ListviewDelegate::headerTextRole);
        item->setData(emu.description,ListviewDelegate::subHeaderTextrole);
        item->setData(icon,ListviewDelegate::IconRole);
        item->setEditable(false);
        model->appendRow(item);
    }

    // Set the current emulator selection to the first row.
    emuView->setCurrentIndex(model->index(0, 0));

    // Check if the filter was used, and if so, clear it.
    filterText->setText("");
    if (filemodel->nameFilters().size() > 0)
    {
        delete filemodel;
        filemodel = new QFileSystemModel;
        filemodel->setFilter(QDir::Files);
        filemodel->setRootPath("/");
    }

    // Grab the old tabs and flush them out.
    int tabCount = romTabs->count();
    for (i = tabCount - 1; i >= 0; i--)
    {
        LaunchListView* romView = (LaunchListView*)romTabs->widget(i);
        romTabs->removeTab(i);
        delete romView;
    }

    // Add a tab for every filepath associated with this system.
    for (i = 0; i < system.filepaths.size(); i++)
    {
        LaunchListView* romView = new LaunchListView;
        romView->setModel(filemodel);
        romView->setAlternatingRowColors(true);
        romView->setStyleSheet("alternate-background-color: gainsboro;background-color: white;");
        romView->setRootIndex(filemodel->index(system.filepaths.at(i)));
        connect(romView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this,  SLOT(updateArt()));
        connect(romView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(launchEmulator(QModelIndex)));
        connect(romView, SIGNAL(enterPressed(QModelIndex)), this, SLOT(launchEmulator(QModelIndex)));
        romTabs->addTab(romView, system.filepathIds.at(i));
    }

    QString noImagePath = QDir::homePath() + "/.nife/noimage.png";
    art1Pixmap.load(noImagePath);
    art2Pixmap.load(noImagePath);
    resizeArt();
}

void MainWindow::resizeArt()
{
    QSize winSize = this->size();
\
    QList<int> sizes = artHorizSplitter->sizes();
    int width = sizes[1] * 9 / 10;
    int height = width * 3 / 4;
    if (height > winSize.height() * 8 / 20)
    {
        height = winSize.height() * 8 / 20;
        width = height * 4 / 3;
        int safesize = width * 10 / 9;
        artHorizSplitter->setSizes(QList<int>() << winSize.width() - safesize << safesize);
    }

    if (!art1Pixmap.isNull())
    {
        art1Image->setPixmap(art1Pixmap.scaled(width, height, Qt::KeepAspectRatio));
    }
    if (!art2Pixmap.isNull())
    {
        art2Image->setPixmap(art2Pixmap.scaled(width, height, Qt::KeepAspectRatio));
    }
}

void MainWindow::updateArt()
{
    QString artPath;
    QFileInfo romInfo;
    QString romName;
    QString artName = QDir::homePath() + "/.nife/noimage.png";
    QPixmap* pixmap;
    bool worthResizing = false;

    LaunchListView* romView = (LaunchListView*)romTabs->widget(romTabs->currentIndex());
    QFileSystemModel* model = (QFileSystemModel*)romView->model();

    SystemData system = getCurrentSystem();
    for (int x = 0; x < 2; x++)
    {
        artPath = (x == 0) ? system.artPath1 : system.artPath2;
        pixmap = (x == 0) ? &art1Pixmap : &art2Pixmap;
        if (!artPath.isEmpty())
        {
            romInfo = model->fileInfo(romView->currentIndex());
            romName = romInfo.completeBaseName() + ".*";
            QStringList romNameFilter;
            romNameFilter << romName;
            QDirIterator dirIter(artPath, romNameFilter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            if (dirIter.hasNext())
            {
                artName = dirIter.next();
            }
            pixmap->load(artName);
            worthResizing = true;
         }
    }
    if (worthResizing)
    {
        resizeArt();
    }
}

void MainWindow::toggleArtSetting()
{
    settings.showArtwork = !settings.showArtwork;
    showArt(settings.showArtwork);
}

void MainWindow::showArt(bool show)
{
    if (show)
    {
        artworkSizes = artHorizSplitter->sizes();
        artHorizSplitter->setSizes(QList<int>() << 1 << 0);
    }
    else
    {
        if (artworkSizes.isEmpty())
        {
            QSize winSize = this->size();
            int fourthWidth = winSize.width() / 4;
            artworkSizes << fourthWidth * 3 << fourthWidth;
        }
        artHorizSplitter->setSizes(artworkSizes);
        resizeArt();
    }
}

void MainWindow::showSystemContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = sysView->mapToGlobal(pos);
    QStyle* style = sysView->style();

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction(style->standardIcon(QStyle::SP_FileDialogNewFolder), "Add", this, SLOT(addSystem()));
    myMenu.addAction(style->standardIcon(QStyle::SP_TrashIcon), "Remove",  this, SLOT(removeSystem()));
    myMenu.addAction(style->standardIcon(QStyle::SP_FileDialogContentsView), "Edit",  this, SLOT(editSystem()));
    myMenu.addSeparator();
    myMenu.addAction(style->standardIcon(QStyle::SP_ArrowUp), "Move up", this, SLOT(upSystem()));
    myMenu.addAction(style->standardIcon(QStyle::SP_ArrowDown), "Move down", this, SLOT(downSystem()));

    if (currentSystemRow == 0)
    {
        myMenu.actions().at(4)->setEnabled(false);
    }
    else if (currentSystemRow == sysView->model()->rowCount()-1)
    {
        myMenu.actions().at(5)->setEnabled(false);
    }

    // Show context menu at handling positionLaunchListView
    myMenu.exec(globalPos);
}

void MainWindow::addSystem()
{
    SystemData system;
    sysConfig = new SystemConfig(&system, this);
    if (sysConfig->exec() == QDialog::Accepted)
    {
        QIcon icon(system.iconpath);
        QStandardItem* item = new QStandardItem();
        item->setData(system.identity,ListviewDelegate::headerTextRole);
        item->setData(system.description,ListviewDelegate::subHeaderTextrole);
        item->setData(icon,ListviewDelegate::IconRole);
        item->setEditable(false);

        QStandardItemModel* model = (QStandardItemModel*)sysView->model();
        model->appendRow(item);

        SysDataMgr::getInstance().getSysData().append(system);
        SysDataMgr::getInstance().saveSysData();
    }
    delete sysConfig;
}

void MainWindow::removeSystem()
{
    if (currentSystemRow >= 0 && currentSystemRow < sysView->model()->rowCount())
    {
        SystemData system = SysDataMgr::getInstance().getSysData()[currentSystemRow];

        QMessageBox msgBox;
        msgBox.setText("Are you sure you want to remove system: " + system.identity + "?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Ok)
        {
            QStandardItemModel* model = (QStandardItemModel*)sysView->model();
            model->removeRow(currentSystemRow);

            SysDataMgr::getInstance().getSysData().remove(currentSystemRow);
            if (currentSystemRow == sysView->model()->rowCount() - 1)
            {
                currentSystemRow--;
            }
            SysDataMgr::getInstance().saveSysData();
        }
    }
}

void MainWindow::editSystem()
{
    if (currentSystemRow >= 0 && currentSystemRow < sysView->model()->rowCount())
    {
        SystemData system = SysDataMgr::getInstance().getSysData()[currentSystemRow];
        sysConfig = new SystemConfig(&system, this);
        if (sysConfig->exec() == QDialog::Accepted)
        {
            QIcon icon(system.iconpath);
            QStandardItem* item = new QStandardItem();
            item->setData(system.identity,ListviewDelegate::headerTextRole);
            item->setData(system.description,ListviewDelegate::subHeaderTextrole);
            item->setData(icon,ListviewDelegate::IconRole);
            item->setEditable(false);

            QStandardItemModel* model = (QStandardItemModel*)sysView->model();
            model->setItem(currentSystemRow, item);
            sysView->setCurrentIndex(model->index(currentSystemRow, 0));

            updateAndSaveCurrentSystem(system);
        }
        delete sysConfig;
    }
}

void MainWindow::upSystem()
{
    if (currentSystemRow >= 1 && currentSystemRow < sysView->model()->rowCount())
    {
        int row = currentSystemRow;
        QStandardItemModel* model = (QStandardItemModel*)sysView->model();
        QList<QStandardItem*> items = model->takeRow(row);
        model->insertRow(row-1, items[0]);

        std::swap(SysDataMgr::getInstance().getSysData()[row], SysDataMgr::getInstance().getSysData()[row-1]);

        sysView->setCurrentIndex(model->index(row-1, 0));
        SysDataMgr::getInstance().saveSysData();
    }
}

void MainWindow::downSystem()
{
    if (currentSystemRow >= 0 && currentSystemRow < sysView->model()->rowCount() - 1)
    {
        int row = currentSystemRow;
        QStandardItemModel* model = (QStandardItemModel*)sysView->model();
        QList<QStandardItem*> items = model->takeRow(row);
        model->insertRow(row+1, items[0]);

        std::swap(SysDataMgr::getInstance().getSysData()[row], SysDataMgr::getInstance().getSysData()[row+1]);

        sysView->setCurrentIndex(model->index(row+1, 0));
        SysDataMgr::getInstance().saveSysData();
    }
}

void MainWindow::showEmulatorContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = emuView->mapToGlobal(pos);
    QStyle* style = emuView->style();

    int row = emuView->currentIndex().row();

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction(style->standardIcon(QStyle::SP_FileDialogNewFolder), "Add", this, SLOT(addEmu()));
    myMenu.addAction(style->standardIcon(QStyle::SP_TrashIcon), "Remove",  this, SLOT(removeEmu()));
    myMenu.addAction(style->standardIcon(QStyle::SP_FileDialogContentsView), "Edit",  this, SLOT(editEmu()));
    myMenu.addSeparator();
    myMenu.addAction(style->standardIcon(QStyle::SP_ArrowUp), "Move up", this, SLOT(upEmu()));
    myMenu.addAction(style->standardIcon(QStyle::SP_ArrowDown), "Move down", this, SLOT(downEmu()));

    if (row == 0)
    {
        myMenu.actions().at(4)->setEnabled(false);
    }
    else if (row == emuView->model()->rowCount()-1)
    {
        myMenu.actions().at(5)->setEnabled(false);
    }


    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void MainWindow::addEmu()
{
    SystemData system = getCurrentSystem();
    emuConfig = new EmulatorConfig(&system, -1, this);
    if (emuConfig->exec() == QDialog::Accepted)
    {
        EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(system.emuNames[system.emuNames.size() - 1]);

        QIcon icon(emu.iconpath);
        QStandardItem* item = new QStandardItem();
        item->setData(emu.name,ListviewDelegate::headerTextRole);
        item->setData(emu.description,ListviewDelegate::subHeaderTextrole);
        item->setData(icon,ListviewDelegate::IconRole);
        item->setEditable(false);

        QStandardItemModel* model = (QStandardItemModel*)emuView->model();
        model->appendRow(item);

        updateAndSaveCurrentSystem(system);
    }
    delete emuConfig;
}

void MainWindow::removeEmu()
{
    int row = emuView->currentIndex().row();
    if (row >= 0 && row < emuView->model()->rowCount())
    {
        SystemData system = getCurrentSystem();
        QString emuName = system.emuNames[row];
        EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(emuName);

        QMessageBox msgBox;
        msgBox.setText("Are you sure you want to remove emulator: " + emu.name + "?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Ok)
        {
            QStandardItemModel* model = (QStandardItemModel*)emuView->model();
            model->removeRow(row);

            system.emuNames.remove(row);
            system.emuParams.remove(row);
            updateAndSaveCurrentSystem(system);
        }
    }
}

void MainWindow::editEmu()
{
    int row = emuView->currentIndex().row();
    if (row >= 0 && row < emuView->model()->rowCount())
    {
        SystemData system = getCurrentSystem();
        QString emuName = system.emuNames[row];
        EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(emuName);
        emuConfig = new EmulatorConfig(&system, row, this);
        if (emuConfig->exec() == QDialog::Accepted)
        {
            EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(emuName);

            QIcon icon(emu.iconpath);
            QStandardItem* item = new QStandardItem();
            item->setData(emu.name,ListviewDelegate::headerTextRole);
            item->setData(emu.description,ListviewDelegate::subHeaderTextrole);
            item->setData(icon,ListviewDelegate::IconRole);
            item->setEditable(false);

            QStandardItemModel* model = (QStandardItemModel*)emuView->model();
            model->setItem(row, item);

            updateAndSaveCurrentSystem(system);
        }
        delete emuConfig;
    }
}

void MainWindow::upEmu()
{
    int row = emuView->currentIndex().row();
    if (row >= 1 && row < emuView->model()->rowCount())
    {
        QStandardItemModel* model = (QStandardItemModel*)emuView->model();
        QList<QStandardItem*> items = model->takeRow(row);
        model->insertRow(row-1, items[0]);

        SystemData system = getCurrentSystem();
        std::swap(system.emuNames[row], system.emuNames[row-1]);
        std::swap(system.emuParams[row], system.emuParams[row-1]);
        SysDataMgr::getInstance().saveSysData();

        emuView->setCurrentIndex(model->index(row-1, 0));
    }
}

void MainWindow::downEmu()
{
    int row = emuView->currentIndex().row();
    if (row >= 0 && row < emuView->model()->rowCount() - 1)
    {
        QStandardItemModel* model = (QStandardItemModel*)emuView->model();
        QList<QStandardItem*> items = model->takeRow(row);
        model->insertRow(row+1, items[0]);

        SystemData system = getCurrentSystem();
        std::swap(system.emuNames[row], system.emuNames[row+1]);
        std::swap(system.emuParams[row], system.emuParams[row+1]);
        SysDataMgr::getInstance().saveSysData();

        emuView->setCurrentIndex(model->index(row+1, 0));
    }
}

void MainWindow::filterRomView()
{
    QStringList filterToApply;
    filterToApply.append("*" + filterText->text() + "*");

    filemodel->setNameFilters(filterToApply);
    filemodel->setNameFilterDisables(false);

    LaunchListView* romView = (LaunchListView*)romTabs->widget(romTabs->currentIndex());
    romView->setModel(filemodel);
}

void MainWindow::openEmuMgrConfig()
{
    EmuManager emuMgr(this);
    emuMgr.exec();
}

int MainWindow::toolBarLaunchEmulator()
{
    LaunchListView* romView = (LaunchListView*)romTabs->widget(romTabs->currentIndex());
    return launchEmulator(romView->currentIndex());
}

int MainWindow::launchEmulator(const QModelIndex& selectedRomIndex)
{
    // Get the currently selected emulator (if any)
    int emuRow = emuView->currentIndex().row();
    // If there's only one emulator, go ahead and use it even if it's not selected.
    if (emuRow < 0 && emuView->model()->rowCount() == 1)
    {
        emuRow = 0;
    }
    // Otherwise, if there's more than one emulator and none are selected, force the user to select one.
    else if (emuRow < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select an emulator before launching a game.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return 0;
    }
    // Get the emulator data
    SystemData system = getCurrentSystem();
    EmulatorData emu = EmuDataMgr::getInstance().getEmuData().value(system.emuNames[emuRow]);

    LaunchListView* romView = (LaunchListView*)romTabs->widget(romTabs->currentIndex());
    QFileSystemModel* model = (QFileSystemModel*)romView->model();
    QString romName = model->filePath(selectedRomIndex);

    // Construct the command to execute as a QString, and convert it to a char* for std::system.
    QString execute = emu.emupath + " " + system.emuParams[emuRow] + " \"" + romName + "\"";
    QByteArray ba = execute.toLatin1();
    const char *c_str2 = ba.data();
    return std::system(c_str2);
}
