#include "LuaBase.h"
#include "GameNetworkNode.h"
#include "LuaTools.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "../cocos/network/HttpClient.h"
#include "Protocol.h"

USING_NS_CC;
using namespace std;
using namespace network;
using namespace KxCocos;

////////////////////////////////网络相关//////////////////////////////////////////
int requestHttp(lua_State* l)
{
    const char* url = luaL_checkstring(l, -2);
    const char* post = luaL_checkstring(l, -1);
    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(url);
    request->setRequestType(HttpRequest::Type::POST);
    request->setRequestData(post, strlen(post));
    HttpClient::getInstance()->sendImmediate(request);
    request->release();
    return 0;
}

int requestHttpWithCallback(lua_State* l)
{
    const char* url = tolua_tostring(l, 1, "");
    const char* post = tolua_tostring(l, 2, "");
    LUA_FUNCTION handler = (toluafix_ref_function(l, 3, 0));

    ccHttpRequestCallback callback = [=](HttpClient* client, HttpResponse* respone)
    {
        LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
        stack->clean();
        stack->pushInt(respone->getResponseCode());
        stack->pushString(respone->getResponseData()->data());
        stack->executeFunctionByHandler(handler, 2);
        stack->clean();
    };

    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(url);
    request->setRequestType(HttpRequest::Type::GET);
    request->setRequestData(post, strlen(post));
    request->setResponseCallback(callback);
    HttpClient::getInstance()->sendImmediate(request);
    request->release();
    return 0;
}

int getSubCmd(lua_State* l)
{
	int cmd = luaL_checkint(l, 1);
	lua_pushinteger(l, cmd & 0x0000ffff);
	return 1;
}

int getMainCmd(lua_State* l)
{
	int cmd = luaL_checkint(l, 1);
	lua_pushinteger(l, cmd >> 16);
	return 1;
}

int makeCmd(lua_State* l)
{
	int mainCmd = luaL_checkint(l, -2);
	int subCmd = luaL_checkint(l, -1);
	lua_pushinteger(l, MakeCommand(mainCmd, subCmd));
	return 1;
}

int request(lua_State* l)
{
	int nServerID = 0;
	// 要发送给服务端的数据
	CBufferData* buffer = nullptr;
	int argc = lua_gettop(l);
	if (argc == 1)
	{
		buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
	}
	else if (argc == 2)
	{
		nServerID = luaL_checkint(l, -1);
		buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
	}
	else
	{
		KXLOGDEBUG("request param count %d error! expect one or two", argc);
		return 0;
	}

	if (NULL != buffer && buffer->getDataLength() >= sizeof(Head))
	{
		Head* head = reinterpret_cast<Head*>(buffer->getBuffer());
		// lua中不再计算包头的长度字段
		head->length = buffer->getDataLength();

		int mainCmd = head->MainCommand();
		int subCmd = head->SubCommand();

		int ret = CGameNetworkNode::getInstance()->sendData(buffer->getBuffer(), buffer->getDataLength(), nServerID);
		KXLOGDEBUG("send cmd %d %d, len %d, send ret %d", mainCmd, subCmd, head->length, ret);
	}
	return 0;
}

int openURL(lua_State *l)
{
	auto url = luaL_checkstring(l, -1);

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 
	auto result = ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	if ((int)result <= 32) { // 打开默认浏览器失败，尝试打开IE浏览器
		ShellExecuteA(NULL, "open", "iexplore.exe", url, NULL, SW_SHOWNORMAL);
	}
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	auto packageName = JniHelper::callStaticStringMethod("org/cocos2dx/lib/Cocos2dxHelper", "getCocos2dxPackageName");
	JniHelper::callStaticVoidMethod(packageName + "/AppActivity", "openURL", url);
#endif

	return 0;
}

int LogPrint(lua_State * L)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	string out;

	lua_getglobal(L, "tostring");
	for (i = 1; i <= n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		size_t sz;
		s = lua_tolstring(L, -1, &sz);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "
				LUA_QL("print"));
		if (i > 1) out.append("\t");
		out.append(s, sz);
		lua_pop(L, 1);  /* pop result */
	}
	KXLOGDEBUG("[LUA-print] %s", out.c_str());

	return 0;
}

int connectToServer(lua_State* l)
{
	const char* ip = luaL_checkstring(l, 1);
	int port = luaL_checkint(l, 2);
	int serverID = luaL_checkint(l, 3);
	bool ipv6 = lua_toboolean(l, 4) == 1;

	CCLOG("connect to server ip = %s, port = %d sockversion = %s", ip, port, !ipv6 ? "ipv4" : "ipv6");
	// 传入ip
	CGameNetworkNode::getInstance()->connect(ip, port, serverID, ipv6, [](bool success) {
		CCLOG("connect result:%s", success ? "success" : "failed");
		onLuaStrEventWithParamInt("NetConnect", success);
	});
	return 0;
}

int reconnectToServer(lua_State* l)
{
	int serverID = luaL_checkint(l, 1);

	CCLOG("reconnect to server id = %d", serverID);
	// 传入ip
	CGameNetworkNode::getInstance()->reconnect(serverID, [](bool success) {
		CCLOG("reconnect result:%s", success ? "success" : "failed");
		onLuaStrEventWithParamInt("NetReconnect", success);
	});
	return 0;
}

int logout(lua_State* l)
{
	// 断开连接
	CGameNetworkNode::getInstance()->closeAll();
	return 0;
}

bool regiestBase()
{
    auto luaState = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    lua_register(luaState, "requestHttpWithCallback", requestHttpWithCallback);
    lua_register(luaState, "requestHttp", requestHttp);
	lua_register(luaState, "request", request);
	lua_register(luaState, "makeCommand", makeCmd);
	lua_register(luaState, "getMainCmd", getMainCmd);
	lua_register(luaState, "getSubCmd", getSubCmd);
    lua_register(luaState, "print", LogPrint);
	lua_register(luaState, "openURL", openURL);
	lua_register(luaState, "connectToServer", connectToServer);
	lua_register(luaState, "reconnectToServer", reconnectToServer);
	lua_register(luaState, "logout", logout);
	
    return true;
}

void onLuaRespone(int id, CBufferData* buffer)
{
    if (NULL == buffer)
    {
        return;
    }

    // 回调Lua入口
    auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    LuaEngine::getInstance()->getLuaStack()->clean();
    // 获取NetHelper.onResponse方法
    lua_getglobal(l, "NetHelper");
	if (lua_isnoneornil(l, -1))
	{
		lua_pop(l, 1);
		return;
	}

    lua_getfield(l, 1, "onResponse");
	lua_pushinteger(l, id);
    // 传入BufferData作为参数
    LuaTools::pushClass(l, buffer, "KxCocos.BufferData");

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(2);
    // 调用Lua的NetHelper.onResponse方法
    //int ret = lua_pcall(l, 1, 0, 0);
	//if (ret)
	//{
	//		auto err = lua_tostring(l, -1);
	//		KXLOGDEBUG("----------------------------------------");
	//		KXLOGDEBUG("LUA ERROR: %s", err);
	//		KXLOGDEBUG("----------------------------------------");
	//}
    LuaEngine::getInstance()->getLuaStack()->clean();
}

void onLuaErrorEvent()
{
	auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	LuaEngine::getInstance()->getLuaStack()->clean();
	// 获取NetHelper.onErrorEvent方法
	lua_getglobal(l, "NetHelper");
	if (lua_isnoneornil(l, -1))
	{
		lua_pop(l, 1);
		return;
	}

	lua_getfield(l, 1, "onErrorEvent");

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(0);
	LuaEngine::getInstance()->getLuaStack()->clean();
}

void debugLuaStack()
{
    auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    LuaEngine::getInstance()->getLuaStack()->clean();
    lua_getglobal(l, "printTrace");
    if (lua_isnoneornil(l, -1))
    {
        lua_pop(l, 1);
        return;
    }

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(0);
    //lua_pcall(l, 0, 0, 0);
    LuaEngine::getInstance()->getLuaStack()->clean();
}

void onLuaEvent(int eventId)
{
    auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    LuaEngine::getInstance()->getLuaStack()->clean();
    // 获取EventManager:raiseEvent方法
    lua_getglobal(l, "EventManager");
    if (lua_isnoneornil(l, -1))
    {
        lua_pop(l, 1);
        return;
    }

    lua_getfield(l, 1, "raiseEvent");
    lua_getglobal(l, "EventManager");
    lua_pushinteger(l, eventId);

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(2);
    //lua_pcall(l, 2, 0, 0);
    LuaEngine::getInstance()->getLuaStack()->clean();
}

void onLuaEventWithParamInt(int eventId, int param)
{
    auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    LuaEngine::getInstance()->getLuaStack()->clean();
    // 获取EventManager:raiseEvent方法
    lua_getglobal(l, "EventManager");
    if (lua_isnoneornil(l, -1))
    {
        lua_pop(l, 1);
        return;
    }

    lua_getfield(l, 1, "raiseEvent");
    lua_getglobal(l, "EventManager");
    lua_pushinteger(l, eventId);
    lua_pushinteger(l, param);

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(3);
    //lua_pcall(l, 3, 0, 0);
    LuaEngine::getInstance()->getLuaStack()->clean();
}

void onLuaEventWithParamStr(int eventId, std::string param)
{
	auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	LuaEngine::getInstance()->getLuaStack()->clean();
	// 获取EventManager:raiseEvent方法
	lua_getglobal(l, "EventManager");
	if (lua_isnoneornil(l, -1))
	{
		lua_pop(l, 1);
		return;
	}

	lua_getfield(l, 1, "raiseEvent");
	lua_getglobal(l, "EventManager");
	lua_pushinteger(l, eventId);
	lua_pushstring(l, param.c_str());

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(3);
	//lua_pcall(l, 3, 0, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
}

void onLuaStrEventWithParamInt(std::string eventname, int param)
{
	auto l = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	LuaEngine::getInstance()->getLuaStack()->clean();
	// 获取EventManager:raiseEvent方法
	lua_getglobal(l, "EventManager");
	if (lua_isnoneornil(l, -1))
	{
		lua_pop(l, 1);
		return;
	}

	lua_getfield(l, 1, "raiseEvent");
	lua_getglobal(l, "EventManager");
	lua_pushstring(l, eventname.c_str());
	lua_pushinteger(l, param);

	//参数为传入的参数个数
	LuaEngine::getInstance()->getLuaStack()->executeFunction(3);
	//lua_pcall(l, 3, 0, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
}
