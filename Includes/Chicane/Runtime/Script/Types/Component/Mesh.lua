---@meta

---@class CMesh : Component
---@field mesh string
local CMesh = {}

---@param path string
function CMesh:setMesh(path) end

---@return boolean
function CMesh:hasMesh() end

---@param id string
function CMesh:playAnimation(id) end

---@param id string
function CMesh:queueAnimation(id) end

function CMesh:stopAnimation() end

---@return boolean
function CMesh:isPlayingAnimation() end

---@return string
function CMesh:getPlayingAnimation() end

---@param value boolean
function CMesh:setCanCastShadows(value) end

---@param value boolean
function CMesh:setIsLit(value) end
