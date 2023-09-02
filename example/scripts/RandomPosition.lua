RandomPosition = {
  _extends = Sprite 
}

function RandomPosition:Start(node)
  -- node:setAscii("@")
  node:setPosition(math.random(0,3), math.random(0,3))
  print(node:getName(), node:getAscii(), node:getPosition())
end
function RandomPosition:Update(node)
  node:setPosition(math.random(0,3), math.random(0,3))
  -- print(node:getName(), node:getAscii(), node:getPosition())
end
