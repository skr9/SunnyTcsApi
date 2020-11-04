#ifndef SUNNYTCSMIXEDMESSAGE_H
#define SUNNYTCSMIXEDMESSAGE_H

#include <QObject>
#include "sunnytcsmessage.hpp"

class SunnyTcsMsgArg
{
public:
    SunnyTcsMsgArg()
    {
        _ty = ARG_TYPE_UNDEFINED;
        _len = 0;
    }

    SunnyTcsMsgArg(ARG_TYPE ty, qint8 len, QByteArray valueBytes)
    {
        _ty = ty;
        _len = len;
        _value = valueBytes;
    }

    SunnyTcsMsgArg(const SunnyTcsMsgArg& rhs)
    {
        _ty = rhs._ty;
        _len = rhs._len;
        _value = rhs._value;
    }

    ARG_TYPE type()const{return _ty;}
    qint8 size()const{return _len + 2;}
    QByteArray toBytes()
    {
        QByteArray bytes;
        bytes.append(qint8(_ty));
        bytes.append(_len);
        bytes.append(_value);
        return bytes;
    }

    qint8 toChar()const{return _value[0];}
    qint16 toShort()const{
        return _value.toHex().toShort(nullptr, 16);
    }
    qint32 toInt()const
    {
        return _value.toHex().toInt(nullptr, 16);
    }
    qint64 toInt64()const
    {
        return _value.toHex().toLongLong(nullptr, 16);
    }
    QString toString()const
    {
        return QString::fromLocal8Bit(_value);
    }

private:
    ARG_TYPE _ty;
    qint8 _len;
    QByteArray _value;
};

class SunnyTcsMixedMessage:public QObject
{
public:
    explicit SunnyTcsMixedMessage(QObject* parent = nullptr):QObject(parent),
        _header(new SunnyTcsMsgHeader()) {
        _broadCastTopic = "";
        _bodySize = 0;
    }

    SunnyTcsMixedMessage(qint8 mt, qint16 msgID, qint16 to, QObject* parent = nullptr):QObject(parent)
    {
        _header.reset(new SunnyTcsMsgHeader(mt, msgID, to, ARG_TYPE_MIXED, parent));
        _broadCastTopic = "";
        _bodySize = 0;
    }

    SunnyTcsMixedMessage(const QByteArray& msgBytes, QObject* parent = nullptr):QObject(parent),
        _header(new SunnyTcsMsgHeader())
    {
        parseFrom(msgBytes);
    }

    SunnyTcsMixedMessage(const SunnyTcsMixedMessage& msg):QObject(msg.parent()),
    _header(new SunnyTcsMsgHeader(*(msg._header)))
    {
        _body = msg._body;
        _broadCastTopic = msg._broadCastTopic;
    }
    ~SunnyTcsMixedMessage(){}

    size_t size()const{
        return _header->size() + _bodySize + (isBroadCast() ? 1 + _broadCastTopic.size() : 0);
    }
    inline void setSize(qint16 sz){_header->setMsgLength(sz);}
    bool isEmpty()const{return MESSAGE_FOR_ERROR == _header->msgType();}

    bool isBroadCast()const{return _header->msgType() == MESSAGE_BROADCAST;}
    QString topic()const{return _broadCastTopic;}
    void setTopic(QString topic){_broadCastTopic = topic;}

    bool hasBody()const{return !_body.isEmpty();}
    qint8 messageType()const{return _header->msgType();}
    qint16 messageId()const{return _header->msgID();}
    qint16 from()const{return _header->from();}
    void setFrom(qint16 from){_header->setFrom(from);}
    qint16 to()const{return _header->to();}
    qint8 argType()const{return _header->argTypeID();}
    qint32 headerSize()const{return SunnyTcsMsgHeader::size();}
    QByteArray headerBytes()const{return _header->toBytes();}
    const QList<SunnyTcsMsgArg>* args()const{return &_body;}

    void appendArg(qint8 arg){
        _body.append(SunnyTcsMsgArg(ARG_TYPE_CHAR, 1, QByteArray::number(arg, 16)));
        _bodySize += 3;
    }

    void appendArg(qint16 arg)
    {
        _body.append(SunnyTcsMsgArg(ARG_TYPE_QINT16, 2, itoBytes(arg, 2)));
        _bodySize += 4;
    }

    void appendArg(qint32 arg)
    {
        _body.append(SunnyTcsMsgArg(ARG_TYPE_QINT32, 4, itoBytes(arg, 4)));
        _bodySize += 6;
    }

    void appendArg(qint64 arg)
    {
        _body.append(SunnyTcsMsgArg(ARG_TYPE_QINT64, 8, itoBytes(arg, 8)));
        _bodySize += 10;
    }

    void appendArg(QString arg)
    {
        QByteArray bytes = arg.toLocal8Bit();
        qint32 count = bytes.count();
        Q_ASSERT(count <= 255);
        _body.append(SunnyTcsMsgArg(ARG_TYPE_QSTRING, static_cast<qint8>(count), bytes));
        _bodySize += (2 + count);
    }

    void appendArg(SunnyTcsMsgArg arg)
    {
        _body.append(arg);
        _bodySize += arg.size();
    }

    QByteArray toBytes()
    {
        QByteArray bytes;
        _header->setMsgLength(size());
        bytes.append(headerBytes());

        if(_header->msgType() == MESSAGE_BROADCAST)
        {
            bytes.append(static_cast<qint8>(_broadCastTopic.size() & 0xFF));
            bytes.append(_broadCastTopic.toLocal8Bit());
        }

        auto iter = _body.begin();
        while(iter != _body.end())
        {
            bytes.append(iter->toBytes());
            ++iter;
        }
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
            _broadCastTopic =QString::fromStdString(bytes.mid(minSz, sz).toStdString());
            minSz += static_cast<size_t>(sz);
        }

        rc &= parseBodyFromBytes(bytes.right(bytes.count() - minSz));
        return rc;
    }

private:
    unique_ptr<SunnyTcsMsgHeader> _header;
    QString _broadCastTopic;
    QList<SunnyTcsMsgArg> _body;
    qint32 _bodySize;

    bool parseHeaderFromBytes(QByteArray bytes){return _header->fromBytes(std::move(bytes));}
    bool parseBodyFromBytes(QByteArray bytes)
    {
        qint32 index = 0;
        qint32 count = bytes.count();
        while((index + 2) <  count)
        {
            ARG_TYPE ty = ARG_TYPE(qint8(bytes[index++]));
            qint8 len = bytes[index++];
            if((index + len) > count) return false;
            _body.append(SunnyTcsMsgArg(ty, len, bytes.mid(index, len)));
            index += len;
            _bodySize += (len + 2);
        }
        return true;
    }
    QByteArray itoBytes(qint64 number, qint8 len = 8)
    {
        QByteArray bytes;
        for(qint32 i = len - 1; i >= 0; --i)
            bytes.append((number >> (i * 8)) & 0xFF);
        return bytes;
    }
};
#endif // SUNNYTCSMIXEDMESSAGE_H
