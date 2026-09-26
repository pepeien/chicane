---@meta

---@class CPhysics : Component
local CPhysics = {}

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function CPhysics:moveTo(x, y, z) end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function CPhysics:moveBy(x, y, z) end

---@return Vec3
function CPhysics:getLinearVelocity() end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function CPhysics:setLinearVelocity(x, y, z) end

---@param direction Vec3
---@param force number
---@param location Vec3
function CPhysics:addImpulse(direction, force, location) end

---@param mass number
function CPhysics:setMass(mass) end

---@return number
function CPhysics:getGravityFactor() end

---@param factor number
function CPhysics:setGravityFactor(factor) end
