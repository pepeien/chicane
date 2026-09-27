---@meta

-- Absolute-transform helpers that Lua exposes under these names. The C++ API
-- they call is variadic, so they cannot come from reflection.
---@class Object
local Object = {}

---@return Vec3
function Object:getTranslation() end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Object:setTranslation(x, y, z) end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function Object:lookAt(x, y, z) end

---@return Vec3
function Object:getCenter() end

---@return Vec3
function Object:getSize() end
