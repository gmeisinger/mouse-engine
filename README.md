# Mouse Engine

A tiny game engine designed with ASCII graphics in mind.

### Dependencies

* Lua 5.4
* yaml-cpp [https://github.com/jbeder/yaml-cpp]()
* ncurses

### Adding Functionality to Nodes

1. Write the class function
2. Write the lua wrapper
    * Takes lua_State, returns int, uses stack
3. Add the function to the register list

### What is exposed to Lua?

1. Base types - Like Node, these are C++ objects that have been exposed to lua as userdata. These types can be instanced and scripted in lua.
2. Modules - These are simple wrappers around common methods, exposed to lua as a Table that can be used to interact with the engine.
