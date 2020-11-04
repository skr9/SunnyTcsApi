/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-25 08:49:22
 * @Descripttion: 
 */

#include "SunnyApiRouter.h"
#include "controller/SunnyTcsApiAgvController.h"
#include "controller/SunnyTcsApiOrderController.h"
#include "controller/SunnyTcsApiMapController.h"
#include <QDebug>

SunnyApiRouter::SunnyApiRouter(SunnyTcsAgvModel* agvModel, SunnyTcsApiMapModel* mapModel, 
SunnyTcsOrderModel* orderModel, QObject* parent): HttpRequestHandler(parent), 
_agvModel(agvModel), _mapModel(mapModel), _orderModel(orderModel)
{
    qDebug()<<"SunnyApiRouter created";
}

SunnyApiRouter::~SunnyApiRouter()
{
    qDebug()<<"SunnyApiRouter deleted";
}

void SunnyApiRouter::service(HttpRequest& request, HttpResponse& responed)
{
    QString path =  request.getPath();
    qDebug()<<"api router:path="<<path.data();

    if(path.startsWith("/sunnyapi/V1/order"))
    {
		SunnyTcsApiOrderController(_orderModel).service(request, responed);
    }
    else if(path.startsWith("/sunnyapi/V1/agv"))
    {
        SunnyTcsApiAgvController(_agvModel).service(request, responed);
    }
    else if(path.startsWith("/sunnyapi/V1/map"))
    {
		SunnyTcsApiMapController(_mapModel).service(request, responed);
    }
}