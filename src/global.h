/*
 * @version: V1.0
 * @Author: xxx
 * @Date: 2020-08-23 22:44:31
 * @Descripttion: 
 */
/**
  @file
  @author Stefan Frings
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include "../third_party/QtWebApp/QtWebApp/templateengine/templatecache.h"
#include "../third_party/QtWebApp/QtWebApp/httpserver/httpsessionstore.h"
#include "../third_party/QtWebApp/QtWebApp/httpserver/staticfilecontroller.h"
#include "../third_party/QtWebApp/QtWebApp/logging/filelogger.h"

using namespace stefanfrings;

/**
  Global objects that are shared by multiple source files
  of this project.
*/

/** Cache for template files */
extern TemplateCache* templateCache;

/** Storage for session cookies */
extern  HttpSessionStore* sessionStore;

/** Controller for static files */
extern  StaticFileController* staticFileController;

/** Redirects log messages to a file */
extern  FileLogger* logger;

#endif // GLOBAL_H
