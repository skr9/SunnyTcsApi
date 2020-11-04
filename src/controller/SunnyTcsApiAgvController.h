/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 21:17:44
 * @Descripttion: 
 */
#ifndef  SUNNY_TCS_API_AGV_CONTROLLER_H
#define SUNNY_TCS_API_AGV_CONTROLLER_H

#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequest.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httpresponse.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.h"
#include "../SunnyTcsApiMessageCenter.h"
#include "../model/SunnyTcsAgvModel.h"

using namespace stefanfrings;

class SunnyTcsApiAgvController:public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(SunnyTcsApiAgvController)

private:
    SunnyTcsAgvModel* _model;

public:
    SunnyTcsApiAgvController(SunnyTcsAgvModel* model);

    void service(HttpRequest& request, HttpResponse& respond);
};


#endif