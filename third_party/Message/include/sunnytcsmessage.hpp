/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2018~2019 Sunny-baer Company Ltd.
 * Contract: http://www.sunny-baer.com
 *
 * Describle:
 * SunnyTcsMessage is the abstraction of the message send/recv between two
 * zmq nodes(mostly between client and broker). it contains two parts in one
 * message,the message header and the message body, header is to tell reciver
 * some basic information,for example:message type, message id, who send the
 * message(from),and who should recive this message...; message body contains
 * arg informations of this message.
 * accroding to current design, the format of message header is fixed, it is
 * consisted by: message type(1 byte), check bytes(2 bytes), message id(2 bytes),
 * who send this message('from', 2 bytes), who should recive this message('to',
 * 2 bytes) and the type identify of body arg(1 bytes)
 * the format of message body is different with header, which is mutable, that
 * is to say, whether the count of args or the data type of the arg, is not fixed.
 * so, when you recived a message with body, you should get the arg type information
 * from arg type byte(which is the last byte of header), and than parser the args
 * according to the arg type.
 * we designed a class 'SunnyTcsMessageParser' to help you decode the message.
 * you can simply create a instance of this class with the bytes you got from
 * the msg, and then, use the member function of SunnyTcsMessageParser to get
 * the informations in this message
 * or if you kown the arg type of message which you just recived, you can create
 * a instance of 'SunnyTcsMessage' with the arg type(for example,if you recived a
 * string arg message, you can create a message object like this: SunnyTcsMessage<QString> msg)
 * and then, you have the msg object which contains all information, you can
 * use the member function of this object to get the information which you need
 *
 * Version:
 *
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef SUNNYTCSMESSAGE_H
#define SUNNYTCSMESSAGE_H

#include "sunnytcsmsgargpolicy.hpp"
#include "sunnytcsargtypetraits.h"
#include "sunnytcsmessage_global.h"
#include <memory>
#include <QVector>
#include <QDateTime>
using namespace std;

#define UNUSEABLE_MESSAGE_ID 0  //this id is unuseable
#define UNUSEABLE_NODE_ID -1

class SunnyTcsMsgHeader:public QObject
{
public:
    SunnyTcsMsgHeader(qint8 mt, qint16 msgID, qint16 to, qint8 argType, QObject* parent = nullptr):
        QObject(parent), _msgType(mt), _time(QDateTime::currentMSecsSinceEpoch()), _msgID(msgID),
        _from(UNUSEABLE_NODE_ID), _to(to), _argTypeID(argType){}

    SunnyTcsMsgHeader(const SunnyTcsMsgHeader& msg):
        QObject(msg.parent()), _msgType(msg._msgType), _time(msg.time()), _msgID(msg._msgID),
        _from(msg._from), _to(msg._to), _argTypeID(msg._argTypeID){}

    SunnyTcsMsgHeader(qint8 mt, qint16 msgID, qint8 argType, QObject *parent = nullptr):
        QObject(parent), _msgType(mt), _time(QDateTime::currentMSecsSinceEpoch()), _msgID(msgID),
        _from(UNUSEABLE_NODE_ID), _to(UNUSEABLE_NODE_ID), _argTypeID(argType){}

    explicit SunnyTcsMsgHeader(QObject *parent = nullptr):
        QObject (parent), _msgType(MESSAGE_FOR_ERROR), _time(QDateTime::currentMSecsSinceEpoch()), _msgID(UNUSEABLE_MESSAGE_ID),
        _from(UNUSEABLE_NODE_ID), _to(UNUSEABLE_NODE_ID),
        _argTypeID(static_cast<qint8>(ARG_TYPE_UNDEFINED)){}

    ~SunnyTcsMsgHeader(){}

    qint64 time()const{return _time;}
    qint8 msgType()const{return _msgType;}
    qint16 msgLength()const{return _msgLength;}
    void setMsgLength(qint16 len){_msgLength = len > 0 ? len : 0;}
    qint16 msgID()const{return _msgID;}
    qint16 from()const{return _from;}
    void setFrom(qint16 from){_from = from;}

    qint16 to()const{return  _to;}
    static bool dstReplace(QByteArray& headerBytes, qint16 newDst)
    {
        if(headerBytes.count() < static_cast<qint32>(size())) return false;

        headerBytes[15] = static_cast<qint8>((newDst >> 8) & 0xFF);
        headerBytes[16] = static_cast<qint8>(newDst & 0xFF);

        return true;
    }

    static bool lenBytesReplace(QByteArray& bytes, qint16 newLen)
    {
        if(bytes.count() < static_cast<qint32>(size())) return false;
        bytes[9] = static_cast<qint8>((newLen >> 8) & 0xFF);
        bytes[10] = static_cast<qint8>(newLen & 0xFF);
        return true;
    }

    qint8 argTypeID()const{return _argTypeID;}

    QByteArray toBytes()const{
        QByteArray header;
        header.append(static_cast<qint8>(_msgType & 0XFF));

        for(qint32 i = 7; i >= 0; --i)
        {
            header.append(static_cast<qint8>((_time >> (i * 8)) & 0xFF));
        }

        header.append(static_cast<qint8>((_msgLength >> 8) & 0xFF));
        header.append(static_cast<qint8>(_msgLength & 0xFF));

        header.append(static_cast<qint8>((_msgID >> 8) & 0xFF));
        header.append(static_cast<qint8>(_msgID & 0xFF));

        header.append(static_cast<qint8>((_from >> 8) & 0xFF));
        header.append(static_cast<qint8>(_from & 0xFF));

        header.append(static_cast<qint8>((_to >> 8) & 0xFF));
        header.append(static_cast<qint8>(_to & 0xFF));

        header.append(static_cast<qint8>(_argTypeID & 0xFF));
        return  header;
    }

   static size_t size(){return _size;}

    bool fromBytes(QByteArray&& bytes)
    {
        if(bytes.count() != static_cast<qint32>(_size)) return false;

        try {
            _msgType = bytes[0];
            if(_msgType < MESSAGE_FOR_ERROR || _msgType > MESSAGE_BROADCAST)
            {
                _msgType = MESSAGE_FOR_ERROR;
                return false;
            }

            bool ok = true;
            do
            {
                _time = bytes.mid(1, 8).toHex().toLongLong(&ok, 16);
                if(!ok) break;

                _msgLength = bytes.mid(9, 2).toHex().toShort(&ok, 16);
                if(!ok) break;

                _msgID = bytes.mid(11, 2).toHex().toShort(&ok, 16);
                if(!ok) break;

                _from = bytes.mid(13, 2).toHex().toShort(&ok, 16);
                if(!ok) break;

                _to = bytes.mid(15, 2).toHex().toShort(&ok, 16);
                if(!ok) break;

                _argTypeID = bytes[bytes.count() - 1];
            }while(0);

            if(!ok)
            {
                _msgType = MESSAGE_FOR_ERROR;
                return false;
            }
        } catch (...) {
            return false;
        }

        return true;
    }

protected:
    qint8  _msgType; //type of message,MT
    qint64 _time;    //时间戳
    qint16 _msgLength; //for checking the length of message
    qint16 _msgID;   //id of message
    qint16 _from;   //who send this msg
    qint16 _to;     //who shall recive this msg
    qint8  _argTypeID; //the type id of args(if exists)
    const static size_t _size = 18; //qint64 + qint8 * 2 + qint16 * 4
};

class SunnyTcsMessageParser;
template <class argType,
          template <typename> class transformPolicy = SunnyTcsTansformPolicy,
          template<typename> class argTypeTraits = SunnyTcsArgTypeTraits>
class SunnyTcsMsgBody:public QObject
{
    typedef  QList<argType> argList;

    friend SunnyTcsMessageParser;

public:
    explicit SunnyTcsMsgBody(QObject* parent = nullptr):
        QObject(parent){
        _cur = -1;
    }

   SunnyTcsMsgBody(const SunnyTcsMsgBody& rhs):
        QObject(rhs.parent()), _args(rhs._args){
       _cur = rhs._cur;
   }

    ~SunnyTcsMsgBody(){}

    inline void addArg(argType& arg){_args.append(arg);}
    inline qint32 argCount()const{return _args.size();}

    QByteArray toBytes()const{
        QByteArray body;
        for(argType arg : _args)
            body.append(transformPolicy<argType>::toBytes(arg));
        return body;
    }

    size_t size()const
    {
        size_t sz = 0;
        auto iter = _args.begin();
        while(iter != _args.end())
        {
            sz += SunnyTcsArgTypeTraits<argType>::size(*iter);
            ++iter;
        }
        return sz;
    }

    bool fromBytes(QByteArray&& bytes){
      //  if(bytes.count() < static_cast<qint32>(sizeof(qint16))) return false;

        try {
            _args.clear();
            qint32 sz = SunnyTcsArgTypeTraits<argType>::size(argType());
            qint32 index = 0;
            while(bytes.count() >= (index + sz))
            {
                qint32 used = 0;
                auto arg = transformPolicy<argType>::fromBytes(bytes.right(bytes.count() - index), &used);
                if(used >= sz)
                    _args.append(arg);
                index += used;
            }
            _cur = -1;
        } catch (...) {
            _cur = -1;
            _args.clear();
            return false;
        }

        return true;
    }

    typename argList::const_iterator current(){return argAt(_cur);}

    typename argList::const_iterator argAt(qint32 i){
        return (i >= _args.count() || i < 0) ? _args.end() : _args.begin() + i;
    }

    bool move(){
        if(_cur >= _args.count() - 1)
            return false;

        _cur++;
        return true;
    }

    const argList& args(){return _args;}
    void clearArguments(){_args.clear();}

private:
    argList _args;
    qint32 _cur;
};

template <class msgArgType>
class SunnyTcsMessage:public QObject
{
    //Q_OBJECT
public:
    explicit SunnyTcsMessage(QObject* parent = nullptr):
        QObject (parent), _body(nullptr),
        _header(new SunnyTcsMsgHeader()), _broadcastTopic(""){}

    SunnyTcsMessage(qint8 mt, qint16 msgID, qint16 to, QObject *parent = nullptr):
        QObject(parent)
    {
        _header.reset(new SunnyTcsMsgHeader(mt, msgID, to, SunnyTcsArgTypeTraits<msgArgType>::typeID(), parent));
        _body.reset(new SunnyTcsMsgBody<msgArgType>(parent));
        _broadcastTopic = "";
    }

    SunnyTcsMessage(const QByteArray& msgBytes, QObject* parent = nullptr):
        QObject(parent),_header(new SunnyTcsMsgHeader())
    {
        parseFrom(msgBytes);
    }

    SunnyTcsMessage(const SunnyTcsMessage& msg):
                    QObject (msg.parent()),
                    _header(new SunnyTcsMsgHeader(*(msg._header))),
                    _body(msg._body ? new SunnyTcsMsgBody<msgArgType>(*(msg._body)) : nullptr),
                    _broadcastTopic(msg._broadcastTopic){}

    ~SunnyTcsMessage(){}

    size_t size()const{
        return headerSize() + bodySize() + (isBroadCast() ? 1 + _broadcastTopic.size() : 0);
    }
    inline void setSize(qint16 sz){_header->setMsgLength(sz);}
    bool isEmpty()const{return MESSAGE_FOR_ERROR == _header->msgType();}

    QString topic()const{return _broadcastTopic;}
    void setTopic(QString topic){_broadcastTopic = topic;}
    bool isBroadCast()const{return _header->msgType() == MESSAGE_BROADCAST;}

    QByteArray toBytes(){
        QByteArray bytes;
        _header->setMsgLength(size());
        bytes.append(headerBytes());

        if(_header->msgType() == MESSAGE_BROADCAST)
        {
            bytes.append(static_cast<qint8>(_broadcastTopic.size() & 0xFF));
            bytes.append(_broadcastTopic.toLocal8Bit());
        }

        if(hasBody())
            bytes.append(bodyBytes());

        return bytes;
    }

    bool parseFrom(const QByteArray& bytes)
    {       
        size_t minSz = headerSize();
        if(bytes.count() < static_cast<qint32>(minSz)) return false;

        bool rc = true;
        rc &= parseHeaderFromBytes(bytes.left(headerSize()));
        minSz += (isBroadCast() ? 1 : 0);

        if(isBroadCast())
        {
            qint16 sz = bytes.mid(headerSize(), 1).toHex().toShort(&rc, 16);
            if(bytes.count() < (static_cast<qint32>(minSz) + sz)) return false;
            _broadcastTopic =QString::fromStdString(bytes.mid(minSz, sz).toStdString());
            minSz += static_cast<size_t>(sz);
        }

        _body.reset(new SunnyTcsMsgBody<msgArgType>());
        rc &= parseBodyFromBytes(bytes.right(bytes.count() - minSz));

        return rc;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * check if the bytes is a message
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static bool check(const QByteArray& bytes)
    {
        if(bytes.count() <= 11) return  false;

        if(static_cast<qint8>(bytes[0]) == MESSAGE_FOR_ERROR) return false;

        qint16 len = bytes.mid(9, 2).toHex().toShort(nullptr, 16);
        if(bytes.count() != len) return false;

        return true;
    }

    void setBody(const SunnyTcsMsgBody<msgArgType>& inc){ _body.reset(inc);}
    bool hasBody()const{return _body.get() != nullptr; }
    void clearBody(){
        if(_body.get())
            _body->clearArguments();
    }

    qint8 messageType()const{return _header->msgType();}
    qint16 messageId()const{return _header->msgID();}
    qint16 from()const{return _header->from();}
    void setFrom(qint16 from){_header->setFrom(from);}
    qint16 to()const{return _header->to();}
    qint8 argType()const{return _header->argTypeID();}
    qint32 headerSize()const{return SunnyTcsMsgHeader::size();}
    QByteArray headerBytes()const{return _header->toBytes();}
    qint64 sendTime()const{return _header->time();}

    qint32 argCount()const{return _body.get() ? _body->argCount() : 0;}
    qint32 bodySize()const{return _body.get() ? _body->size() : 0;}
    void addArg(msgArgType arg){if(_body.get()) _body->addArg(arg);}
    QByteArray bodyBytes()const{return _body.get() ? _body->toBytes() : QByteArray();}

   typename QList<msgArgType>::const_iterator current(){return _body.get() ? _body->current() : nullptr;}
   typename QList<msgArgType>::const_iterator argAt(qint32 i){return _body.get() ? _body->argAt(i) : nullptr;}
   bool move(){return _body.get() ? _body->move() : false;}
   const QList<msgArgType>& args(){return _body->args();}

    void clear()
    {
        _header.reset(new SunnyTcsMessage(MESSAGE_FOR_ERROR));
        _body.reset(nullptr);
    }

private:
    unique_ptr<SunnyTcsMsgHeader> _header;
    QString _broadcastTopic;  //session topic for broadcast msg only
    unique_ptr<SunnyTcsMsgBody<msgArgType>> _body;

    bool parseHeaderFromBytes(QByteArray bytes){return _header->fromBytes(std::move(bytes));}
    bool parseBodyFromBytes(QByteArray bytes){return _body.get() ? _body->fromBytes(std::move(bytes)) : false;}
};

class SunnyTcsMessageParser
{
public:
    SunnyTcsMessageParser(QByteArray bytes)
    {
        _bytes.reset(new QByteArray(bytes));
        init();
    }

    SunnyTcsMessageParser(QByteArray&& bytes)
    {
        _bytes.reset(new QByteArray(bytes));
        init();
    }

    qint8 msgType()const{return _header->msgType();}
    qint16 msgID()const{return _header->msgID();}
    qint16 from()const{return _header->from();}
    qint16 to()const{return _header->to();}
    qint8 argType()const{return _header->argTypeID();}
    bool hasBody()const{return _hasBody;}

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * parse the message type from 'bytes'
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static qint8 parseMsgType(const QByteArray& bytes)
    {
        if(bytes.count() == 0) return static_cast<qint8>(ARG_TYPE_UNDEFINED);

        return bytes[0];
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * parse the arguments type id from the arg type bytes contained in 'bytes'
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static qint8 parseArgType(const QByteArray& bytes)
    {
        qint32 hsz = static_cast<qint32>(SunnyTcsMsgHeader::size());
        if(bytes.count() < hsz) return static_cast<qint8>(ARG_TYPE_UNDEFINED);

        return bytes.left(hsz)[hsz - 1];
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * parse the broad cast topic, if the msg
     * is a broad cast type msg
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static QString parseBroadcastTopic(const QByteArray& bytes)
    {
        QString rs;
        qint32 hsz = static_cast<qint32>(SunnyTcsMsgHeader::size());
        if(bytes.count() <= hsz) return rs;

        bool ok = true;
        qint16 sz = bytes.mid(hsz, 1).toHex().toShort(&ok, 16);
        if(sz < 1 || !ok) return rs;

        rs = QString::fromStdString(bytes.mid(hsz + 1, sz).toStdString());
        return rs;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 解析消息ID
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static qint16 parseMsgID(const QByteArray& bytes)
    {
        if(bytes.count() < 13) return MESSAGE_ID_ERROR;
        bool ok = false;
        qint16 msgID = bytes.mid(11, 2).toHex().toShort(&ok, 16);
        return ok ? msgID : MESSAGE_ID_ERROR;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 解析消息源点（from），也即消息是从哪发送而来
     * 如果解析失败，返回-1
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static qint16 parseFrom(const QByteArray& bytes)
    {
        if(bytes.count() < 15) return -1;
        bool ok = true;
        qint16 from = bytes.mid(13, 2).toHex().toShort(&ok, 16);
        return ok ? from : -1;
    }

    /*++++++++++++++++++++++++++++++++++++++++
     *function:
     * 解析消息终点（to），也即消息的目标接收者，
     * 如果解析失败，返回-1
     *params:
     *return:
     *version:
     *
     +++++++++++++++++++++++++++++++++++++++++*/
    static qint16 parseTo(const QByteArray& bytes)
    {
        if(bytes.count() < 17) return -1;
        bool ok = true;
        qint16 to = bytes.mid(15, 2).toHex().toShort(&ok, 16);
        return ok ? to : -1;
    }

    QString getBroadcastTopic()const{return _topic;}

    bool getChar(char* result){return getValue<char>(result);}
    bool getShort(qint16* result){return getValue<qint16>(result);}
    bool getInt(qint32* result){return getValue<qint32>(result);}
    bool getLong(qint64* result){return getValue<qint64>(result);}
    bool getString(QString* result){return getValue(result);}

    template<class T>
    bool getValue(T* result)
    {
        Q_ASSERT(result);

        qint32 sz = SunnyTcsArgTypeTraits<T>::size(*result);
        if(_bytes->count() < (_current + sz)) return false;

        qint32 used = 0;
        *result = SunnyTcsTansformPolicy<T>::fromBytes(_bytes->right(_bytes->count() - _current), &used);
        if(used >= sz)
            _current += used;
        else
            return false;

        return true;
    }

    bool atEnd()const{return _bytes->count() <= _current;}

private:
    unique_ptr<QByteArray> _bytes;
    unique_ptr<SunnyTcsMsgHeader> _header;
    QString _topic;
    bool _hasBody;
    qint32 _current;

    void init()
    {
        _header.reset(new SunnyTcsMsgHeader());
        _header->fromBytes(_bytes->left(static_cast<qint32>(SunnyTcsMsgHeader::size())));
        _current = static_cast<qint32>(SunnyTcsMsgHeader::size());
        _hasBody = (_bytes->count() > _current);

        if(_header->msgType() == MESSAGE_BROADCAST)
        {
            _topic = parseBroadcastTopic(*_bytes);
            _current += _topic.size() + 1;
        }
        else {
            _topic = "";
        }
    }
};

#endif // SUNNYTCSMESSAGE_H
