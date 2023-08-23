RandomPosition = {
  _extends = Node2d
}

function RandomPosition:Start(node)
  node:setPosition(math.random(0,3), math.random(0,3))
  print(node:getName(), node:getPosition())
end
function RandomPosition:Update(node)
  node:setPosition(math.random(0,3), math.random(0,3))
  print(node:getName(), node:getPosition())
end