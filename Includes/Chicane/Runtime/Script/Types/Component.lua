---@meta

---@class Component
local Component = {}

---@return string
function Component:getId() end

---@param id string
function Component:setId(id) end

---@return Vec3
function Component:getTranslation() end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Component:setTranslation(x, y, z) end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Component:lookAt(x, y, z) end

---@return boolean
function Component:isActive() end

function Component:activate() end

function Component:deactivate() end

---@return Actor|Component|nil
function Component:getParent() end

---@return Component[]
function Component:getAttachments() end
