--[[
    ResLoader用于异步加载资源，基于C++的ResManager
    1.传入要加载的资源（包括不释放的资源），准备预加载
    2.开启预加载，先cache要加载的资源，保证不被误删，再根据isCleanup决定是否进行一次清除
        如果执行清除，同时对引擎的缓存进行清理，最后开始异步加载
    3.可设置加载完成，以及进度更新的回调

    2015-10-30 by 宝爷
]]

ResLoader = ResLoader or {}

-- 预加载资源列表（集合） key为资源名，value为true or string or table
-- 1. value为true则调用getResManager:addPreloadRes 将key传入
-- 2. value为"Armature" string则调用getResManager:addPreloadArmature 将key传入
-- 3. value为string则调用getResManager:addPreloadRes 将key、value一同传入
-- value为table表示该资源包含了复杂的辅助资源（暂无此类资源）
ResLoader.Res = {}
-- 加载进度回调, 参数为：
-- 资源总量、当前加载量、当前加载的资源名、是否成功
ResLoader.LoadingCallback = nil
-- 加载结束回调, 无参数
ResLoader.LoadFinishCallback = nil
-- 预加载的总量
ResLoader.AllCount = 0
-- 当前加载完成的量
ResLoader.CurrentCount = 0

-- 格式化资源，将标准资源src的中的UI，Func转成最小资源单位
-- 输出的table为 { skel = atlas, csbpath = Csb2, Cache = {} ... }
function ResLoader.formatRes(src, dest)
	if type(src) ~= "table" or
		type(dest) ~= "table" then
		return
	end

	for resType, resTable in pairs(src) do
		if type(resTable) == "table" then
			for k, v in pairs(resTable) do
				if resType == "UI" then
					-- UI的配置为 { "UILogin", "UIMail" ... }
					-- 从ResConfig的UI配置中查找
					local uiRes = ResConfig[v]
					ResLoader.formatRes(uiRes, dest)
				elseif resType == "Func" then
					-- Func的配置为 { "MainRes", "XXXRes" ... }
					local funRes = ResConfig[v]()
					ResLoader.formatRes(funRes, dest)
				elseif resType == "Spine" then
					-- Spine的配置为 { "skel" = "atlas" ... }
					dest[k] = v
				elseif resType == "Cache" then
					-- 记录缓存的资源类型
					local cacheResId = dest["Cache"] or {}
					cacheResId[#cacheResId + 1] = v
				else
					-- 纹理，CSB，音乐等资源，k为下标或可忽略的索引名称
					dest[v] = resType
				end
			end
		end
	end
end

function ResLoader.cacheRes(formatedRes)
	for k, paths in pairs(formatedRes) do
		if k == "Cache" then
			for resType, _ in pairs(paths) do
				getResManager():cacheResInt(resType)
				print("===========Cache Cache", resType)
			end
		else
			getResManager():cacheResStr(k)
			print("============Cache Other", k)
		end
	end
end

-- 传入资源或资源列表（已格式化的）
function ResLoader.addPreloadRes(formatedRes)
    -- 合并到ResLoader.Res中
    if type(formatedRes) == "table" then
    	for k,v in pairs(formatedRes) do
    		ResLoader.Res[k] = v
    	end
   	elseif type(formatedRes) == "string" then
   		ResLoader.Res[formatedRes] = true
    end
end

function ResLoader.startLoadResAsyn()
	ResLoader.AllCount = 0
	ResLoader.CurrentCount = 0
	for k,v in pairs(ResLoader.Res) do
		-- Armature骨骼特殊处理
		if v == "Armature" then
			if getResManager():addPreloadArmature(k, ResLoader.onLoading) then
				ResLoader.AllCount = ResLoader.AllCount + 1
			end
		-- 标准资源直接加载
		elseif ResConfig.ResType[v] ~= nil then
			if getResManager():addPreloadRes(k, ResLoader.onLoading) then
				ResLoader.AllCount = ResLoader.AllCount + 1
			end
		-- 多参数资源加载（如Spine = skel + atlas）
		elseif type(v) == "string" then
			if getResManager():addPreloadRes(k, v, ResLoader.onLoading) then
				ResLoader.AllCount = ResLoader.AllCount + 1
			end
		else
			print("Res Type is error !!!", v)
		end
	end
    
    print("Resloader allcount is " .. ResLoader.AllCount)
	if ResLoader.AllCount < 1 then
		ResLoader.onFinish()
		return true
	end

	getResManager():setFinishCallback(ResLoader.onFinish)
	getResManager():startResAsyn()

    -- 最后执行清空
    ResLoader.Res = {}
    return true
end

function ResLoader.onLoading(resName, success)
	if not success then
		print(resName .. " Load Failed")
	end
	ResLoader.CurrentCount = ResLoader.CurrentCount + 1
	if type(ResLoader.LoadingCallback) == "function" then
		ResLoader.LoadingCallback(ResLoader.CurrentCount, ResLoader.AllCount, resName, success)
	end
end

function ResLoader.onFinish()
    print("ResLoader.onFinish()")
    getResManager():setFinishCallback(nil)
	if type(ResLoader.LoadFinishCallback) == "function" then
		ResLoader.LoadFinishCallback()
        ResLoader.LoadFinishCallback = nil
        ResLoader.LoadingCallback = nil
	end
end

return ResLoader
