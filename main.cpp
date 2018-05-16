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

    if (!file.exists())
    {
        system.identity = "Atari";
        system.description = "Atari 2600";
        system.filepaths.append("/home/procyon/roms/Atari - 2600/");
        system.filepathIds.append("No-Intro");
        system.iconpath = "/home/procyon/Pac-Man_Avatar.png";

        SysDataMgr::getInstance().getSysData().append(system);

        system.identity = "Nintendo";
        system.description = "Nintendo Entertainment System";
        system.filepaths[0] = "/home/procyon/roms/Nintendo - Nintendo Entertainment System/";
//        system.filepathIds.append("No-Intro");
        system.iconpath = "/home/procyon/Pac-Man_Avatar.png";

        SysDataMgr::getInstance().getSysData().append(system);

        system.identity = "Sega";
        system.description = "Sega Master System";
        system.filepaths[0] = "/home/procyon/roms/Sega - Master System - Mark III/";
//        system.filepathIds.append("No-Intro");
        system.iconpath = "/home/procyon/Pac-Man_Avatar.png";

        SysDataMgr::getInstance().getSysData().append(system);

        system.identity = "NEC";
        system.description = "TurboGrafx 16";
        system.filepaths[0] = "/home/procyon/roms/NEC - PC Engine - TurboGrafx 16/";
//        system.filepathIds.append("No-Intro");
        system.iconpath = "/home/procyon/Pac-Man_Avatar.png";

        SysDataMgr::getInstance().getSysData().append(system);

        if( !file.open( QIODevice::WriteOnly ) )
          return 0;

        QTextStream stream( &file );
        stream << "0";
        foreach (SystemData system, SysDataMgr::getInstance().getSysData())
        {
            stream << system;
        }
        file.close();
    }
    else
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
