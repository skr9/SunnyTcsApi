#include "SunnyTcsApiMapController.h"
#include "./model/SunnyTcsApiMapModel.h"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>

SunnyTcsApiMapController::SunnyTcsApiMapController(SunnyTcsApiMapModel* model): _model(model)
{

}

SunnyTcsApiMapController::~SunnyTcsApiMapController()
{
}

void SunnyTcsApiMapController::service(HttpRequest& request, HttpResponse& reponse)
{
	QString path = request.getPath();
	QString prefix = "/sunnyapi/V1/map";
	if (!path.startsWith(prefix)) return;
	path.remove(prefix);

	reponse.setHeader("Content-Type", "text/json;charset=utf-8");
	QByteArray body;
	QString method = request.getMethod();
	if (method == "GET")
	{
		if (path.startsWith("/points"))
		{
			QJsonObject jo;
			QJsonArray jarr;
			auto points = _model->points();
			for (MapPoint pt : points)
			{
				QJsonObject temp;
				temp["Name"] = pt.name();
				temp["ID"] = pt.id();
				temp["Pos"] = pt.pos();
				jarr.append(temp);
			}
			jo["Points"] = jarr;
			QJsonDocument jd(jo);
			body.append(jd.toJson());
		}
		else
		{
			/* code */
		}
	}

	reponse.write(body, true);
}
