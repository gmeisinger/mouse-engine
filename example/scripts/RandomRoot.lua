RandomRoot = {
  _extends = Node
}

function RandomRoot:Start(node)
  print("Random positions go!")
  local baby1 = Sprite.new("baby1")
  baby1:setScript("scripts/RandomPosition.lua")
  node:addChild(baby1)
  local baby2 = Sprite.new("baby2")
  baby2:setScript("scripts/RandomPosition.lua")
  baby2:setAscii("@")
  node:addChild(baby2)
  print("Added 2 babies!")
end
function RandomRoot:Update(node)
  x1, y1 = node:getChild("baby1"):getPosition()
  x2, y2 = node:getChild("baby2"):getPosition()
  if x1 == x2 and y1 == y2 then
    print("Whoa the babies touched! Gay!")
  end
end
