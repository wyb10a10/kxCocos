--[[
	SceneManager用于管理场景
	1. 自动清理当前场景，并加载下一个场景所需资源
	2. 提供场景切换的回调
	3. 提供Loading场景（Loading场景）
	4. 用于UI栈恢复的资源预加载
	5. 支持只加载资源，资源加载完成后的场景切换由传入的回调处理

	2015-11-01 by 宝爷
]]

local UIManager = require("kxCocos.ui.UIManager")
local ResLoader = require("kxCocos.ResLoader")
local scheduler = cc.Director:getInstance():getScheduler()

SceneManager = SceneManager or {}

SceneManager.CurScene = nil
SceneManager.PrevScene = nil
SceneManager.ChangeSceneDelegate = nil

function SceneManager.init()
	-- 建立一个SceneName的Map
	SceneManager.SceneName = {}
	for k,v in pairs(SceneManager.Scene) do
		SceneManager.SceneName[v] = k
	end
end

-- 切换场景（不带资源加载和清理）
function SceneManager.changeScene(sceneId, ...)
	-- 切换Lua场景
	app:enterScene(SceneManager.SceneName[sceneId], ...)
end

-- 直接切换场景（不带资源加载和清理）
function SceneManager.replaceScene(sceneId, scene)
   cc.Director:getInstance():replaceScene(scene)
end

-- 加载场景资源，完成后执行回调
-- 如果需要手动获取资源加载的信息，可以设置ResLoader.onLoading回调
function SceneManager.loadSceneRes(sceneId, finishCallback, ...)
	print("SceneManager.loadSceneRes" .. sceneId)
    -- 获取场景配置
    local sceneCfg = SceneManager.SceneConfig[SceneManager.SceneName[sceneId]]
    if sceneCfg == nil then
    	return
    end

    -- 场景资源加载回调
    local scheduleHandle = nil
    local startLoad = function(dt)
    	-- 只执行一次回调
        if scheduleHandle then
            scheduler.unscheduleGlobal(scheduleHandle)
        end

        -- 是否恢复场景UI栈
        if UIManager.hasSave() and UIManager.SaveScene == sceneId then
            local loadCount = #UIManager.UIStackCache
            ResConfig.Cache.UI = {}
            for i = 1, loadCount do
                ResConfig.Cache.UI[#ResConfig.Cache.UI + 1] =
                UIManager.UIName[UIManager.UIStackCache[i].UIID]
            end
        end

        -- 缓存资源（标记这些资源不要被清理）
        local formatedRes = {}
        ResLoader.formatRes(ResConfig.Cache, formatedRes)
		ResLoader.formatRes(ResConfig.Common, formatedRes)
		ResLoader.formatRes(ResConfig[SceneManager.SceneName[sceneId]], formatedRes)
		ResLoader.cacheRes(formatedRes)

		-- ResConfig.Cache是一次性的缓存资源
		ResConfig.Cache = {}

        -- 清理资源
	    getResManager():clearRes()
        -- 加载新资源
        if type(SceneManager.ChangeSceneDelegate) == "function" then
            SceneManager.ChangeSceneDelegate()
        end
		ResLoader.addPreloadRes(formatedRes)
    	ResLoader.startLoadResAsyn()
    end

    -- 先关闭所有UI，清理UI缓存
	UIManager.closeAll()
	UIManager.clearCache()

    local params = {...}
    local wrapCallback = function ()
        SceneManager.PrevScene = SceneManager.CurScene
		SceneManager.CurScene = sceneId
		print("SceneManager.changeScene " .. SceneManager.CurScene .. " prev " .. (SceneManager.PrevScene or "nil"))
	    finishCallback(sceneId, params)
	end

    -- 根据配置决定是否切换Loading场景
    -- 如果不切换Loading场景则在加载完成后执行回调
    -- 否则创建Loading场景，并切换到Loading场景，在Loading场景加载完资源后执行回调
    if type(sceneCfg.loadingView) == "string" then
        -- 切换到loading场景
        scheduleHandle = scheduler.scheduleGlobal(startLoad, 0)
        app:enterScene(sceneCfg.loadingView, {ResLoader, wrapCallback})
    else
        ResLoader.LoadFinishCallback = wrapCallback
        startLoad()
    end
end

-- 自动切换场景
function SceneManager.loadScene(sceneId, ...)
	SceneManager.loadSceneRes(sceneId, SceneManager.changeScene, ...)
end

return SceneManager
