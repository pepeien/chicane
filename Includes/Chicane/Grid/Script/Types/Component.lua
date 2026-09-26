---@meta

---@class Component
local Component = {}

---@return string
function Component:getClassName() end

---@param name string
function Component:setClassName(name) end

---@return string
function Component:getText() end

---@param text string
function Component:setText(text) end

---@return boolean
function Component:isVisible() end

---@param visible boolean
function Component:setVisible(visible) end

---@param name string
function Component:invoke(name) end
