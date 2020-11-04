/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 21:19:34
 * @Descripttion: 
 */
#include "SunnyTcsApiAgvController.h"
#include <QVariant>
#include <QDateTime>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SunnyTcsApiAgvController::SunnyTcsApiAgvController(SunnyTcsAgvModel* model):_model(model){}

void SunnyTcsApiAgvController::service(HttpRequest& request, HttpResponse& reponse)
{
    QString path = request.getPath();
    QString prefix = "/sunnyapi/V1/agv";
    if(!path.startsWith(prefix)) return;
    path.remove(prefix);

	qInfo() << "get agv request....";

     reponse.setHeader("Content-Type", "text/json;charset=utf-8");
    QByteArray body;
    QString method = request.getMethod();
    if(method == "GET")
    {
        if(path.startsWith("/all"))
        {
            QJsonObject jo;
            QJsonArray jarr;
            auto ves = _model->allVehicles();
            for(auto ve :ves)
            {
                QJsonObject temp;
                temp["Name"] = ve.name;
                temp["IP"] = ve.ip;
                temp["Port"] = ve.port;
                jarr.append(temp);
            }
            jo["vehicles"] = jarr;
            QJsonDocument jd(jo);
            body.append(jd.toJson());
        }
        else 
        {
            /* code */
        }
    }

    reponse.write(body, true);    
}