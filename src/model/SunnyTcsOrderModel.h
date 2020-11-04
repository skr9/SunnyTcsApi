#ifndef SUNNY_TCS_ORDER_MODEL_H
#define SUNNY_TCS_ORDER_MODEL_H

#include <qobject.h>
#include <qmap.h>

typedef struct {
	QString id;
	QString vehicle;
	QString statu;
	QList<QPair<qint32, QString>> points; //订单途径点集<point， location>
	bool isCyclic;
	QString startTime;
	QString failedReason;
}Order;



class SunnyTcsOrderModel:public QObject
{
	Q_OBJECT

private:
	QList<Order> _orders;
	QMap<QString, qint64> _updatedOrders; //状态已更新的订单，保留10s, <OrderID, lastUpdateMillsecods>

	QString stateString(qint32 state);
	void removeOverTimeUpdatedOrders();

public slots:
	void syncDataReadySlot(const QByteArray& data);
	void orderUpdatedSlot(const QString& ordID, qint8 state, const QString& failReson);
	void orderCreatedSlot(const QByteArray& data);

public:
    SunnyTcsOrderModel(/* args */);
    ~SunnyTcsOrderModel();

	QList<Order> orders()const { return _orders; }
	QList<QString> updatedOrders();
};


#endif