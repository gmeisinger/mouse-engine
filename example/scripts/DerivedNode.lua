-- This is how you inherit Node
DerivedNode = {
  _extends = Node,
  value = 1}

function DerivedNode:Start(node)
  print(node:getName(), " started!")
  if node:getName() == "gay" then 
    node:addChild(Node.new("gayson"))
    node:getChild("gayson"):setScript("DerivedNode")
  end
end
function DerivedNode:Update(node)
  self.value = self.value + 1
  print(node:getName(), " updated!", self.value)
  -- print(self:generateRandomString(2))
end
function DerivedNode:generateRandomString(n)
  -- Define the characters to choose from
  local chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

  -- Initialize an empty string to store the result
  local result = ""

  -- Use a loop to generate the random string
  for i = 1, n do
      -- Generate a random index within the range of chars
      local randomIndex = math.random(1, #chars)

      -- Append the character at the random index to the result
      result = result .. string.sub(chars, randomIndex, randomIndex)
  end

  return result
end