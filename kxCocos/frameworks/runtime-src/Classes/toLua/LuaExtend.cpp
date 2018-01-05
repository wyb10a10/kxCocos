#include "LuaExtend.h"
#include "LuaTools.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include <spine/spine-cocos2dx.h>
#include "effect/SimpleShader.h"

using namespace spine;

static int lua_Spine_extend_spine_getBonePos(lua_State* tolua_S)
{
	if (nullptr == tolua_S)
		return 0;

	int argc = 0;
	spine::SkeletonAnimation* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_Spine_extend_spine_getBonePos'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 1)
	{
		//const char* arg;

		std::string arg_tmp;
		ok &= luaval_to_std_string(tolua_S, 2, &arg_tmp, "sp.SkeletonAnimation:getBonePos");
		//arg = arg_tmp.c_str();

		if (!ok)
			return 0;
		auto mbone = cobj->findBone(arg_tmp);
		if (!mbone)
		{
			CCLOG("not bone in sp.SkeletonAnimation by boneName:");
			//tolua_error(tolua_S, "not bone in sp.SkeletonAnimation by boneName:");
			return 0;
		}
		//x(0), y(0), rotation(0), scaleX(0), scaleY(0),
		const cocos2d::Vec2& pos = Vec2(mbone->x, mbone->y);
		vec2_to_luaval(tolua_S, pos);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getBonePos", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_Spine_extend_spine_getBonePos'.", &tolua_err);
#endif

	return 0;
}

static int lua_Spine_extend_spine_getBoneRotation(lua_State* tolua_S)
{
	if (nullptr == tolua_S)
		return 0;

	int argc = 0;
	spine::SkeletonAnimation* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_Spine_extend_spine_getBoneRotation'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 1)
	{
		//const char* arg;

		std::string arg_tmp;
		ok &= luaval_to_std_string(tolua_S, 2, &arg_tmp, "sp.SkeletonAnimation:getBoneRotation");
		//arg = arg_tmp.c_str();

		if (!ok)
			return 0;
		auto mbone = cobj->findBone(arg_tmp);
		if (!mbone)
		{
			CCLOG("not bone in sp.SkeletonAnimation by boneName:");
			//tolua_error(tolua_S, "not bone in sp.SkeletonAnimation by boneName:");
			return 0;
		}
		//x(0), y(0), rotation(0), scaleX(0), scaleY(0),
		double ret = mbone->rotation;
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getBoneRotation", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_Spine_extend_spine_getBoneRotation'.", &tolua_err);
#endif

	return 0;
}

static int lua_Spine_extend_spine_getBoneScaleX(lua_State* tolua_S)
{
	if (nullptr == tolua_S)
		return 0;

	int argc = 0;
	spine::SkeletonAnimation* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_Spine_extend_spine_getBoneScaleX'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 1)
	{
		//const char* arg;

		std::string arg_tmp;
		ok &= luaval_to_std_string(tolua_S, 2, &arg_tmp, "sp.SkeletonAnimation:getBoneScaleX");
		//arg = arg_tmp.c_str();

		if (!ok)
			return 0;
		auto mbone = cobj->findBone(arg_tmp);
		if (!mbone)
		{
			CCLOG("not bone in sp.SkeletonAnimation by boneName:");
			//tolua_error(tolua_S, "not bone in sp.SkeletonAnimation by boneName:");
			return 0;
		}
		//x(0), y(0), rotation(0), scaleX(0), scaleY(0),
		double ret = mbone->scaleX;
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getBoneScaleX", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_Spine_extend_spine_getBoneScaleX'.", &tolua_err);
#endif

	return 0;
}

static int lua_Spine_extend_spine_getBoneScaleY(lua_State* tolua_S)
{
	if (nullptr == tolua_S)
		return 0;

	int argc = 0;
	spine::SkeletonAnimation* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_Spine_extend_spine_getBoneScaleY'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 1)
	{
		//const char* arg;

		std::string arg_tmp;
		ok &= luaval_to_std_string(tolua_S, 2, &arg_tmp, "sp.SkeletonAnimation:getBoneScaleY");
		//arg = arg_tmp.c_str();

		if (!ok)
			return 0;
		auto mbone = cobj->findBone(arg_tmp);
		if (!mbone)
		{
			CCLOG("not bone in sp.SkeletonAnimation by boneName:");
			//tolua_error(tolua_S, "not bone in sp.SkeletonAnimation by boneName:");
			return 0;
		}
		//x(0), y(0), rotation(0), scaleX(0), scaleY(0),
		double ret = mbone->scaleY;
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "getBoneScaleY", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_Spine_extend_spine_scaleY'.", &tolua_err);
#endif

	return 0;
}

static void extendSpine(lua_State* L)
{
	lua_pushstring(L, "sp.SkeletonAnimation");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_istable(L, -1))
	{
		tolua_function(L, "getBonePos", lua_Spine_extend_spine_getBonePos);
		tolua_function(L, "getBoneRotation", lua_Spine_extend_spine_getBoneRotation);
		tolua_function(L, "getBoneScaleX", lua_Spine_extend_spine_getBoneScaleX);
		tolua_function(L, "getBoneScaleY", lua_Spine_extend_spine_getBoneScaleY);
	}
	lua_pop(L, 1);
}

static int applyGray(lua_State* L)
{
	bool ok = true;
	int argc = lua_gettop(L);

	do
	{
		if (argc == 1)
		{
			cocos2d::Node* arg0;
			ok &= luaval_to_object<cocos2d::Node>(L, 1, "cc.Node", &arg0);
			if (!ok) break;

			CSimpleShader::applyGray(arg0);
		}
	} while (0);

	lua_pushboolean(L, ok);
	return 1;
}

static int removeGray(lua_State* L)
{
	bool ok = true;
	int argc = lua_gettop(L);

	do
	{
		if (argc == 1)
		{
			cocos2d::Node* arg0;
			ok &= luaval_to_object<cocos2d::Node>(L, 1, "cc.Node", &arg0);
			if (!ok) break;

			CSimpleShader::removeGray(arg0);
		}
	} while (0);

	lua_pushboolean(L, ok);
	return 1;
}

static int applyHSV(lua_State* L)
{
	bool ok = true;
	int argc = lua_gettop(L);

	do
	{
		if (argc == 3)
		{
			Node* arg0;
			Vec3 arg1;
			bool arg2;

			ok &= luaval_to_object<Node>(L, 1, "cc.Node", &arg0, "applyHSV");
			if (!ok) break;
			ok &= luaval_to_vec3(L, 2, &arg1, "applyHSV");
			if (!ok) break;
			ok &= luaval_to_boolean(L, 3, &arg2, "applyHSV");
			if (!ok) break;

			if (arg2)
				CSimpleShader::applyHSVShader(arg0, arg1);
			else
				CSimpleShader::applyHSVShaderNoMVP(arg0, arg1);
		}
		else
		{
			luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "applyHSV", argc, 3);
		}
	} while (0);

	lua_pushboolean(L, ok);
	return 1;
}

static int applyHue(lua_State* L)
{
	bool ok = true;
	int argc = lua_gettop(L);

	do
	{
		if (argc == 3)
		{
			Node* arg0;
			double arg1;
			bool arg2;

			ok &= luaval_to_object<Node>(L, 1, "cc.Node", &arg0, "applyHue");
			if (!ok) break;
			ok &= luaval_to_number(L, 2, &arg1, "applyHue");
			if (!ok) break;
			ok &= luaval_to_boolean(L, 3, &arg2, "applyHue");
			if (!ok) break;

			if (arg2)
				CSimpleShader::applyHueShader(arg0, arg1);
			else
				CSimpleShader::applyHueShaderNoMVP(arg0, arg1);
		}
		else
		{
			luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "applyHue", argc, 3);
		}
	} while (0);

	lua_pushboolean(L, ok);
	return 1;
}

void extendShaderTools(lua_State* L)
{
	lua_register(L, "applyGray", applyGray);
	lua_register(L, "removeGray", removeGray);
	lua_register(L, "applyHSV", applyHSV);
	lua_register(L, "applyHue", applyHue);
}

int register_all_extend_manual(lua_State* L)
{
	if (nullptr == L)
		return 0;

	extendSpine(L);
	extendShaderTools(L);

	return 0;
}

bool regiestExtend()
{
	auto L = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	register_all_extend_manual(L);

	return true;
}

