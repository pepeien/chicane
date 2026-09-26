---@meta

---@class Actor
local Actor = {}

---@return string
function Actor:getId() end

---@param id string
function Actor:setId(id) end

---@return Vec3
function Actor:getTranslation() end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Actor:setTranslation(x, y, z) end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Actor:lookAt(x, y, z) end

---@return Vec3
function Actor:getCenter() end

---@return Vec3
function Actor:getSize() end

---@return boolean
function Actor:isTransient() end

---@return Component[]
function Actor:getAttachments() end
