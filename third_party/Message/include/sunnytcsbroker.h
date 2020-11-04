/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2018~2019 Sunny-baer Company Ltd.
 * Contract: http://www.sunny-baer.com
 *
 * Describle:
 * SunnyTcsBroker is a message broker, routing and dispatch messages for all
 * nodes in SunnyTcs System 2.0, it is base on zmq
 * it can deal with 5 types message:REQ/REP/READY/HEARTBEAT/DISCONNECT
 * in these types, READY and HEARTBEAT just suitable for workers, READY
 * is send by worker to tell broker that he is ready for work now, HEARTBEAT
 * message is for broker to kown the worker is still alive when the worker isn't
 * in busy state
 * the broker listen all the connection on fixed endpoint,when the new connection
 * come in, it would automatically add the node who created this connection to the
 * node list if it is not.
 * when the new message come in, the broker would find out the message type, and make
 * different response for different kinds of message. if it is a REQ/REP message,
 * broker would find out the destination of the message, and send the message to
 * the right node; and if it is DISCONNECT message, broker would delete the node
 * from the node list and then cut off the connection.
 * what's more, you can set accessable nodes for all nodes, and then just the accessable
 * nodes can send message to this node
 *
 * Version:
 *
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef SUNNYTCSBROKER_H
#define SUNNYTCSBROKER_H

#include "sunnytcsmessage_global.h"
#include <zmq.hpp>
#include <QThread>
#include "sunnytcszmqnode.hpp"
#include "sunnytcssession.h"
#include <QMap>

#define DEFAULT_HEART_BEAT_INTERVAL 5000
#define DEFAULT_CONFIG_FILE "config\\sunnytcs_broker_config.xml"

class SUNNYTCSMESSAGESHARED_EXPORT SunnyTcsBroker:QThread
{
public:
    explicit SunnyTcsBroker(quint16 port = BROKER_LISTEN_PORT,
                            qint32 heartBeatInterval = DEFAULT_HEART_BEAT_INTERVAL,
                            QObject* parent = nullptr);

    bool startup(QString configFile = DEFAULT_CONFIG_FILE);
    void shutdown();
    void enableLogBroadcast(bool enable){_logRecord = enable;}
    void enableAutoFeedback(bool enable){_autoFeedback = enable;}

    inline void resetHeartBeatInterval(qint32 interval){
        _defaultHeartbeatInterval = interval > 0 ? interval : _defaultHeartbeatInterval;
    }

private:
    quint16 _port;
    QString _lastError;
    QVector<SunnyTcsSession> _sessions;
    QMap<qint16, qint64> _allNodes;
    qint32 _defaultHeartbeatInterval;
    bool _logRecord; //是否启用消息件日志，若启用，会自动注册一个日志广播主题，任意节点订阅该主题后可收到消息件日志
    QString _logTopic; //日志广播主题
    QString _configFile;
    bool _autoFeedback; //是否允许自动回复收到确认消息

    void run() Q_DECL_OVERRIDE;

    bool readXmlConfig();
    void setDefaultConfig();

    void msgHandle(SunnyTcsZmqNode* broker, QByteArray& msgBytes);
    void liveCheck(SunnyTcsZmqNode* broker);

    void broadCastLog(SunnyTcsZmqNode* broker, QString logInfo);
    void broadCastLog(SunnyTcsZmqNode* broker, QByteArray& msgBytes, qint16 sender);

    void dealReadyMsg(SunnyTcsZmqNode* broker, const QByteArray& msgBytes);
    void notifyReadyConfirm(SunnyTcsZmqNode* broker, qint16 client, qint8 confirmFlag);

    void dealRegiste(SunnyTcsZmqNode* broker, QByteArray& msgBytes);
    void registe(QString topic, qint16 node);

    qint32 dealHeartBeat(SunnyTcsZmqNode* broker, const QByteArray& msgBytes);

    void dealUnicast(SunnyTcsZmqNode* broker, QByteArray& msgBytes);

    void dealBroadcast(SunnyTcsZmqNode* broker,  QByteArray& msgBytes);
    void broadcast(SunnyTcsZmqNode* broker, qint16 from, QString& topic, QByteArray& msgBytes);

    void dealDisconnect(SunnyTcsZmqNode* broker, const QByteArray& msgBytes);
    void killNode(SunnyTcsZmqNode* broker, qint16 node, QString reason);

    void dealQuery(SunnyTcsZmqNode* broker, const QByteArray& msgBytes);

    void feedBack(SunnyTcsZmqNode* broker, qint16 recivedMsg, qint16 dst);
};

#endif // SUNNYTCSBROKER_H
