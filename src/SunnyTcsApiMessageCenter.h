
#ifndef SUNNY_TCS_API_MESSAGE_CENTER_H
#define SUNNY_TCS_API_MESSAGE_CENTER_H

#include "../third_party/Message/include/sunnytcsmessage.hpp"
#include "../third_party/Message/include/sunnytcszmqnode.hpp"
#include <QThread>
#include <memory>
using namespace std;

class SunnyTcsApiMessageCenter: public QThread
{
    Q_OBJECT
private:
    static SunnyTcsApiMessageCenter* _instance;
    unique_ptr<SunnyTcsZmqNode> _node;
    QString _errorString;
    bool _isCancelRequest;

    SunnyTcsApiMessageCenter(/* args */);
    ~SunnyTcsApiMessageCenter();

    void msgRoute(const QByteArray&);

protected:
    void run() override;

signals:
    void disconnected();
    void modelSyncDataReady(const QByteArray& bytes);
    void agvFeedbackDataReady(const QList<QByteArray>& bytes);
    void lineFeedbackDataReady(const QByteArray& bytes);
    void orderUpdated(const QString& ordID, qint8 state, const QString& reason);
    void orderCreated(const QByteArray& bytes);


public:
    static SunnyTcsApiMessageCenter* instance();

    bool connectTo(QString address, qint32 port);
    void queryAllVehicles();
    void sendStrMessage(SunnyTcsMessage<QString>& msg);
    void sendIntMessage(SunnyTcsMessage<qint32>& msg);
    void sendMixMessage(SunnyTcsMixedMessage& msg);
    void close();
};


#endif