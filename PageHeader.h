#pragma once

#include <WebsiteFramework/WebsiteFramework.h>

#include "RequestData.h"
#include "Response.h"
#include "InputHandler.h"
#include "SubdatinData.h"
#include "UserData.h"
#include "UserPermissions.h"

void createPageHeader(FcgiData* fcgi, RequestData* data, int64_t subdatinId = -1);