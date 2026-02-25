struct PolyInstance2D {
    vec2 view;
    vec2 scale;
    vec2 size;
    vec2 offset;
    vec3 position;
    vec4 color;
    int textureIndex;
};

float normalize2DDepth(float inValue) {
    float result = inValue;
    result /= 999.0;
    result  = clamp(abs(result - 1.0), 0.0, 1.0);

    return result;
}

vec2 get2DVertexPosition(PolyInstance2D inInstance, vec2 inPosition, vec2 inMultipliers) {
    vec2 position = inPosition;
    position *= inInstance.scale;  // Apply vertex scale
    position += inInstance.offset; // Apply vertex offset
    position /= inInstance.view;   // Normalize to view size
    position *= inMultipliers;

    return position;
}

vec3 get2DScreenPosition(PolyInstance2D inInstance, vec2 inMultipliers) {
    vec3 position = inInstance.position;
    position.xy  += inInstance.size * 0.5;     // Convert to top-left origin
    position.xy  /= inInstance.view;           // Normalize to view size
    position.xy   = (position.xy * 2.0) - 1.0; // Normalize to NDC
    position.xy  *= inMultipliers;
    position.z    = normalize2DDepth(position.z);

    return position;
}

vec4 normalize2DColor(vec4 inColor) {
    return inColor / 255.0;
}