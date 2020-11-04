/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2018~2019 Sunny-baer Company Ltd.
 * Contract: http://www.sunny-baer.com
 *
 * Describle:
 * when send messages, all data is send/recv in bytes, because the arg type
 * in message body is mutable, so the way of encode(arg to bytes) and decode
 * (bytes to arg) is different from type to type.
 * for resloving this problem, we define the transform template to help encoding
 * and decoding args, and for all arg types supported currently, we specialized
 * the template for all these types
 *
 * Version:
 *
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef SUNNYTCSMSGARGPOLICY_H
#define SUNNYTCSMSGARGPOLICY_H

#include <qobject.h>
#include "sunnytcsmessage_global.h"
#include <QDebug>

template <class msgArgType>
class SunnyTcsTansformPolicy
{
public:
    static QByteArray toBytes(msgArgType arg){

        return QByteArray(0, sizeof(arg));
    }

    static msgArgType fromBytes(QByteArray bytes, qint32* usedBytes){
        return msgArgType();
    }
};


//for encode/decode 'bool' args in message
template <>
class SunnyTcsTansformPolicy<bool>
{
public:
    static QByteArray toBytes(bool arg)
    {
        return QByteArray::number(arg ? 1 : 0);
    }

    static bool fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(bytes.count() >= 1 && usedBytes);

        *usedBytes = 1;
        return  bytes.left(1).toShort() > 0;
    }
};

//for encode/decode 'char' args
template<>
class SunnyTcsTansformPolicy<char>
{
public:
    static QByteArray toBytes(char arg)
    {
        return QByteArray(1, arg);
    }

    static char fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(bytes.count() >= 1 && usedBytes);

        *usedBytes = 1;
        return  bytes[0];
    }
};

//encode/decode 'qint16' args
template <>
class SunnyTcsTansformPolicy<qint16>
{
public:
    static QByteArray toBytes(qint16 arg)
    {
        QByteArray bytes;
        bytes.append(static_cast<qint8>(arg >> 8));
        bytes.append(static_cast<qint8>(0xFF & arg));
        return bytes;
    }

    static qint16 fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(bytes.count() >= static_cast<qint32>(sizeof (qint16)) && usedBytes);

        *usedBytes = sizeof (qint16);
        return bytes.left(*usedBytes).toHex().toShort(nullptr, 16);
    }
};

//encode/decode 'qint32' args
template <>
class SunnyTcsTansformPolicy<qint32>
{
public:
    static QByteArray toBytes(qint32 arg)
    {
        QByteArray bytes;
        bytes.append(static_cast<qint8>(arg >> 24));
        bytes.append(static_cast<qint8>((arg >> 16) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 8) & 0xFF));
        bytes.append(static_cast<qint8>(arg & 0xFF));
        return bytes;
    }

    static qint32 fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(bytes.count() >= static_cast<qint32>(sizeof(qint32)) && usedBytes);

        *usedBytes = sizeof (qint32);
        return bytes.left(*usedBytes).toHex().toInt(nullptr, 16);
    }
};

//encode/decode 'qint64' args
template<>
class SunnyTcsTansformPolicy<qint64>
{
public:
    static QByteArray toBytes(qint64 arg)
    {
        QByteArray bytes;
        bytes.append(static_cast<qint8>(arg >> 56));
        bytes.append(static_cast<qint8>((arg >> 48) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 40) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 32) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 24) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 16) & 0xFF));
        bytes.append(static_cast<qint8>((arg >> 8) & 0xFF));
        bytes.append(static_cast<qint8>(arg & 0xFF));
        return bytes;
    }

    static qint64 fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(bytes.count() >= static_cast<qint32>(sizeof(qint64)) && usedBytes);

        *usedBytes = sizeof (qint64);
        return bytes.left(*usedBytes).toHex().toLong(nullptr, 16);
    }
};

//encode/decode 'QString' args
template <>
class SunnyTcsTansformPolicy<QString>
{
public:
    static QByteArray toBytes(QString arg)
    {
        QByteArray bytes;

        //explain the size of string arg
        QByteArray temp = arg.toLocal8Bit();
        bytes.append(static_cast<qint8>(temp.size() & 0xFF));
        bytes.append(temp);

        return bytes;
    }

    static QString fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(usedBytes && bytes.count() >= 1);

        *usedBytes = 0;
        bool szParsed = false;
        qint16 sz = bytes.mid(0, 1).toHex().toShort(&szParsed, 16);
        if(!szParsed || (sz > bytes.count())) return QString();

        *usedBytes = sz + 1;

        return QString::fromLocal8Bit(bytes.mid(1, sz));
    }
};

template <>
class SunnyTcsTansformPolicy<QByteArray>
{
public:
    static QByteArray toBytes(QByteArray arg)
    {
        QByteArray bytes;
        bytes.append(static_cast<qint8>((arg.count() >> 8) & 0xFF));
        bytes.append(static_cast<qint8>(arg.count() & 0xFF));
        bytes.append(arg);
        return bytes;
    }

    static QByteArray fromBytes(QByteArray bytes, qint32* usedBytes)
    {
        Q_ASSERT(usedBytes && bytes.count() >= 2);
        *usedBytes = 0;
        bool szParsed = false;
        qint16 sz = bytes.mid(0, 2).toHex().toShort(&szParsed, 16);
        if(!szParsed || sz > bytes.count()) return QByteArray();

        *usedBytes = sz + 2;

        return bytes.mid(2, sz);
    }
};
#endif // SUNNYTCSMSGARGPOLICY_H
