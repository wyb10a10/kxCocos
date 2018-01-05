#ifndef _KXCOCOS_SUMMONER_BASE_
#define _KXCOCOS_SUMMONER_BASE_

#include "LuaTools.h"
#include "BufferData.h"

bool regiestBase();

void onLuaRespone(int id, CBufferData* buffer);
void onLuaErrorEvent();
void debugLuaStack();

void onLuaEvent(int eventId);
void onLuaEventWithParamInt(int eventId, int param);
void onLuaEventWithParamStr(int eventId, std::string param);

void onLuaStrEventWithParamInt(std::string eventname, int param = 0);

#endif