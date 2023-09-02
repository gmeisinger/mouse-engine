--[[ Intro Sequence
--
-- Create a Sprite for each letter
-- Find the array of positions for the string to be centered
-- position each sprite just off screen
-- update position until its where it needs to be
--
--]]--

Intro = {
  _extends = Node,
  titleString = "MOUSE ENGINE",
  center_x = 0,
  center_y = 0
}

function Intro:Start(node)
  self.center_x = (Mouse.Graphics.getScreenWidth() // 2) - (#self.titleString // 2)
  self.center_y = Mouse.Graphics.getScreenHeight() // 2
  for i = 1, #self.titleString do
    local top = i % 2
    local c = self.titleString:sub(i,i)
    local letter = Sprite.new()
    local x = self.center_x + i
    local y = -i
    if top == 1 then
      y = Mouse.Graphics.getScreenHeight() + i
    end
    letter:setAscii(c)
    letter:setPosition(x, y)
    node:addChild(letter)
  end
end
function Intro:Update(node)
  if node:childCount() < 1 then
    error("no kids!!!")
  end
  for i = 0, node:childCount() do
    --local c = self.titleString:sub(i,i)
    local letter = node:getChild(i)
    if letter == nil then
      error("nil kid!")
    end
    local y = letter:getY()
    if y > self.center_y then
      letter:setY(y - 1)
    elseif y < self.center_y then
      letter:setY(y + 1)
    end
  end
end
