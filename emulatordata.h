#ifndef EMULATORDATA_H
#define EMULATORDATA_H

#include <QString>
#include <QVector>
#include <QTextStream>

struct EmulatorData
{
    QString name;
    QString description;
    QString emupath;
    QString iconpath;
};

QTextStream &operator<<( QTextStream &stream, const EmulatorData &data );

QTextStream &operator>>( QTextStream &stream, EmulatorData &data );

class EmuDataMgr
{
    public:
        static EmuDataMgr& getInstance()
        {
            static EmuDataMgr instance;
            return instance;
        }

        QMap<QString, EmulatorData>& getEmuData() { return emuData; };
        void loadEmuData();
        void saveEmuData();

    private:
        EmuDataMgr() {}

        EmuDataMgr(EmuDataMgr const&);              // Don't Implement
        void operator=(EmuDataMgr const&);          // Don't implement

        QMap<QString, EmulatorData> emuData;
};

#endif // EMULATORDATA_H
