/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 21:17:27
 * @Descripttion: 
 */
#include "SunnyTcsApiOrderController.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qstringlist.h>
#include "SunnyTcsApiMessageCenter.h"

#define MESSAGE_ID_ADD_ORDER 11001
#define MESSAGE_ID_CANCEL_ORDER 11004

SunnyTcsApiOrderController::SunnyTcsApiOrderController(SunnyTcsOrderModel* model):_model(model)
{
}

void SunnyTcsApiOrderController::service(HttpRequest& request, HttpResponse& respond)
{
    QString path = request.getPath();
    QString prefix = "/sunnyapi/V1/order";
    if(!path.startsWith(prefix)) return;
    path.remove(prefix);

    QString method = request.getMethod();
    respond.setHeader("Content-Type", "text/json;charset=utf-8");
    QByteArray body;
    if(method == "GET") //查询订单信息
    {
        if(path.startsWith("/all"))//查询所有
        {
            QJsonObject jo;
            QJsonArray jarr;
            auto ords = _model->orders();
            for (auto ord : ords) {
                QJsonObject temp;
                temp["ID"] = ord.id;
                temp["AGV"] = ord.vehicle;
                temp["Statu"] = ord.statu;
                temp["StartTime"] = ord.startTime;
                temp["Cyclic"] = ord.isCyclic ? "YES" : "NO";
                jarr.append(temp);
            }
            jo["Orders"] = jarr;
            QJsonDocument jd(jo);
            body.append(jd.toJson());
        }
        else if (path.startsWith("/updated")){
            auto orders = _model->orders();
            auto updateds = _model->updatedOrders();
            QJsonArray jarr;
            for (QString id : updateds) {
                for (Order ord : orders) {
                    if (ord.id == id) {
                        QJsonObject jo;
                        jo["ID"] = id;
                        jo["State"] = ord.statu;
                        jo["Reason"] = ord.failedReason;
                        jarr.append(jo);
                    }
                }
            }
            QJsonObject temp;
            temp["UpdatedOrders"] = jarr;
            QJsonDocument jd(temp);
            body.append(jd.toJson());
        }
    }
    else if(method == "POST") //新建订单
    {
		QJsonObject jo = QJsonDocument::fromJson(request.getBody()).object();
		QString ve = jo["vehicle"].toString();
		QJsonArray jarr = jo["Points"].toArray();
		QStringList args;
		args.append("0");
		args.append(ve);
		args.append("16");
		args.append("Y");
		args.append("0");
		for (auto ja : jarr)//目前只添加点，还没有工位
		{
			QJsonObject jo = ja.toObject();
			QString pt = jo["Name"].toString();
			QString loc = jo["Location"].toString("");
			QString str = pt;
			if (!loc.isEmpty()) {
				str += ":" + loc;
			}
			args.append(str);
		}
		SunnyTcsMessage<QString> msg(MESSAGE_UNICAST, MESSAGE_ID_ADD_ORDER, MESSAGE_NODE_KERNEL);
        for (QString arg : args) {
            msg.addArg(arg);
        }
		SunnyTcsApiMessageCenter::instance()->sendStrMessage(msg);
		qInfo() << "order send........";
    }
    else if(method == "DELETE") //删除订单
    {
        QJsonObject jo = QJsonDocument::fromJson(request.getBody()).object();
        QString ord = jo["ID"].toString();
        SunnyTcsMessage<QString> msg(MESSAGE_UNICAST, MESSAGE_ID_CANCEL_ORDER, MESSAGE_NODE_KERNEL);
        msg.addArg(ord);
        SunnyTcsApiMessageCenter::instance()->sendStrMessage(msg);
    }
}