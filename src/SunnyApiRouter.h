/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 08:49:11
 * @Descripttion: 
 */
#include "../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.h"
#include "model/SunnyTcsAgvModel.h"
#include "model/SunnyTcsApiMapModel.h"
#include "model/SunnyTcsOrderModel.h"
#include "SunnyTcsApiMessageCenter.h"
using namespace stefanfrings;

class SunnyApiRouter:public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(SunnyApiRouter)

private:
    SunnyTcsAgvModel* _agvModel;
	SunnyTcsApiMapModel* _mapModel;
	SunnyTcsOrderModel* _orderModel;

public:
    SunnyApiRouter(SunnyTcsAgvModel* agvModel, SunnyTcsApiMapModel* mapModel, 
		                       SunnyTcsOrderModel* orderModel, QObject* parent = 0);
    ~SunnyApiRouter();

    void service(HttpRequest& request, HttpResponse& response);
};


