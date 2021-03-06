﻿#ifndef SUNNYTCSBROKER_GLOBAL_H
#define SUNNYTCSBROKER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SUNNYTCSMESSAGE_LIBRARY)
#  define SUNNYTCSMESSAGESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SUNNYTCSMESSAGESHARED_EXPORT Q_DECL_IMPORT
#endif

enum MESSAGE_TYPE
{
    MESSAGE_FOR_ERROR = 0x00, //just for report error when send or recv
    MESSAGE_READY = 0x01,
    MESSAGE_REGISTE = 0X02,
    MESSAGE_HEARTBEAT = 0x03,
    MESSAGE_DISCONNECT = 0x04,
    MESSAGE_UNICAST = 0x05,
    MESSAGE_BROADCAST = 0x06,
};

enum ARG_TYPE
{
    ARG_TYPE_CHAR = 0X01,
    ARG_TYPE_QINT16 = 0X02,
    ARG_TYPE_QINT32 = 0X03,
    ARG_TYPE_QINT64 = 0X04,
    ARG_TYPE_QSTRING = 0X05,
    ARG_TYPE_BOOL = 0X06,
    ARG_TYPE_MIXED = 0X07,
    ARG_TYPE_BYTEARRAY = 0X08,
    ARG_TYPE_UNDEFINED = 0X09,
};

//node id
#define MESSAGE_NODE_BROKER 0
#define MESSAGE_NODE_KERNEL 1
#define MESSAGE_NODE_OREDER_CENTER 2
#define MESSAGE_NODE_COMMUNICATION_CENTER 3
#define MESSAGE_NODE_DATA_CENTER 4
#define MESSAGE_NODE_SUPERVISORY 5
#define MESSAGE_NODE_MAPEDITOR 6
#define MESSAGE_NODE_REMOTE_CONTROLLER 7
#define MESSAGE_NODE_SIMULATOR 8
#define MESSAGE_NODE_CONFIG_TOOL 9

//listen port of broker
#define BROKER_LISTEN_PORT 3000

//MSG ID
#define MESSAGE_ID_ERROR 0X00
#define MESSAGE_ID_READY 0X01
#define MESSAGE_ID_REGISTE 0X02
#define MESSAGE_ID_HEARTBEAT 0X03
#define MESSAGE_ID_DISCONNECT 0X04
#define MESSAGE_ID_HANDSHARK 0x05
#define MESSAGE_ID_CONFIRM_BACK 0x06
#define MESSAGE_ID_INFO_QUERY 0x07
#define MINIMAL_UNICAST_MESSAGE_ID 10001
#define MINIMAL_BROADCAST_MESSAGE_ID 20001
#define MESSAGE_AGV_COMMANDS 21001
#define MESSAGE_FEEDBACK_TO_LINE 21002
#define MESSAGE_FEEDBACK_TO_MES 21003
#define MESSAGE_AGV_FEEDBACK 22001
#define MESSAGE_LINE_CONTROL 22002
#define MESSAGE_MES_COMMAND 22003
#define MESSAGE_LOG_INFO 23001

//session topics
#define LOG_SESSION "SunnyTCS_LSession"

//confirm result of ready msg
#define READY_CONFIRM_OK 0
#define READY_CONFIRM_FROM_ERROR 1
#define READY_CONFIRM_EXISTED 2



#endif // SUNNYTCSBROKER_GLOBAL_H
