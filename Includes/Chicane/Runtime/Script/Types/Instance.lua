---@meta

---@class Scene
Instance = {}

---@param fn fun()
function Instance.onLoad(fn) end

---@param fn fun(delta: number)
function Instance.onTick(fn) end

---@param id string
---@return Actor|nil
function Instance.findActor(id) end

---@return Actor[]
function Instance.getActors() end

---@param id string
---@return Component|nil
function Instance.findComponent(id) end

---@return Component[]
function Instance.getComponents() end

---@overload fun(origin: Vec3, destination: Vec3, ignored: Actor[]|nil): Trace|nil
---@param request TraceRequest
---@param ignored Actor[]|nil
---@return Trace|nil
function Instance.trace(request, ignored) end

---@overload fun(origin: Vec3, destination: Vec3, ignored: Actor[]|nil): Trace[]
---@param request TraceRequest
---@param ignored Actor[]|nil
---@return Trace[]
function Instance.traceMulti(request, ignored) end

---@param text string
function Instance.Log(text) end

---@param name string
---@param callback fun(data: string)
---@return integer
function Instance.subscribe(name, callback) end

---@param token integer
function Instance.unsubscribe(token) end

---@param name string
---@param data string|nil
function Instance.send(name, data) end
