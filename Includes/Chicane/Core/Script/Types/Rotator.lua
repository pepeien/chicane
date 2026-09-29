---@meta

---@class Rotator
Rotator = {}

---@param pitch number
---@param roll number
---@param yaw number
---@return Rotator
function Rotator(pitch, roll, yaw) end

---@param pitch number
---@param roll number|nil
---@param yaw number|nil
function Rotator:set(pitch, roll, yaw) end

---@param pitch number
---@param roll number|nil
---@param yaw number|nil
function Rotator:addLocal(pitch, roll, yaw) end

---@param pitch number
---@param roll number|nil
---@param yaw number|nil
function Rotator:addWorld(pitch, roll, yaw) end

---@param origin Vec3
---@param target Vec3
function Rotator:lookAt(origin, target) end

---@return Vec3
function Rotator:getAngles() end

---@param angles Vec3
function Rotator:setAngles(angles) end

---@return Vec3
function Rotator:getRight() end

---@return Vec3
function Rotator:getForward() end

---@return Vec3
function Rotator:getUp() end
