#ifndef SUNNY_TCS_API_MAP_MODEL_H
#define SUNNY_TCS_API_MAP_MODEL_H

#include <QObject>

class MapPoint
{
public:
	MapPoint(const QString& name, qint32 id, qint32 pos):_name(name), _id(id), _pos(pos)
	{
	}

	~MapPoint(){}

	inline QString name()const { return _name; }
	inline qint32 id()const { return _id; }
	inline qint32 pos()const { return _pos; }

	bool operator==(const MapPoint& rhs)const
	{
		return _id == rhs.id();
	}

private:
	QString _name;
	qint32 _id;
	qint32 _pos;
};


class SunnyTcsApiMapModel:public QObject
{
	Q_OBJECT

private:
	QList<MapPoint> _points;

public slots:
	void syncDataReadySlot(const QByteArray& data);

public:
	SunnyTcsApiMapModel();
	~SunnyTcsApiMapModel();

	QList<MapPoint> points()const { return _points; }
};

#endif

