#pragma once

#include <fcgio.h>

#include "Config.h"

enum StatusCode{Ok = 200, SeeOther = 303, Unauthorized = 401, NotFound = 404, MethodNotAllowed = 405, UnprocessableEntity = 422};

void sendStatusHeader(std::ostream& fcgiOu, StatusCode code);
void sendCookieHeader(std::ostream& fcgiOut, std::string cookieName, std::string cookieValue);
void sendDeleteCookieHeader(std::ostream& fcgiOut, std::string cookieName);
void sendLocationHeader(std::ostream& fcgiOut, std::string location);
void sendHtmlContentTypeHeader(std::ostream& fcgiOut);
void finishHttpHeader(std::ostream& fcgiOut);
