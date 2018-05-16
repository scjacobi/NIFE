#include "mainwindow.h"
#include "systemdata.h"
#include <QApplication>
#include <QFile>
#include <QDir>

bool EnsureNifeDir()
{
    QString pathToDir = QDir::homePath() + "/.nife";
    QDir nifeDir(pathToDir);

    if (!nifeDir.exists())
    {
        return nifeDir.mkpath(".");
    }

    return true;
}

int InitializeSystemData()
{
    int startingSystem = 0;
    SysDataMgr::getInstance().getSysData().empty();

    SystemData system;
    QString pathToFile = QDir::homePath() + "/.nife/systems.cfg";
    QFile file(pathToFile);

    if (file.exists())
    {
        if( !file.open( QIODevice::ReadOnly ) )
          return 0;

        QTextStream stream( &file );
        while (!stream.atEnd())
        {
            stream >> system;
            SysDataMgr::getInstance().getSysData().append(system);
        }
        file.close();
    }

    return startingSystem;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!EnsureNifeDir())
    {
        return 0;
    }

    InitializeSystemData();

    EmuDataMgr::getInstance().loadEmuData();
    MainWindow w;
    w.show();

    return a.exec();
}
