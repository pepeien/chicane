#pragma once

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
			namespace Uniform
			{
				struct BufferObject
                {
                    glm::mat4 view;
                    glm::mat4 projection;
                    glm::mat4 viewProjection;
                };
			}
		}
	}
}