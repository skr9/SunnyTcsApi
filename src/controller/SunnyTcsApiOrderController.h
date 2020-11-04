/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 21:17:10
 * @Descripttion: 
 */
#ifndef SUNNY_TCS_API_ORDER_CONTROLLER_H
#define SUNNY_TCS_API_ORDER_CONTROLLER_H

#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequest.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httpresponse.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.h"
#include "model/SunnyTcsOrderModel.h"
#include "SunnyTcsApiMessageCenter.h"

using namespace stefanfrings;

class SunnyTcsApiOrderController:public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(SunnyTcsApiOrderController)

private:
	SunnyTcsOrderModel* _model;

public:
    SunnyTcsApiOrderController(SunnyTcsOrderModel* model);

    void service(HttpRequest& request, HttpResponse& respond);
};



#endif