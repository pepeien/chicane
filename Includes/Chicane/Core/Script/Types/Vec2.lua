---@meta

---@class Vec2
---@field x number
---@field y number
Vec2 = {}

---@param x number
---@param y number
---@return Vec2
function Vec2(x, y) end

---@return Vec2
function Vec2.Zero() end

---@return Vec2
function Vec2.One() end

---@return Vec2
function Vec2.Right() end

---@return Vec2
function Vec2.Up() end

---@return string
function Vec2:toString() end
