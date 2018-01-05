#include "LogConsoleHandler.h"
#include "cocos2d.h"

bool KxLogConsoleHandler::onLog(int level, const std::string& log)
{
	cocos2d::log("%s", log.c_str());
    return true;
}
