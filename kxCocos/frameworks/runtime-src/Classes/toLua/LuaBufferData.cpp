#include "LuaBufferData.h"
#include "LuaTools.h"
#include "BufferData.h"

int newBufferData(lua_State* l)
{
    CBufferData* buffer = new CBufferData();
    buffer->init(256);
	LuaTools::pushClass(l, buffer, "KxCocos.BufferData");
    return 1;
}

int deleteBufferData(lua_State* l)
{
    // 之所以不用luaL_checkudata(l, -2, "KxCocos.BufferData")
    // 是因为lightuserdata并不支持(应该是这个版本lua的bug)
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    if (NULL != buffer)
    {
        delete buffer;
    }
    return 0;
}

int writeIntToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    int data = luaL_checkint(l, -1);
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeCharToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    char data = lua_tointeger(l, -1);
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeUCharFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    unsigned char data = lua_tointeger(l, -1);
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeShortFromBufferData(lua_State* l)
{
	CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
	short data = lua_tointeger(l, -1);
	if (NULL != buffer)
	{
		buffer->writeData(data);
	}
	return 0;
}

int writeUShortFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    unsigned short data = lua_tointeger(l, -1);
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeFloatToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    float data = luaL_checknumber(l, -1);
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeBoolToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    bool data = lua_toboolean(l, -1) == 1;
    if (NULL != buffer)
    {
        buffer->writeData(data);
    }
    return 0;
}

int writeStringToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
	
	size_t len = 0;
    const char* data = luaL_checklstring(l, 2, &len);
    if (NULL != buffer)
    {
        buffer->writeData(data, len);
    }
    return 0;
}

int writeBufferToBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    size_t len;
    const char* data = luaL_checklstring(l, 1, &len);
    if (NULL != buffer)
    {
        buffer->writeData(data, len);
    }
    return 0;
}

int readIntFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    int data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushinteger(l, data);
        return 1;
    }
    return 0;
}

int readCharFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    char data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushinteger(l, data);
        return 1;
    }
    return 0;
}

int readUCharFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    unsigned char data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushinteger(l, data);
        return 1;
    }
    return 0;
}

int readShortFromBufferData(lua_State* l)
{
	CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
	short data;
	if (NULL != buffer && buffer->readData(data))
	{
		lua_pushinteger(l, data);
		return 1;
	}
	return 0;
}

int readUShortFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    unsigned short data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushinteger(l, data);
        return 1;
    }
    return 0;
}

int readFloatFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    float data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushnumber(l, data);
        return 1;
    }
    return 0;
}

int readBoolFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    bool data;
    if (NULL != buffer && buffer->readData(data))
    {
        lua_pushboolean(l, data);
        return 1;
    }
    return 0;
}

int readStringFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
    if (NULL != buffer && buffer->getDataLength() > buffer->getOffset())
    {
        // 将当前内容视为字符串,返回剩下的内容
        char* data = buffer->getBuffer() + buffer->getOffset();
        unsigned int len = buffer->getDataLength() - buffer->getOffset();
        if (len > 0)
        {
            buffer->updateOffset(buffer->getOffset() + len);
            lua_pushlstring(l, data, len);
            return 1;
        }
    }
    return 0;
}

int readCharArrayFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    unsigned int len = luaL_checkint(l, -1);
    if (NULL != buffer && buffer->getDataLength() > buffer->getOffset())
    {
        char* data = buffer->getBuffer() + buffer->getOffset();
        if (len > 0 && len <= buffer->getDataLength() - buffer->getOffset())
        {
            buffer->updateOffset(buffer->getOffset() + len);
            lua_pushstring(l, data);
            return 1;
        }
    }
    return 0;
}

int readBufferFromBufferData(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    unsigned int bufferLen = luaL_checkint(l, -1);
    if (buffer != NULL && bufferLen < buffer->getDataLength() - buffer->getOffset())
    {
        char* data = new char[bufferLen];
        buffer->readData(data, bufferLen);
        lua_pushlstring(l, data, bufferLen);
        delete data;
        return 1;
    }
    return 0;
}

int resetOffsetBufferData(lua_State* l)
{
	CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -1, "KxCocos.BufferData");
	if (NULL != buffer)
	{
		buffer->resetOffset();
	}
	return 0;
}

int writeCharArray(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -3, "KxCocos.BufferData");
    std::string str = luaL_checkstring(l, -2);
    unsigned int strLength = luaL_checkint(l, -1);
    if (NULL != buffer)
    {
        if (str.length() < strLength)
        {
            buffer->writeData(str.c_str(), str.length());
			strLength -= str.length();
			for (unsigned int i = 0; i < strLength; ++i)
			{
				buffer->writeData<char>('\0');
			}
        }
        else
        {
            buffer->writeData(str.c_str(), strLength-1);
			buffer->writeData<char>('\0');
        }
    }
    return 0;
}

int setIsRead(lua_State* l)
{
    CBufferData* buffer = LuaTools::checkClass<CBufferData>(l, -2, "KxCocos.BufferData");
    bool isRead = lua_toboolean(l, -1) == 1;
    if (NULL != buffer)
    {
        buffer->setIsReadModel(isRead);
    }
    return 0;
}

// BufferData的Lua方法
static const struct luaL_reg bufferData_m[] = 
{
    { "readInt", readIntFromBufferData },
    { "writeInt", writeIntToBufferData },
    { "readBool", readBoolFromBufferData },
    { "writeBool", writeBoolToBufferData },
    { "readFloat", readFloatFromBufferData },
    { "writeFloat", writeFloatToBufferData },
    { "readChar", readCharFromBufferData },
    { "readUChar", readUCharFromBufferData },
	{ "readShort", readShortFromBufferData },
    { "readUShort", readUShortFromBufferData },
    { "writeChar", writeCharToBufferData },
    { "writeUChar", writeUCharFromBufferData },
	{ "writeShort", writeShortFromBufferData },
    { "writeUShort", writeUShortFromBufferData },
    { "readBuffer", readBufferFromBufferData },
    { "writeBuffer", writeBufferToBufferData },
    { "readString", readStringFromBufferData },
    { "readCharArray", readCharArrayFromBufferData },
	{ "writeString", writeStringToBufferData },
	{ "resetOffset", resetOffsetBufferData },
    { "writeCharArray", writeCharArray },
    { "setIsRead", setIsRead },
    { NULL, NULL }
};

bool regiestBufferData()
{
    auto luaStack = cocos2d::LuaEngine::getInstance()->getLuaStack();
    auto luaState = luaStack->getLuaState();

    // 注册BufferData的Lua类
    luaL_newmetatable(luaState, "KxCocos.BufferData");
    lua_pushstring(luaState, "__index");
    // pushes the metatable
    lua_pushvalue(luaState, -2);
    // metatable.__index = metatable 
    lua_settable(luaState, -3);
    luaL_openlib(luaState, NULL, bufferData_m, 0);

    lua_register(luaState, "newBufferData", newBufferData);
    lua_register(luaState, "deleteBufferData", deleteBufferData);
    return true;
}