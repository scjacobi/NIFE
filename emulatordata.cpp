#include "emulatordata.h"
#include <QDir>

QTextStream &operator<<( QTextStream &stream, const EmulatorData &data )
{
    QString filepath;
    QString filepathId;
    QStringList emulatorInfo;

    emulatorInfo << data.name
               << data.description
               << data.emupath
               << data.iconpath;

    stream << emulatorInfo.join("|") << "\n";
    return stream;
}

QTextStream &operator>>( QTextStream &stream, EmulatorData &data )
{
    QString filepath;
    QString filepathId;

    QString emulatorLine = stream.readLine();

    if (emulatorLine.isNull() || emulatorLine.isEmpty())
    {
        return stream;
    }

    QStringList emulatorInfo = emulatorLine.split("|");

    int index = 0;
    data.name = emulatorInfo[index++];
    data.description = emulatorInfo[index++];
    data.emupath = emulatorInfo[index++];
    data.iconpath = emulatorInfo[index++];

    return stream;
}

void EmuDataMgr::saveEmuData()
{
    QString pathToFile = QDir::homePath() + "/.nife/emulators.cfg";
    QString pathToBackup = QDir::homePath() + "/.nife/emulators.bak";

    QFile file(pathToFile);
    file.copy(pathToBackup);

    if( !file.open( QIODevice::WriteOnly ) )
      return;

    QTextStream stream( &file );
    foreach (EmulatorData emulator, emuData)
    {
        stream << emulator;
    }
    file.close();
}

void EmuDataMgr::loadEmuData()
{
    emuData.clear();

    QString pathToFile = QDir::homePath() + "/.nife/emulators.cfg";
    QFile file(pathToFile);

    if( !file.open( QIODevice::ReadOnly ) )
      return;

    QTextStream stream( &file );
    while (!stream.atEnd())
    {
        EmulatorData emu;
        stream >> emu;
        emuData.insert(emu.name, emu);
    }
    file.close();
}
