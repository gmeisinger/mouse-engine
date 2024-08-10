--[[ INNER MONOLOGUE SIMULATOR ]]--

happy_messages = {
  "WORK HARD",
  "EYES ON THE PRIZE",
  "DO YOUR BEST",
  "ONE PROBLEM AT A TIME",
  "FUCK BLE",
  "YOU GOT THIS",
  "GO AFTER IT"
}

messages = happy_messages

Intro = {
  _extends = Node,
  titleString = "MOUSE ENGINE",
  center_x = 0,
  center_y = 0,
  letter_pos = {},
  rev = false,
  turn = 0,
  msg = messages[1]
}

function Intro:Start(node)
  math.randomseed(os.time())
  self.center_x = (Mouse.Graphics.getScreenWidth() // 2) - (#self.titleString // 2)
  self.center_y = Mouse.Graphics.getScreenHeight() // 2
  Mouse.Logger.log("hello!")
  for i = 1, #self.msg do
    local top = i % 2
    local c = self.msg:sub(i,i)
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
  for i = 0, node:childCount()-1 do
    --local c = self.titleString:sub(i,i)
    local letter = node:getChild(i)
    local bottom = Mouse.Graphics.getScreenHeight() + i
    if letter == nil then
      error("nil kid!")
    end
    local y = letter:getY()
    local top = i % 2
    if self.rev and i <= self.turn then
      if top == 0 and y >= -i then
        letter:setY(y-1)
      elseif top == 1 and y <= bottom then
        letter:setY(y+1)
      elseif i == node:childCount() - 1 then
        self.rev = false
        self:Restart(node)
      end
    elseif not self.rev then 
      if y > self.center_y then
        letter:setY(y - 1)
      elseif y < self.center_y then
        letter:setY(y + 1)
      elseif i == node:childCount() - 1 then
        self.rev = true
        self.turn = 0
      end
    end
  end
  self.turn = self.turn + 1
end
function Intro:Restart(node)
  while node:childCount() > 0 do
    node:removeChild(0)
  end
  self.msg = messages[math.random(#messages)]
  for i = 1, #self.msg do
    local top = i % 2
    local c = self.msg:sub(i,i)
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
