---@meta

---@class ACharacter : APawn
local ACharacter = {}

---@param scale number
function ACharacter:setMoveScale(scale) end

---@param forward number
---@param right number
---@param up number|nil
function ACharacter:setMoveInput(forward, right, up) end

---@param x number|Vec3
---@param y number
---@param z number|nil
---@param scale number|nil
function ACharacter:move(x, y, z, scale) end

---@param speed number|nil
function ACharacter:jump(speed) end

---@param value number
function ACharacter:addPitch(value) end

---@param value number
function ACharacter:addYaw(value) end

---@param value number
function ACharacter:addRoll(value) end
