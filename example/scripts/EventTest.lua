--[[EventManager Test]]--

EventTest = {
  _extends = Node,
  testString = "This is a test.",
  cb_fired = false
}

function EventTest:Start(node)
  math.randomseed(os.time())
  self.center_x = (Mouse.Graphics.getScreenWidth() // 2) - (#self.testString // 2)
  self.center_y = Mouse.Graphics.getScreenHeight() // 2
  Mouse.Logger.log("hello!")
  Mouse.Events.subscribe(node, "test", EventTest.Callback)
  Mouse.Logger.log("hello again!")
end
function EventTest:Update(node)
  if not cb_fired then
    cb_fired = true
    Mouse.Events.fire("test")
    Mouse.Logger.log("Firing!")
  end
end
function EventTest:Callback(node)
  Mouse.Logger.log("Callback fired!")
end