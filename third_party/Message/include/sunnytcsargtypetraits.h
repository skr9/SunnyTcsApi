/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2018~2019 Sunny-baer Company Ltd.
 * Contract: http://www.sunny-baer.com
 *
 * Describle:
 * traits template of the arg type, currently contains the the traits
 * of 'size' and 'type id'
 *
 * Version:
 *
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef SUNNYTCSARGTYPETRAITS_H
#define SUNNYTCSARGTYPETRAITS_H

#include <QObject>
#include "sunnytcsmessage_global.h"

template<class T>
class SunnyTcsArgTypeTraits
{
public:
    static qint32 size(T arg){return sizeof(arg);}
    static ARG_TYPE typeID(){return ARG_TYPE_UNDEFINED;}
};

template<>
class SunnyTcsArgTypeTraits<bool>
{
public:
    static qint32 size(bool arg){return 1;}
    static ARG_TYPE typeID(){return ARG_TYPE_BOOL;}
};

template<>
class SunnyTcsArgTypeTraits<char>
{
public:
    static qint32 size(char arg){return sizeof (arg);}
    static ARG_TYPE typeID(){return ARG_TYPE_CHAR;}
};

template<>
class SunnyTcsArgTypeTraits<qint16>
{
public:
    static qint32 size(qint16 arg){return sizeof (arg);}
    static ARG_TYPE typeID(){return ARG_TYPE_QINT16;}
};

template<>
class SunnyTcsArgTypeTraits<qint32>
{
public:
    static qint32 size(qint32 arg){return sizeof (arg);}
    static ARG_TYPE typeID(){return ARG_TYPE_QINT32;}
};

template <>
class SunnyTcsArgTypeTraits<qint64>
{
public:
    static qint32 size(qint64 arg){return sizeof (arg);}
    static ARG_TYPE typeID(){return ARG_TYPE_QINT64;}
};


template <>
class SunnyTcsArgTypeTraits<QString>
{
public:
    static qint32 size(QString arg){
        return arg.toLocal8Bit().size() + 1; //1 is the bytes of header who tell reciver how many chars in this arg arg
    }

    static ARG_TYPE typeID(){return ARG_TYPE_QSTRING;}
};

template <>
class SunnyTcsArgTypeTraits<QByteArray>
{
public:
    static qint32 size(QByteArray arg){return arg.count() + 2;}

    static ARG_TYPE typeID(){return ARG_TYPE_BYTEARRAY;}
};

#endif // SUNNYTCSARGTYPETRAITS_H
