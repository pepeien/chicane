---@meta

---@class Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer
Color = {}

---@param r integer
---@param g integer
---@param b integer
---@param a integer|nil
---@return Color
function Color(r, g, b, a) end

---@param hex string
---@return Color
function Color.fromHex(hex) end

---@return boolean
function Color:isVisible() end
