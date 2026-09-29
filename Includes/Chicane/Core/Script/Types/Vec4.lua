---@meta

---@class Vec4
---@field x number
---@field y number
---@field z number
---@field w number
Vec4 = {}

---@param x number
---@param y number
---@param z number
---@param w number
---@return Vec4
function Vec4(x, y, z, w) end

---@return Vec4
function Vec4.Zero() end

---@return Vec4
function Vec4.One() end

---@return Vec4
function Vec4.Right() end

---@return Vec4
function Vec4.Forward() end

---@return Vec4
function Vec4.Up() end

---@return Vec4
function Vec4.Sentinel() end

---@return string
function Vec4:toString() end
