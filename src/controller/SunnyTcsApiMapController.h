#ifndef  SUNNY_TCS_API_MAP_CONTROLLER_H
#define SUNNY_TCS_API_MAP_CONTROLLER_H

#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequest.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httpresponse.h"
#include "../../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.h"
#include "model/SunnyTcsApiMapModel.h"
#include <qobject.h>

using namespace stefanfrings;

class SunnyTcsApiMapController:public HttpRequestHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(SunnyTcsApiMapController)

private:
	SunnyTcsApiMapModel*  _model;

public:
	SunnyTcsApiMapController(SunnyTcsApiMapModel* model);
	~SunnyTcsApiMapController();

	void service(HttpRequest& request, HttpResponse& respond);
};

#endif // ! SUNNY_TCS_API_MAP_CONTROLLER_H

