#pragma once

// SETTINGS
#ifdef CLEANCHEAT_USER_CONFIG
#include CLEANCHEAT_USER_CONFIG
#else
#include "Config.h"
#endif

// KEYWORDS
#define ABSTRACT

// MACROS
#define DELETE_HEAP(ptr) delete ptr; ptr = nullptr

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#ifndef CLEANCHEAT_LOG
#define CLEANCHEAT_LOG(format, ...)
#endif // !CLEANCHEAT_LOG
