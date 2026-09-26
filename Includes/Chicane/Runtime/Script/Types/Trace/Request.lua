---@meta

---@class TraceRequest
local TraceRequest = {}

---@param origin Vec3
---@param destination Vec3
---@param cellSize number|nil
---@return TraceRequest
function TraceRequest.Line(origin, destination, cellSize) end

---@param origin Vec3
---@param destination Vec3
---@param halfExtentX number
---@param halfExtentY number
---@param cellSize number|nil
---@return TraceRequest
function TraceRequest.Rectangle(origin, destination, halfExtentX, halfExtentY, cellSize) end

---@param origin Vec3
---@param destination Vec3
---@param angle number
---@param cellSize number|nil
---@param segmentCount integer|nil
---@return TraceRequest
function TraceRequest.Cone(origin, destination, angle, cellSize, segmentCount) end

---@param origin Vec3
---@param destination Vec3
---@param radius number
---@param cellSize number|nil
---@param segmentCount integer|nil
---@return TraceRequest
function TraceRequest.Cylinder(origin, destination, radius, cellSize, segmentCount) end

---@return boolean
function TraceRequest:isValid() end

---@return number
function TraceRequest:getLength() end

---@return Vec3
function TraceRequest:getDirection() end

---@return Vec3
function TraceRequest:getOrigin() end

---@param origin Vec3
function TraceRequest:setOrigin(origin) end

---@return Vec3
function TraceRequest:getDestination() end

---@param destination Vec3
function TraceRequest:setDestination(destination) end
