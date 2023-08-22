-- This is how you inherit Node
DerivedNode2d = {
  _extends = Node2d
}

function DerivedNode2d:Start(node)
  print(node:getName(), " started!")
  node:setX(6)
  node:setY(9)
end
function DerivedNode2d:Update(node)
  node:setPosition(node:getX(), node:getY() + 1)
  print("Position: ", node:getX(), node:getY())
end