
CONFIG += c++11 console
LANGUAGE = C++
QT += network
DEFINES -= UNICODE
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += debug_and_release warn_on
CONFIG += warn_off -w4828

TRANSLATIONS += ./translator/VideoAnalysisClient_zh_CN.ts \
                ./translator/VideoAnalysisClient_EN.ts

win32{
	QMAKE_CXXFLAGS += /MP /utf-8
}

QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE   += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

CONFIG(debug,debug|release){
    TARGET = SunnyTcsApid
    DESTDIR_LIB = ../build/debug
    DESTDIR = ../debug/
    TEMP_DESTDIR = ../build/intermediate/debug/$$TARGET
}
CONFIG(release,debug|release){
    TARGET = SunnyTcsApi
    DESTDIR_LIB = ../build/release
    DESTDIR = ../release/
    TEMP_DESTDIR = ../build/intermediate/release/$$TARGET
}

MOC_DIR         = $$TEMP_DESTDIR/moc
OBJECTS_DIR     = $$TEMP_DESTDIR/obj

INCLUDEPATH  +=  \
		../third_party/Message/include \
		../third_party/ZeroMQ4.3/include

HEADERS +=  global.h \
            SunnyApiRouter.h \
			SunnyTcsApiMessageCenter.h \
			./model/SunnyTcsAgvModel.h \
			./model/SunnyTcsOrderModel.h \
			./model/SunnyTcsApiMapModel.h \
			./controller/SunnyTcsApiAgvController.h \
			./controller/SunnyTcsApiOrderController.h \
			./controller/SunnyTcsApiMapController.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpconnectionhandler.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpconnectionhandlerpool.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpglobal.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httplistener.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httprequest.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpresponse.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpsession.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpsessionstore.h \
			../third_party/QtWebApp/QtWebApp/httpserver/staticfilecontroller.h \
			../third_party/QtWebApp/QtWebApp/httpserver/httpcookie.h \
			../third_party/QtWebApp/QtWebApp/templateengine/template.h \

SOURCES += main.cpp \
           global.cpp \
		   SunnyApiRouter.cpp \
		   SunnyTcsApiMessageCenter.cpp \
		   ./model/SunnyTcsAgvModel.cpp \
		   ./model/SunnyTcsOrderModel.cpp \
		   ./model/SunnyTcsApiMapModel.cpp \
		   ./controller/SunnyTcsApiAgvController.cpp \
		   ./controller/SunnyTcsApiOrderController.cpp \
		   ./controller/SunnyTcsApiMapController.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpconnectionhandler.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpconnectionhandlerpool.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpglobal.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httplistener.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httprequest.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httprequesthandler.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpresponse.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpsession.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpsessionstore.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/staticfilecontroller.cpp \
		   ../third_party/QtWebApp/QtWebApp/httpserver/httpcookie.cpp
		   
include(../third_party/QtWebApp/QtWebApp/templateengine/templateengine.pri)
		   
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'third_party/ZeroMQ4.3/lib/' -llibzmq-v141-mt-4_3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'third_party/ZeroMQ4.3/lib/' -llibzmq-v141-mt-4_3_2

INCLUDEPATH += $$PWD/../'third_party/ZeroMQ4.3/include'
DEPENDPATH += $$PWD/../'third_party/ZeroMQ4.3/include'

LIBS += -L$$PWD/'third_party/Message/lib/' -lSunnyTcsMessage