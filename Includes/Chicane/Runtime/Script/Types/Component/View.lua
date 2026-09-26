---@meta

---@class CView : Component
local CView = {}

---@return number
function CView:getFieldOfView() end

---@param fov number
function CView:setFieldOfView(fov) end

---@return number
function CView:getAspectRatio() end

---@return number
function CView:getNearClip() end

---@return number
function CView:getFarClip() end

---@param nearClip number
---@param farClip number
function CView:setClip(nearClip, farClip) end

---@return Vec3
function CView:getFocusPoint() end

---@param x number|Vec3
---@param y number|nil
---@param z number|nil
function CView:setFocusPoint(x, y, z) end

---@return string
function CView:getTarget() end

---@param target string
function CView:setTarget(target) end
