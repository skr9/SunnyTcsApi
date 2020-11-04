#ifndef SUNNYTCSSESSION_H
#define SUNNYTCSSESSION_H

#include <QObject>
#include <QVector>


class SunnyTcsSession:public QObject
{
public:
    explicit SunnyTcsSession(QObject *parent = nullptr);
    explicit SunnyTcsSession(QString topic, QObject *parent = nullptr);
    SunnyTcsSession(const SunnyTcsSession& session);

    QString topic()const{return _topic;}

    QVector<qint16> nodes(){return _nodes;}
    void addNode(qint16 node);
    void removeNode(qint16 node);
    bool exist(qint16 node);

    inline qint32 nodeCount()const{return _nodes.size();}
    qint16 nodeAt(qint32 index);

private:
    QString _topic;
    QVector<qint16> _nodes;
};

#endif // SUNNYTCSSESSION_H
