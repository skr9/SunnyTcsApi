/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-23 22:21:43
 * @Descripttion: 
 */
/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include <QDir>
#include <QObject>
#include "global.h"
#include "../third_party/QtWebApp/QtWebApp/httpserver/httplistener.h"
#include "SunnyApiRouter.h"
#include "SunnyTcsApiMessageCenter.h"
#include "model/SunnyTcsAgvModel.h"
#include "model/SunnyTcsApiMapModel.h"
#include "model/SunnyTcsOrderModel.h"

using namespace stefanfrings;

/** Search the configuration file */
QString searchConfigFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QString fileName(appName+".ini");
    qInfo()<<fileName;

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/config");
    searchList.append(binDir+"/../config");
    searchList.append(binDir+"/../../config"); // for development without shadow build
    searchList.append(binDir+"/../"+appName+"/config"); // for development with shadow build
    searchList.append(binDir+"/../../"+appName+"/config"); // for development with shadow build
    searchList.append(binDir+"/../../../"+appName+"/config"); // for development with shadow build
    searchList.append(binDir+"/../../../../"+appName+"/config"); // for development with shadow build
    searchList.append(binDir+"/../../../../../"+appName+"/config"); // for development with shadow build
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            // found
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
}


/**
  Entry point of the program.
*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);

    app.setApplicationName("SunnyTcsApi");
    app.setOrganizationName("Butterfly");

    // Find the configuration file
    QString configFileName=searchConfigFile();

    qInfo()<<configFileName;

    // Configure logging into a file
    // QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    // logSettings->beginGroup("logging");
    // FileLogger* logger=new FileLogger(logSettings,10000,&app);
    // logger->installMsgHandler();

    // Configure template loader and cache
    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    templateSettings->beginGroup("templates");
    templateCache=new TemplateCache(templateSettings,&app);

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    sessionSettings->beginGroup("sessions");
    sessionStore=new HttpSessionStore(sessionSettings,&app);

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("docroot");
    staticFileController=new StaticFileController(fileSettings,&app);

    SunnyTcsAgvModel agvModel;
	SunnyTcsApiMapModel mapModel;
	SunnyTcsOrderModel orderModel;
    auto msgCenter = SunnyTcsApiMessageCenter::instance();
    QObject::connect(msgCenter, SIGNAL(modelSyncDataReady(const QByteArray&)), &agvModel, SLOT(syncDataReadySlot(const QByteArray&)));
	QObject::connect(msgCenter, SIGNAL(modelSyncDataReady(const QByteArray&)), &mapModel, SLOT(syncDataReadySlot(const QByteArray&)));
	QObject::connect(msgCenter, SIGNAL(modelSyncDataReady(const QByteArray&)), &orderModel, SLOT(syncDataReadySlot(const QByteArray&)));
    QObject::connect(msgCenter, &SunnyTcsApiMessageCenter::orderCreated, &orderModel, &SunnyTcsOrderModel::orderCreatedSlot);
    QObject::connect(msgCenter, &SunnyTcsApiMessageCenter::orderUpdated, &orderModel, &SunnyTcsOrderModel::orderUpdatedSlot);

    // Configure and start the TCP listener
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings,new SunnyApiRouter(&agvModel, &mapModel, &orderModel, &app),&app);

    msgCenter->connectTo("10.10.10.192", 3000);
    msgCenter->queryAllVehicles();

    app.exec();

    return 0;
}
