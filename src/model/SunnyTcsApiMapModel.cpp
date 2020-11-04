#include "SunnyTcsApiMapModel.h"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qdebug.h>

SunnyTcsApiMapModel::SunnyTcsApiMapModel()
{
}


SunnyTcsApiMapModel::~SunnyTcsApiMapModel()
{
}

void SunnyTcsApiMapModel::syncDataReadySlot(const QByteArray& data)
{
	QJsonParseError err;
	QJsonDocument jd = QJsonDocument::fromJson(data, &err);
	if (!jd.isNull() && err.error == QJsonParseError::NoError)
	{
		QJsonObject jo = jd.object();
		QString ty = jo["elementType"].toString();
		if (ty == "Point")
		{
			QString name = jo["Name"].toString();
			qint32 id = jo["ID"].toInt();
			qint32 pos = jo["AbsPos"].toInt();
			MapPoint pt(name, id, pos);

			if (!_points.contains(pt))
			{
				_points.append(pt);
				qInfo() << "add point:" << name << ";current count" << _points.count();
			}
		}
	}
}
