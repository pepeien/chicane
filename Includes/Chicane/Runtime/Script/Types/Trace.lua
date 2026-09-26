---@meta

---@class Trace
local Trace = {}

---@return Vec3
function Trace:getLocation() end

---@return Vec3
function Trace:getImpact() end

---@return number
function Trace:getDistance() end

---@return Vec3
function Trace:getStart() end

---@return Vec3
function Trace:getEnd() end

---@return Actor|nil
function Trace:getActor() end
