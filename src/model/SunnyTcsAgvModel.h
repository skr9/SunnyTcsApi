/*
 * @Author: your name
 * @Date: 2020-09-01 16:20:30
 * @LastEditTime: 2020-09-02 21:46:59
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \SunnyTcsApi\src\model\SunnyTcsAgvModel.h
 */
#ifndef SUNNY_TCS_AGV_MODEL_H
#define SUNNY_TCS_AGV_MODEL_H

#include <QObject>

class AgvInfo{
public:
    QString name;
    QString ip;
    qint32 port;

    AgvInfo(QString agvName, QString address, qint32 listenPort):name(agvName), ip(address), port(listenPort){}

    bool operator == (const AgvInfo& rhs)
    {
        return name == rhs.name;
    }
};

class SunnyTcsAgvModel:public QObject
{
    Q_OBJECT
private:
    QList<AgvInfo> _vehicles;

public slots:
    void syncDataReadySlot(const QByteArray& data);
    
public:
    SunnyTcsAgvModel(/* args */);
    ~SunnyTcsAgvModel();

    QList<AgvInfo> allVehicles()const{return _vehicles;}
};


#endif