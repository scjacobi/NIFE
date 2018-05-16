#include "mainwindow.h"
#include "systemdata.h"
#include <QApplication>
#include <QFile>
#include <QDir>

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

    InitializeSystemData();

    EmuDataMgr::getInstance().loadEmuData();
    MainWindow w;
    w.show();

    return a.exec();
}
