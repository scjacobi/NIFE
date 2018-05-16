#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include <QString>
#include <QVector>
#include <QTextStream>

struct SystemData
{
    QString identity;
    QString description;
    QVector<QString> filepaths;
    QVector<QString> filepathIds;
    QString iconpath;
    QVector<QString> emuNames;
    QVector<QString> emuParams;
    QString artPath1;
    QString artPath2;
};

QTextStream &operator<<( QTextStream &stream, const SystemData &data );

QTextStream &operator>>( QTextStream &stream, SystemData &data );

class SysDataMgr
{
    public:
        static SysDataMgr& getInstance()
        {
            static SysDataMgr instance;
            return instance;
        }

        QVector<SystemData>& getSysData() { return sysData; };
        void loadSysData();
        void saveSysData();

    private:
        SysDataMgr() {}

        SysDataMgr(SysDataMgr const&);              // Don't Implement
        void operator=(SysDataMgr const&);          // Don't implement

        QVector<SystemData> sysData;
};

#endif // SYSTEMDATA_H
