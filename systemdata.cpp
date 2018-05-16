#include "systemdata.h"
#include <QDir>

QTextStream &operator<<( QTextStream &stream, const SystemData &data )
{
    int x;

    QString filepath;
    QString filepathId;
    QString emuName;
    QString parameters;
    QStringList systemInfo;

    systemInfo << data.identity
               << data.description
               << data.iconpath
               << QString::number(data.filepaths.size());

    for (x = 0; x < data.filepaths.size(); x++)
    {
        filepath = data.filepaths.value(x);
        filepathId = data.filepathIds.value(x);
        systemInfo << filepath << filepathId;
    }

    systemInfo << QString::number(data.emuNames.size());

    for (x = 0; x < data.emuNames.size(); x++)
    {
        emuName = data.emuNames.value(x);
        parameters = data.emuParams.value(x);
        systemInfo << emuName << parameters;
    }

    systemInfo << data.artPath1 << data.artPath2;

    stream << systemInfo.join("|") << "\n";
    return stream;
}

QTextStream &operator>>( QTextStream &stream, SystemData &data )
{
    int x;

    int numRomPaths;
    int numEmuConfigs;

    QString systemLine = stream.readLine();

    if (systemLine.isNull() || systemLine.isEmpty())
    {
        return stream;
    }

    QStringList systemInfo = systemLine.split("|");

    int index = 0;
    data.identity = systemInfo[index++];
    data.description = systemInfo[index++];
    data.iconpath = systemInfo[index++];

    numRomPaths = systemInfo[index++].toInt();
    data.filepaths.clear();
    data.filepathIds.clear();
    for (x = 0; x < numRomPaths; x++)
    {
        data.filepaths.append(systemInfo[index++]);
        data.filepathIds.append(systemInfo[index++]);
    }

    numEmuConfigs = systemInfo[index++].toInt();
    data.emuNames.clear();
    data.emuParams.clear();
    for (x = 0; x < numEmuConfigs; x++)
    {
        data.emuNames.append(systemInfo[index++]);
        data.emuParams.append(systemInfo[index++]);
    }

    data.artPath1 = systemInfo[index++];
    data.artPath2 = systemInfo[index++];

    return stream;
}

void SysDataMgr::saveSysData()
{
    QString pathToFile = QDir::homePath() + "/.nife/systems.cfg";
    QString pathToBackup = QDir::homePath() + "/.nife/systems.bak";

    QFile file(pathToFile);
    if (QFile::exists(pathToBackup))
    {
        QFile::remove(pathToBackup);
    }
    file.copy(pathToBackup);

    if( !file.open( QIODevice::WriteOnly ) )
      return;

    QTextStream stream( &file );
    foreach (SystemData system, sysData)
    {
        stream << system;
    }
    file.close();
}

void SysDataMgr::loadSysData()
{
    sysData.clear();

    QString pathToFile = QDir::homePath() + "/.nife/systems.cfg";
    QFile file(pathToFile);

    if( !file.open( QIODevice::ReadOnly ) )
      return;

    QTextStream stream( &file );
    while (!stream.atEnd())
    {
        SystemData sys;
        stream >> sys;
        sysData.append(sys);
    }
    file.close();
}
