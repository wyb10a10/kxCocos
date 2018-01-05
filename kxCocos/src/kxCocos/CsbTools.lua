----------------------使用到csb里面的一些基本方法--------------------
CsbTools = {}
local PopTip = require("kxCocos.PopTip")

-- 递归获取节点
function CsbTools.getChildFromPath(node, path)
    local list = string.split(path, "/")
	local ret = node
	for k, v in pairs(list) do
		if not ret then 
			return nil
		end
		ret = ret:getChildByName(v)
	end
	return ret
end

-- 替换图片
function CsbTools.replaceImg(imageNode, ImgName)
    if imageNode and ImgName then       
        if cc.SpriteFrameCache:getInstance():getSpriteFrame(ImgName) == nil then
            local info = debug.getinfo(2, "Sl")
            print(string.format("\"%s\" 图片不存在  [%s]:%d", ImgName, info.source, info.currentline))
            if device.platform == "android" or device.platform == "ios" or gIsQQHall then
                return
            end
        end 
        imageNode:loadTexture(ImgName, 1)
    end
end

function CsbTools.replaceSprite(spriteNode, ImgName)
    if spriteNode and ImgName then       
        if cc.SpriteFrameCache:getInstance():getSpriteFrame(ImgName) == nil then
            local info = debug.getinfo(2, "Sl")
            print(string.format("\"%s\" 图片不存在  [%s]:%d", ImgName, info.source, info.currentline))
            if device.platform == "android" or device.platform == "ios" or gIsQQHall then
                return
            end
        end 
        spriteNode:setSpriteFrame(ImgName)
    end
end

-- 由于关闭界面的时候动画会暂停, 再次打开的时候导致文字还存在,
-- 添加到runningScene后动作不会暂停
function CsbTools.addTipsToRunningScene(tipText, format)
    local mScene = display.getRunningScene()
    if not mScene.allTipList then
        mScene.allTipList = {}
    end
    if not(CsbTools.tipsId and CsbTools.tipsId > 0) then
        CsbTools.tipsId = 0
    end

    CsbTools.tipsId = CsbTools.tipsId + 1
    local sceneTag = CsbTools.tipsId
    format = format or {}
    local tips = PopTip.new({
        text = tipText or "nil", 
        font = format.font or "../fonts/msyh.ttf",
        animate = format.animate or 1,
        x = format.x or display.cx,
        y = format.y or display.cy,
        size = format.size or 32, 
        color = format.color or cc.c3b(253, 243, 77),
        align = format.align or cc.TEXT_ALIGNMENT_CENTER,
        valign= format.valign or cc.VERTICAL_TEXT_ALIGNMENT_CENTER,
        dimensions = cc.size(display.width, display.height),
        pic = format.pic or "",
        picScale = format.picScale or 1,
        removeFun = function( ... )
            if mScene.allTipList then
                for k,v in ipairs(mScene.allTipList) do
                    if v and v.sceneTag and sceneTag and v.sceneTag == sceneTag then
                        table.remove(mScene.allTipList, k)
                        break
                    end
                end
            end
        end
    })
    tips.sceneTag = sceneTag
    mScene:addChild(tips, 1000)

    for k,v in ipairs(mScene.allTipList) do
        if v then
            v:setPositionY(v:getPositionY()+40)
        end
    end
    table.insert(mScene.allTipList, tips)

    return tips
end

-- 填充按钮文字，传入按钮button、要设置的文字text、文字节点textNode、按钮下的节点csbNode
-- 实现按钮点击时，按钮下方的内容自动跟随按钮进行缩放的效果
-- textNode参数详解
-- textNode为空时默认查找按钮下的Text节点作为文字节点，找不到则不处理
-- textNode为字符串时默认查找按下指定名字的节点作为文字节点
-- 其他情况需要将按钮的文字节点作为textNode参数传入
-- csbNode参数详解
-- csbNode为空时默认使用按钮下的Text节点作为缩放节点
-- csbNode为字符串时默认查找按下指定名字的节点作为文字节点
-- 其他情况需要将按钮的文字节点作为textNode参数传入
function CsbTools.fillButtonText(button, text, textNode, csbNode)
    if not textNode then
        textNode = button:getChildByName("Text")
    elseif type(textNode) == "string" then
        textNode = CsbTools.getChildFromPath(button, textNode)
    end

    if textNode and type(text) == "string" then
        textNode:setString(text)
    end

    if csbNode == nil and textNode then
        csbNode = textNode
    elseif type(csbNode) == "string" then
        csbNode = CsbTools.getChildFromPath(button, csbNode)
    end

    if not csbNode then
        return
    end

    -- textNode可以传入节点、空、字符串
    local callback = function ()
        local normalNode = button:getRendererNormal()
        csbNode:setScaleX(normalNode:getScaleX())
        csbNode:setScaleY(normalNode:getScaleY())
    end
    csbNode:scheduleUpdateWithPriorityLua(callback, 0) 
end

-- 初始化按钮，传入按钮、点击回调、要设置的文字text、文字节点textNode、按钮下的节点csbNode
-- 除按钮点击回调外，其他参数同fillButtonText方法
function CsbTools.initButton(button, callback, text, textNode, csbNode)
    button:setTouchEnabled(true)
    button:setSwallowTouches(true)
    button:addClickEventListener(function (obj)
        if callback then
            callback(obj)
        end

        -- 根据音效id或按钮名播放音效
        MusicManager.playSoundEffect(obj.soundId or obj:getName())
    end)
    CsbTools.fillButtonText(button, text, textNode, csbNode)
end

-- Cocos2d-x UI组建包装一下，这样可以使用：
-- local xx = ui_delegate(ui_Load_BFile("res/MainScene.csb"))
-- xx.button来访问控件
function CsbTools.ui_(nativeWidget, name)
--print(nativeWidget, name, type(name))
    if type(name) == "string" then
        -- return ccui.Helper:seekWidgetByName( nativeWidget, name)
        local control;
        nativeWidget:enumerateChildren("//"..name, function(ret)
          if ret then
            control = ret
            return true
          end
          return false
        end)
        return control
    else
        -- return ccui.Helper:seekWidgetByTag( nativeWidget, name)
        return nativeWidget:getChildByTag(name)
    end
end
  
function CsbTools.ui_delegate(nativeUI)
    if(nativeUI == nil)then
        return nil
    end
  
    local r = {["nativeUI"] = nativeUI}
    setmetatable(r, {
        __index = function(t,k)
            local u = CsbTools.ui_(t.nativeUI, k)
            rawset(t,k,u)
            return u
        end}
    )
    return r
end