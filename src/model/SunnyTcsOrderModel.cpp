#include "SunnyTcsOrderModel.h"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qdatetime.h>
#include <qdebug.h>

QString SunnyTcsOrderModel::stateString(qint32 state)
{
	switch (state)
	{
		case 0: return "CREATED"; break;
		case 1: return "ACTIVED"; break;
		case 2: return "ROUTED"; break;
		case 3: return "UNROUTABLE"; break;
		case 4: return "READY"; break;
		case 5: return "PROCESSING"; break;
		case 6: return "WITHDRAW"; break;
		case 7: return "FAILED"; break;
		case 8: return "FINISHED"; break;
		default:return "UNKOWN"; break;
	}
}

void SunnyTcsOrderModel::removeOverTimeUpdatedOrders()
{
	auto iter = _updatedOrders.begin();
	while (iter != _updatedOrders.end()) {
		if (QDateTime::currentMSecsSinceEpoch() - iter.value() > 10000) {
			qInfo() << "remove updated order:" << iter.key();
			iter = _updatedOrders.erase(iter);
			continue;
		}
		++iter;
	}
}

void SunnyTcsOrderModel::orderUpdatedSlot(const QString& ordID, qint8 state, const QString& failReson)
{
	auto iter = _orders.begin();
	while (iter != _orders.end()) {
		if (iter->id == ordID) {
			if (7 == state) { //Failed
				iter->failedReason = failReson;
			}

			iter->statu = stateString(state);
			qInfo() << "order:" << ordID << ", updated";

			_updatedOrders[ordID] = QDateTime::currentMSecsSinceEpoch();
			return;
		}
		++iter;
	}

	removeOverTimeUpdatedOrders();
}

void SunnyTcsOrderModel::orderCreatedSlot(const QByteArray& data)
{
	Order ord;
	QJsonObject jo = QJsonDocument::fromJson(data).object();
	QString id = jo["ID"].toString();
	if (id.isEmpty()) {
		return;
	}
	ord.id = id;
	qInfo() << "order created:" << id;
	ord.vehicle = jo["AGV"].toString();
	qint32 state = jo["State"].toInt(-1);
	if (state < 0) {
		return;
	}
	ord.statu = stateString(state);
	ord.isCyclic = jo["Cyclic"].toBool(false);
	ord.startTime = jo["StartTime"].toString();
	QJsonArray jarr = jo["RoutePoints"].toArray();
	for (auto ja : jarr) {
		QJsonObject temp = ja.toObject();
		qint32 pid = jo["ParkPoint"].toInt(0);
		if (0 == pid) {
			continue;
		}
		QString loc = jo["ParkLocation"].toString();
		ord.points.append(QPair<qint32, QString>(pid, loc));
	}
	_orders.append(ord);
	_updatedOrders[id] = QDateTime::currentMSecsSinceEpoch();
	removeOverTimeUpdatedOrders();
}

SunnyTcsOrderModel::SunnyTcsOrderModel(/* args */)
{
}

SunnyTcsOrderModel::~SunnyTcsOrderModel()
{
}

QList<QString> SunnyTcsOrderModel::updatedOrders()
{
	removeOverTimeUpdatedOrders();
	return _updatedOrders.keys();
}

void SunnyTcsOrderModel::syncDataReadySlot(const QByteArray& data)
{
	QJsonParseError err;
	QJsonDocument jd = QJsonDocument::fromJson(data, &err);
	if (!jd.isNull() && err.error == QJsonParseError::NoError)
	{
		QJsonObject jo = jd.object();
		QString ty = jo["elementType"].toString();
		if (ty == "Order")
		{
			Order ord;
			ord.id = jo["ID"].toString();
			ord.vehicle = jo["Vehicle"].toString();
			ord.statu = jo["Statu"].toString();
			ord.startTime = jo["StartTime"].toString();
			ord.isCyclic = jo["Cyclic"].toBool();
			_orders.append(ord);
		}
	}
}