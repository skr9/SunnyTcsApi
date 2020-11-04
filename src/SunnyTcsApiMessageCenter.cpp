#include "SunnyTcsApiMessageCenter.h"
#include <qdebug.h>

#define API_MESSAGE_NODE_ID 1002
#define SUNNYTCS_DATA_SESSION "SunnyTCS_DSession"
#define SUNNYTCS_KERNEL_STATE_SESSION "SunnyTCS_SSession"
#define MESSAGE_ID_SYNC_START 15001
#define MESSAGE_ID_SYNC_DATA 15002
#define MESSAGE_ID_ORD_UPDATED 11002
#define MESSAGE_ID_ORD_CREATED  11008
#define MESSAGE_ID_AGV_FEEDBACK 22001
#define MESSAGE_ID_LINE_FEEDBACK 22002
#define MESSAGE_ID_AGV_COMMAND 21001
#define MESSAGE_ID_DATA_TO_LINE 21002

SunnyTcsApiMessageCenter* SunnyTcsApiMessageCenter::_instance = nullptr;

SunnyTcsApiMessageCenter::SunnyTcsApiMessageCenter():_errorString("OK"), _isCancelRequest(false){
    _node.reset(new SunnyTcsZmqNode(API_MESSAGE_NODE_ID));
}

SunnyTcsApiMessageCenter::~SunnyTcsApiMessageCenter(){}

SunnyTcsApiMessageCenter* SunnyTcsApiMessageCenter::instance()
{
    if(nullptr == _instance)
    {
        SunnyTcsApiMessageCenter* temp = new SunnyTcsApiMessageCenter();
        if(nullptr == _instance)
            _instance = temp;
    }

    return _instance;
}

bool SunnyTcsApiMessageCenter::connectTo(QString address, qint32 port)
{
    if(!_node->connectTo(address, port))
    {
        _errorString = QString("Failed connect to %1 : %2").arg(address).arg(port);
        return false;
    }

    _node->sendReadyMsg();
    _node->registeSession(SUNNYTCS_DATA_SESSION);
    _node->registeSession(SUNNYTCS_KERNEL_STATE_SESSION);

    this->start();

    return true;
}

//发送查询消息，读取调度系统中现有的所有AGV信息
void SunnyTcsApiMessageCenter::queryAllVehicles()
{
    SunnyTcsMessage<char> msg(MESSAGE_UNICAST, MESSAGE_ID_SYNC_START, MESSAGE_NODE_KERNEL);
    msg.addArg('r');
    msg.setFrom(API_MESSAGE_NODE_ID);
    _node->send(msg);
}

void SunnyTcsApiMessageCenter::sendStrMessage(SunnyTcsMessage<QString>& msg)
{
    msg.setFrom(API_MESSAGE_NODE_ID);
    _node->send(msg);
}

void SunnyTcsApiMessageCenter::sendIntMessage(SunnyTcsMessage<qint32>& msg)
{
    msg.setFrom(API_MESSAGE_NODE_ID);
    _node->send(msg);
}

void SunnyTcsApiMessageCenter::sendMixMessage(SunnyTcsMixedMessage& msg)
{
    msg.setFrom(API_MESSAGE_NODE_ID);
    _node->sendMixedMsg(msg);
}

void SunnyTcsApiMessageCenter::close()
{
    if(isRunning())
    {
        _isCancelRequest = true;
        quit();
        wait();
    }
}

void SunnyTcsApiMessageCenter::run()
{
    while (!_isCancelRequest)
    {
        QByteArray bytes = _node->recv();
        if(bytes.count() == 0) continue;

       // qInfo() << "recv msg:" << bytes;

        if(SunnyTcsMessageParser::parseMsgID(bytes) == 4)
        {
            emit disconnected();
            _node->reconnect();
            _node->sendHeartBeatSignal(1000);
        }
        else
        {
            msgRoute(bytes);
        }

        if(QDateTime::currentMSecsSinceEpoch() - _node->lastSendTime() > 1000)
            _node->sendHeartBeatSignal(1000 * 2);

        msleep(10);
    }    
}

void SunnyTcsApiMessageCenter::msgRoute(const QByteArray& bytes)
{
    auto mid = SunnyTcsMessageParser::parseMsgID(bytes);
   // qInfo() << "mid:" << mid;
    switch (mid)
    {
        case MESSAGE_ID_SYNC_DATA: 
            emit modelSyncDataReady(SunnyTcsMessage<QByteArray>(bytes).args().at(0)); break;
        case MESSAGE_ID_AGV_FEEDBACK:
            emit agvFeedbackDataReady(SunnyTcsMessage<QByteArray>(bytes).args()); break;
        case MESSAGE_ID_LINE_FEEDBACK:
            emit lineFeedbackDataReady(SunnyTcsMessage<QByteArray>(bytes).args().at(0)); break;
        case MESSAGE_ID_ORD_CREATED:
            emit orderCreated(SunnyTcsMessage<QByteArray>(bytes).args().at(0)); break;
        case MESSAGE_ID_ORD_UPDATED:
        {
            SunnyTcsMixedMessage msg(bytes);
            QString ord = msg.args()->at(0).toString();
            qint8 state = -1;
            QString reason = "";
            qint32 cnt = msg.args()->count();
            if (cnt > 1) {
                state = msg.args()->at(1).toChar() - '0';
            }

            if (cnt > 2) {
                reason = msg.args()->at(2).toString();
            }
            
            emit orderUpdated(ord, state, reason);
        }; break;
        default:break;
    }
}