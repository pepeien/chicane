---@meta

---@class Instance
Instance = {}

---@param fn fun()
function Instance.onLoad(fn) end

---@param fn fun(delta: number)
function Instance.onTick(fn) end

---@param selector string
---@return Component|nil
function Instance.find(selector) end

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
