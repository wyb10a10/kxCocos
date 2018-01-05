
CommonHelper = {}

function printTrace()
    print(debug.traceback())
end

function replaceStr(str, ...) -- 替换str中的{}为参数
    for _, v in ipairs{...} do
        str = string.gsub(str, "{}", v, 1) -- 替换一个{}为v,每个参数替换一个
    end

    return str
end

function CommonHelper.playCsbAnimation(node, actName, loop, fun)
    local action = node:getActionByTag(node:getTag())
    if nil == action then
        return
    end
    action:play(actName, loop)
    if  loop ~= false or type(fun) ~= "function" then
        return
    end
    local actionTime = (action:getEndFrame() - action:getStartFrame()) / 60
    node:runAction(cc.Sequence:create(cc.DelayTime:create(actionTime), cc.CallFunc:create(fun)))
end

function CommonHelper.playCsbAnimate(node, act, actName, loop, fun, cache)
    local action = act
    if node._csbAnimate ~= nil and cache == true then
        action =  node._csbAnimate
    else
        if type(act) == "string" then
            action = cc.CSLoader:createTimeline(act)
        end

        if action == nil then
            return
        end
        node._csbAnimate = action
        node:runAction(action)
    end
    action:play(actName, loop)

    if  loop ~= false or type(fun) ~= "function" then
        return
    end
    local actionTime = (action:getEndFrame() - action:getStartFrame()) / 60
    actionTime = math.floor(actionTime*100)/100
    node:runAction(cc.Sequence:create(cc.DelayTime:create(actionTime), cc.CallFunc:create(fun)))
end

function CommonHelper.playSpineAnimation(animateNode, animations)
    if not animateNode or not animations then
        return
    end

    animateNode:clearTracks()
	animateNode:setToSetupPose()  -- 去掉残影
    for k, v in ipairs(animations) do
        local entry = animateNode:addAnimation(0, v, true)
        if entry then
            animateNode:animationStateApply()
        end	
    end
end

function CommonHelper.layoutNode(node)
    node:setContentSize(display.width, display.height)
    ccui.Helper:doLayout(node)
end

--递归获取节点
function CommonHelper.getChild(node, path)
    local list = {}
    for match in (path .. '/'):gmatch("(.-)/") do
        table.insert(list, match)
    end

    local ret = node
    for k, v in pairs(list) do
        if not ret then
            return nil
        end
        ret = ret:getChildByName(v)
    end
    return ret
end

function CommonHelper.replaceCsbNodeWithWidget(node)
	local ret = ccui.Widget:create()
    ret:setName(node:getName())
    ret:setTag(node:getTag())

	local children = node:getChildren()
	for _, n in pairs(children) do
		n:retain()
		n:removeFromParent(false)
		ret:addChild(n)
		n:release()
	end
	return ret
end

-- dump一个对象
function CommonHelper.dumpObject(obj)
    local tp = type(obj)
    print("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww")
    print("obj type is nil" .. tp)
    if tp == "string"
    or tp == "boolean"
    or tp == "number" then
        print("obj is " .. obj)
    elseif tp == "table" then
        for k, v in pairs(obj) do
            if type(k) == "string" then
                print(k .. "    " .. type(v))
            else
                print(type(k) .. "    " .. type(v))
            end
        end
    end
    print("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww")
end

-- 节点，是否递归，是否显示tag
function CommonHelper.dumpNode(node, prefix)
    if prefix == nil then
        print("++++++++++++++++++++++++++++++++++++++++")
        prefix = ""
    end
    print(prefix .. " Name " ..  node:getName() .. " Tag " .. node:getTag())
    local children = node:getChildren()
    for _, n in pairs(children) do
         CommonHelper.dumpNode(n, prefix .. "----")
    end
end

-- 播放动画并执行回调
function CommonHelper.playActOverFrameCallBack(nodeAct, actName, loop, frameFunc)
    if not nodeAct or "function" ~= type(frameFunc) then
        return
    end
    nodeAct:pause()
    nodeAct:clearFrameEventCallFunc() -- 清理之前的帧事件
    nodeAct:play(actName, loop)
    nodeAct:setFrameEventCallFunc(frameFunc)
end

-- 移动一个节点到另一个节点下
function CommonHelper.moveNode(node, newParent)
    local ret = node:getParent()
    if ret then
        local posX, posY = node:getPosition()
        local worldPos = ret:convertToWorldSpace(cc.p(posX, posY))
        local newPos = newParent:convertToNodeSpace(cc.p(worldPos.x, worldPos.y))
        print(newPos.x, newPos.y)
        node:retain()
        node:removeFromParent(false)
        newParent:addChild(node)
        node:setPosition(newPos.x, newPos.y)
        node:release()
    end
    return ret
end

-- n个汉字或n*2个字符(utf8一个汉字3个字节)
function CommonHelper.limitStrLen(str, n)
    local newStr = ""
    local len = string.len(str)
    local byteCount = 0
    local chinese = 0

    for i = 1, len do
        local s = ""
        local curByte = string.byte(str, i)

        if curByte > 0 and curByte <= 127 then
            byteCount = byteCount + 1
            s = string.sub(str, i, i)

        elseif curByte >= 192 and curByte < 223 then
            byteCount = byteCount + 2
            chinese = chinese + 1
            s = string.sub(str, i, i + 1)

        elseif curByte >= 224 and curByte < 239 then
            byteCount = byteCount + 3
            chinese = chinese + 1
            s = string.sub(str, i, i + 2)

        elseif curByte >= 240 and curByte <= 247 then
            --byteCount = byteCount + 4
        end

        if chinese <= n and byteCount - 2 * n <= chinese then
            newStr = newStr .. s
        else
            break
        end
    end

    return newStr
end

function CommonHelper.getKeys(t)
    local ret = {}
    for k, _ in pairs(t) do
        table.insert(ret, k)
    end
    return ret
end

-- 在一天当中经过了多少秒
function CommonHelper.passedSecOfDay(...)
    local date

    local args = {...}
    if #args == 1 then
        date = os.date("*t", args[1])
    elseif #args == 3 then
        date = { hour = args[1], min = args[2], sec = args[3] }
    else
        printError("wrong number of arguments")
        return
    end

    return date.hour * 60 * 60 + date.min * 60 + date.sec
end

-- 生成一个随机的字符串，字符串内容为小写字母和数字
function CommonHelper.randomStr(len)
    len = len or 0
    local str = {}
    for _ = 1, len do
        local i = math.random(0, 35)
        if i <= 9 then
            table.insert(str, i)
        else
            i = string.byte("a") + i - 10
            table.insert(str, string.format("%c", i))
        end
    end
    return table.concat(str)
end

-- 将颜色c3b十进制转为十六进制
local hashHex = {[10] = 'a', [11] = 'b', [12] = 'c', [13] = 'd', [14] = 'e', [15] = 'f'}
function CommonHelper.c3bToHex(r, g, b)
    local function toHex(n)
        local function toHexChar(c)
            return hashHex[c] or c
        end

        local a = math.floor(n / 16) % 16
        local b = math.floor(n % 16)

        return toHexChar(a)..toHexChar(b)
    end

    return toHex(r)..toHex(g)..toHex(b)
end

function CommonHelper.urlEncode(str)
    if str and type(str) == "string" then
        str = string.gsub (str, "\n", "\r\n")
        str = string.gsub (str, "([^%w ])",
            function (c) return string.format ("%%%02X", string.byte(c)) end)
        str = string.gsub (str, " ", "+")
    end
    return str
end

-- 0 ~ 39
function CommonHelper.intToRoman(num)
    --dump(num, "num")
    local c = {
        {"","Ⅰ","Ⅱ","Ⅲ","Ⅳ","Ⅴ","Ⅵ","Ⅶ","Ⅷ","Ⅸ"},
        {"","Ⅹ","ⅩⅩ","ⅩⅩⅩ"}
    }
    local roman = "" 
    roman = roman .. (c[2][math.floor(num / 10) % 10 + 1])
    roman = roman .. (c[1][math.floor(num) % 10 + 1])
    return roman
end

-- 添加spine到spine的骨骼上（暂时只支持pos,Rotation,ScaleX,ScaleY属性更改）
function CommonHelper.addSpineToSpineBone(c_spine, parent, boneName)
    if not(c_spine and parent and parent.getBonePos and boneName) then
        print("erro:CommonHelper.addSpineToSpineBone c_spine and parent and boneName")
        return
    end
    local pos = parent:getBonePos(boneName)
    if not pos then
        print("erro:CommonHelper.addSpineToSpineBone->parent:getBonePos(boneName) nil")
        return
    end
    
    parent:addChild(c_spine)
    c_spine:setPosition(pos)
    c_spine:setRotation(parent:getBoneRotation(boneName))
    c_spine:setScaleX(parent:getBoneScaleX(boneName))
    c_spine:setScaleY(parent:getBoneScaleY(boneName))
    function c_spine.play(ani)
        c_spine:setAnimation(0, ani, true)
    end
    return c_spine
end

function CommonHelper.applyGray(node)
    if not node.isGray then
        -- applyGray(node)
        local program = cc.GLProgram:create("shaders/Gray.vsh", "shaders/Gray.fsh")
        program:bindAttribLocation(cc.ATTRIBUTE_NAME_POSITION, cc.VERTEX_ATTRIB_POSITION) 
        program:bindAttribLocation(cc.ATTRIBUTE_NAME_TEX_COORD, cc.VERTEX_ATTRIB_TEX_COORD)
        program:link()
        program:updateUniforms()
        node:setGLProgram(program)
        node.isGray = true
    end
end

function CommonHelper.removeGray(node)
    if node.isGray then
        -- removeGray(node)
        node:setGLProgramState(cc.GLProgramState:getOrCreateWithGLProgram(cc.GLProgramCache:getInstance():getGLProgram("ShaderPositionTextureColor_noMVP")))
        node.isGray = false
    end
end

function CommonHelper.applyWhiteGray(node)
    if not node.isGray then
        -- applyGray(node)
        local program = cc.GLProgram:create("shaders/Gray.vsh", "shaders/WhiteGray.fsh")
        program:bindAttribLocation(cc.ATTRIBUTE_NAME_POSITION, cc.VERTEX_ATTRIB_POSITION) 
        program:bindAttribLocation(cc.ATTRIBUTE_NAME_TEX_COORD, cc.VERTEX_ATTRIB_TEX_COORD)
        program:link()
        program:updateUniforms()
        node:setGLProgram(program)
        node.isGray = true
    end
end

function CommonHelper.removeWhiteGray(node)
    if node.isGray then
        -- removeGray(node)
        node:setGLProgramState(cc.GLProgramState:getOrCreateWithGLProgram(cc.GLProgramCache:getInstance():getGLProgram("ShaderPositionTextureColor_noMVP")))
        node.isGray = false
    end
end

function CommonHelper.applyHue(node, hue, isMvp)
    return applyHue(node, hue, isMvp)
end

function CommonHelper.applyHSV(node, hsv, isMvp)
    return applyHSV(node, hsv, isMvp)
end

-- 获得数字的逗号版（如1234567->1,234,567）
function CommonHelper.getCommaNumString(num)
    local numStr = tostring(num or 0)
    local numStrLen = string.len(numStr)
    local cn = 3
    if numStrLen > cn then
        local mNumStr = ""
        for i = 1,math.ceil(numStrLen/cn),1 do
            if i > 1 then
                mNumStr = ","..mNumStr
            end
            local fn = numStrLen-3*i+1
            if fn < 0 then
                fn = 0
            end
            mNumStr = string.sub(numStr, fn, numStrLen-(i-1)*3)..mNumStr
        end
        return mNumStr
    else
        return numStr
    end
end

-- 显示触摸层，防止触摸
function CommonHelper.showTouchLayer(isShow, arg)
    local mScene = cc.Director:getInstance():getRunningScene()
    if not mScene then
        print("not mScene")
    end
    if isShow then
        if mScene.myTouchLayer then
            mScene.myTouchLayer:setVisible(true)
        else
            mScene.myTouchLayer = ccui.Layout:create():addTo(mScene, 100000)
            :setTouchEnabled(true)
            :setSwallowTouches(true)
            :setContentSize(cc.size(display.width, display.height))
        end
    elseif mScene.myTouchLayer then
        mScene.myTouchLayer:setVisible(false)
    end
end

return CommonHelper
