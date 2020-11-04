/*
 * @Author: your name
 * @Date: 2020-09-01 16:20:47
 * @LastEditTime: 2020-09-02 21:47:33
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \SunnyTcsApi\src\model\SunnyTcsAgvModel.cpp
 */
#include "SunnyTcsAgvModel.h"
#include <QJsonParseError>
#include <qjsondocument.h>
#include <QJsonObject>
#include <qdebug.h>

SunnyTcsAgvModel::SunnyTcsAgvModel(/* args */)
{
}

SunnyTcsAgvModel::~SunnyTcsAgvModel()
{
}

void SunnyTcsAgvModel::syncDataReadySlot(const QByteArray& data)
{
    QJsonParseError err;
    QJsonDocument jd = QJsonDocument::fromJson(data, &err);
    if(!jd.isNull() && err.error == QJsonParseError::NoError)
    {
        QJsonObject jo = jd.object();
        QString ty = jo["elementType"].toString();
        if(ty == "Vehicle")
        {
            QString name = jo["Name"].toString();
            QString ip = jo["IP_Address"].toString();
            qint32 port = jo["Port"].toInt();
            AgvInfo ve(name, ip, port);
            
            if (!_vehicles.contains(ve))
            {
                _vehicles.append(ve);
                qInfo() << "add vehicle:" << ve.name << ";current count" << _vehicles.count();
            }
                
        }
    }
}