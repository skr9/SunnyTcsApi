/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2018~2019 Sunny-baer Company Ltd.
 * Contract: http://www.sunny-baer.com
 *
 * Describle:
 * In SunnyTcs Message system, one application which send/recv messages called
 * a 'node'. SunnyTcsZmqNode(because base on zeromq) is the abstraction of this
 * type node. you can use this class to send or recv messages. in it's construct
 * function, you can set the node to be server or not by the arg 'server',
 * but you should never set it to be true except you really want to create a 'broker',
 * because in the message system, only the 'broker' is the server, and all other
 * applications are just a client
 *
 * Version:
 *
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef SUNNYTCSZMQNODE_H
#define SUNNYTCSZMQNODE_H

#include <zmq.hpp>
#include <QObject>
#include <memory>
#include "sunnytcsmessage_global.h"
#include "sunnytcsmessage.hpp"
#include "sunnytcsmixedmessage.hpp"
#include <iostream>

using namespace std;

#define SERROR_OK 0
#define SERROR_SOCKET_UNINITILIZED -1
#define SERROR_CONTEXT_UNCORRECT -2

#define NODE_IDENTITY_PREFIX "_stcs_node_"
#define DEFAULT_HWM 10000

class SunnyTcsBroker;
class SunnyTcsZmqNode:public QObject
{
    typedef zmq::context_t zcontext;
    typedef zmq::socket_t zsocket; 

    friend class SunnyTcsBroker;

public:
    explicit SunnyTcsZmqNode(qint16 id, bool server = false, QObject *parent = nullptr):
        QObject (parent), _id(id)
    {
        _context = new zcontext(1);
        _isServer = server;
        _socket = new zsocket(*_context, server ? ZMQ_ROUTER : ZMQ_DEALER);
        setSndHwm(0); //取消阈值限制
        setRecvHwm(0);
        _lastErrorCode = 0;
        _lastErrorInfo = "";
    }

    //析构:释放连接
    ~SunnyTcsZmqNode(){
        if(_socket)
        {
            delete _socket;
            _socket = nullptr;
        }

        if(_context)
        {
            delete _context;
            _context = nullptr;
        }
    }

    inline qint16 id()const{return _id;}

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 设置高水位线，当发送端缓存消息数量大于该水位线时，
     * 将暂时阻塞发送线程，直至缓存量重新回到高水位线以下
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    void setSndHwm(quint32 hwm)
    {
        Q_ASSERT(hwm >= 0);
        _socket->setsockopt(ZMQ_SNDHWM, hwm);
    }

    void setRecvHwm(quint32 hwm)
    {
        Q_ASSERT(hwm >= 0);
        _socket->setsockopt(ZMQ_RCVHWM, hwm);
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * connect this node to the endpoit('ipAddress:port')
     * and if connected successfully, it return true,
     * else return false
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool connectTo(QString ipAddress, qint32 port)
    {
        _connectedBroker = QString("tcp://%1:%2").arg(ipAddress).arg(port);
        QString identity(NODE_IDENTITY_PREFIX + QString::number(_id));
        _socket->setsockopt(ZMQ_IDENTITY, identity.toStdString().data(), static_cast<size_t>(identity.size()));
        _socket->connect(_connectedBroker.toStdString().c_str());
        _lastRecvTime = QDateTime::currentMSecsSinceEpoch();
        _lastSendTime = _lastRecvTime;
        return _socket->connected();
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * just for 'broker' node, bind the node
     * on the 'port', and if bind successfully,
     * it return true, else false
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool bind(qint32 port)
    {
        if(!_isServer) return false;

        _socket->bind(QString("tcp://*:%1").arg(port).toStdString());

        return (zmq_errno() == 0 || zmq_errno() == EAGAIN);
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 将指定字节数组（第一个参数指定）发送到指定
     * 节点（第二个参数），如果发送节点是消息中间件，
     * 则根据目标节点自动添加消息信封，使消息能正确
     * 路由至目标节点，这是消息中间件实现自定义路由
     * 的关键所在
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    qint32 send(const QByteArray& bytes, qint16 dst = -1)
    {
        try {
            do{
                if(!_socket->connected())
                {
                    _lastErrorInfo = "unconnected";
                    _lastErrorCode = SERROR_SOCKET_UNINITILIZED;
                    return _lastErrorCode;
                }

                if(_isServer) //route to destion when msg is send by broker(router)
                {
                    qint16 to = dst < 0 ? SunnyTcsMessageParser(bytes).to() : dst;
                    if(to < 0)
                    {
                        _lastErrorCode = SERROR_CONTEXT_UNCORRECT;
                        _lastErrorInfo = tr("目标节点错误");
                        return SERROR_CONTEXT_UNCORRECT;
                    }

                    QString destAddress(NODE_IDENTITY_PREFIX + QString::number(to));
                    if(_socket->send(destAddress.toStdString().data(), static_cast<size_t>(destAddress.size()), ZMQ_SNDMORE) <= 0) break;
                }

                if(_socket->send(bytes, static_cast<size_t>(bytes.size()), 0) <= 0) break;

                _lastErrorCode = SERROR_OK;
                _lastErrorInfo = "OK";
                _lastSendTime = QDateTime::currentMSecsSinceEpoch();
                return SERROR_OK;
            }while(0);
        } catch (...) {
            return zmq_errno();
        }

        return zmq_errno();
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * send the message to destination, if this
     * node is a server(broker), it would find
     * the destination according to 'to' info in
     * 'msg'
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    template <class msgArgType>
    qint32 send(SunnyTcsMessage<msgArgType> &msg)
    {
        msg.setFrom(_id);
        return send(msg.toBytes(), msg.to());
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 发送混合参数类型的消息
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    qint32 sendMixedMsg(SunnyTcsMixedMessage& msg)
    {
        msg.setFrom(_id);
        return send(msg.toBytes());
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 向消息服务（中间件）发送ready消息，说明本节点
     * 已经可以加入到消息网中进行消息收发
     * 此操作只对client节点有意义
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool sendReadyMsg()
    {
        SunnyTcsMessage<char> readyMsg(MESSAGE_READY, MESSAGE_ID_READY, MESSAGE_NODE_BROKER);
        bool ret = (send(readyMsg) == SERROR_OK);
        _lastRecvTime = QDateTime::currentMSecsSinceEpoch();
        _lastSendTime = _lastRecvTime;
        return ret;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 向消息服务器发送心跳信号，只对client节点有意义
     * 并且client只需在空闲时（即不发送其余消息时）发送
     * 心跳即可
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool sendHeartBeatSignal(qint32 liveMs, qint16 dst = MESSAGE_NODE_BROKER)
    {
        SunnyTcsMessage<qint32> heartBeat(MESSAGE_HEARTBEAT, MESSAGE_ID_HEARTBEAT, dst);
        heartBeat.addArg(liveMs);
        if(send(heartBeat) == SERROR_OK)
        {
            _lastSendTime = QDateTime::currentMSecsSinceEpoch();
            return true;
        }

        return false;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 在消息网中注册一组会话，用于在该会话组内进行
     * 消息广播
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool registeSession(QString topic)
    {
        SunnyTcsMessage<QString> regMsg(MESSAGE_REGISTE, MESSAGE_ID_REGISTE, MESSAGE_NODE_BROKER);
        regMsg.addArg(topic);
        bool ret = (send(regMsg) == SERROR_OK);
        if(ret && !_registedTopics.contains(topic))
            _registedTopics.append(topic);
        return ret;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 当节点与broker因为网络异常等原因断开后，可调用
     * 该函数重新连接。
     * 注意该函数成功的前提时，本节点对象已成功与broker
     * 连接过，且完成了ready消息发送和话题注册。
     * 本函数负责向broker重新发送ready消息，用于在broker
     * 端重新注册该节点；另外也会将原本已注册的话题重新注册
     * 一遍
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    bool reconnect()
    {
        bool ret = sendReadyMsg();
        for(QString topic : _registedTopics)
        {
            SunnyTcsMessage<QString> regMsg(MESSAGE_REGISTE, MESSAGE_ID_REGISTE, MESSAGE_NODE_BROKER);
            regMsg.addArg(topic);
            ret &= (send(regMsg) == SERROR_OK);
        }
        return ret;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 接收指定类型的消息:
     * 如果成功，返回接收到的消息；
     * 如果失败，返回一个错误消息
     * 注意：此函数不再被建议使用，效率较低，且可能引起误判，
     * 建议使用下方同名函数替代
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    template <class msgArgType>
    SunnyTcsMessage<msgArgType> recv()
    {
        SunnyTcsMessage<msgArgType> stm;

        QByteArray bytes = recv();
        if(!SunnyTcsMessage<msgArgType>::check(bytes)) return stm;

        stm.parseFrom(std::move(bytes));
        return stm;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 接收并解析混合参数类型的消息
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    SunnyTcsMixedMessage recvMixed()
    {
       SunnyTcsMixedMessage msg;
       QByteArray bytes = recv();
       if(!SunnyTcsMessage<SunnyTcsMsgArg>::check(bytes)) return msg;
       msg.parseFrom(bytes);
       return msg;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     *用于接收指定类型消息：
     * 如果接收成功，将接收到的消息放入result参数中（
     * 调用方需负责清理该消息占用的内存），并返回true；
     * 如果失败，直接返回false
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    template<class msgArgType>
    bool recv(SunnyTcsMessage<msgArgType>** result)
    {
        QByteArray bytes = recv();
        if(bytes.isEmpty() || !SunnyTcsMessage<msgArgType>::check(bytes)) return false;

        *result = new SunnyTcsMessage<msgArgType>();
        (*result)->parseFrom(std::move(bytes));
        return true;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 接收任何类型消息，如接收成功，该函数会返回
     * 接收到的消息字节数组
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    QByteArray recv()
    {
        QByteArray retBytes;

        zmq::pollitem_t pollItems[] = {{*_socket, 0, ZMQ_POLLIN, 0}};

        try {
            qint32 rc = zmq::poll(pollItems, 1, 0);
            if(rc < 0) return retBytes;
        } catch (zmq::error_t& e) {
            _lastErrorCode = e.num();
            _lastErrorInfo = e.what();
            return  retBytes;
        }

        if(pollItems[0].revents & ZMQ_POLLIN)
        {
            zmq::message_t msg;
            _socket->recv(&msg, 0);
            retBytes.append(static_cast<char*>(msg.data()), static_cast<qint32>(msg.size()));
            _lastRecvTime = QDateTime::currentMSecsSinceEpoch();
        }

        return retBytes;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * cut off the connection if it is connected
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    void close()
    {
        if(_socket)
            _socket->close();
    }

    qint64 lastSendTime()const{return _lastSendTime;}
    qint64 lastRecvTime()const{return _lastRecvTime;}
    inline qint32 lastErrorCode()const{return _lastErrorCode;}
    inline QString lastError()const{return _lastErrorInfo;}

private:
    qint16 _id;              //identify of the node,it should be unique
    zcontext* _context;
    zsocket* _socket;
    QString _connectedBroker;
    qint64 _lastSendTime;
    qint64 _lastRecvTime;
    QStringList _registedTopics; //已经注册的所有会话主题
    bool _isServer;

    qint32 _lastErrorCode;
    QString _lastErrorInfo;
};
#endif // SUNNYTCSZMQNODE_H
