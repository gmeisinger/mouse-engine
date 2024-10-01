--[[EventManager Test]]--

EventTest = {
  _extends = Node,
  testString = "This is a test.",
  cb_fired = false
}

function EventTest:Start(node)
  math.randomseed(os.time())
  self.center_x = (Mouse.Graphics.getScreenWidth() // 2)
  self.center_y = Mouse.Graphics.getScreenHeight() // 2
  Mouse.Events.subscribe(node, "test", EventTest.Callback)
  local letter = Sprite.new()
  letter:setAscii("@")
  letter:setPosition(self.center_x, self.center_y)
  node:addChild(letter)
end
function EventTest:Update(node)
  if not cb_fired then
    cb_fired = true
    Mouse.Logger.log("Firing!")
    Mouse.Events.fire("test", "extra_arg")
  end
end
function EventTest:Callback(node, a_string)
  Mouse.Logger.log("Callback fired!")
  Mouse.Logger.log(a_string)
  Mouse.Logger.log("Callback complete!")
end