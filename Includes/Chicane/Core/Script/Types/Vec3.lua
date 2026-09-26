---@meta

---@class Vec3
---@field x number
---@field y number
---@field z number
Vec3 = {}

---@param x number
---@param y number
---@param z number
---@return Vec3
function Vec3(x, y, z) end

---@return Vec3
function Vec3.Zero() end

---@return Vec3
function Vec3.One() end

---@return Vec3
function Vec3.Right() end

---@return Vec3
function Vec3.Forward() end

---@return Vec3
function Vec3.Up() end

---@return string
function Vec3:toString() end

---@param value Vec3
---@return Vec3
function Vec3:min(value) end

---@param value Vec3
---@return Vec3
function Vec3:max(value) end

---@param value Vec3
---@return Vec3
function Vec3:cross(value) end

---@param value Vec3
---@return number
function Vec3:dot(value) end

---@return Vec3
function Vec3:normalize() end
