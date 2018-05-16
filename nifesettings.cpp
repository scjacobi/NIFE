#include "nifesettings.h"
#include <QString>
#include <QDir>
#include <QTextStream>

void writeNifeSettings(const NifeSettings &settings)
{
    QString pathToFile = QDir::homePath() + "/.nife/nife.cfg";
    QFile file(pathToFile);

    if( !file.open( QIODevice::WriteOnly ) )
      return;

    QStringList nifeInfo;
    nifeInfo << QString::number(settings.selectedSystem)
               << QString::number(settings.mainX)
               << QString::number(settings.mainY)
               << QString::number(settings.mainWidth)
               << QString::number(settings.mainHeight)
               << QString::number(settings.showArtwork);

    QTextStream stream( &file );
    stream << nifeInfo.join("|") << "\n";

    file.close();
}

void readNifeSettings(NifeSettings &settings)
{
    QString pathToFile = QDir::homePath() + "/.nife/nife.cfg";
    QFile file(pathToFile);

    if( !file.open( QIODevice::ReadOnly ) )
      return;

    QTextStream stream( &file );
    QString nifeLine = stream.readLine();
    QStringList nifeInfo = nifeLine.split("|");

    int index = 0;
    settings.selectedSystem = nifeInfo[index++].toInt();
    settings.mainX = nifeInfo[index++].toInt();
    settings.mainY = nifeInfo[index++].toInt();
    settings.mainWidth = nifeInfo[index++].toInt();
    settings.mainHeight = nifeInfo[index++].toInt();
    settings.showArtwork = nifeInfo[index++].toInt();

    file.close();
}
