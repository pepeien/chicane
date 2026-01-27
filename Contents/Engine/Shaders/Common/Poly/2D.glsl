struct PolyInstance2D {
    vec2 screen;
    vec2 size;
    vec3 position;
};

vec2 get2DVertexPosition(PolyInstance2D inInstance, vec2 inPosition, vec2 inMultipliers) {
    vec2 position = inPosition;
    position *= inInstance.size;   // Scale vertex to UI size
    position /= inInstance.screen; // Normalize to screen size
    position *= inMultipliers;

    return position;
}

vec3 get2DScreenPosition(PolyInstance2D inInstance, vec2 inMultipliers) {
    vec3 position = inInstance.position;
    position.xy += inInstance.size * 0.5; // Account offset
    position.xy /= inInstance.screen;     // Normalize to screen size
    position.xy *= 2.0;                   // Normalize to NDC
    position.xy -= 1.0;                   // Normalize to NDC
    position.xy *= inMultipliers;
    position.z  /= 999.9;
    position.z   = clamp(abs(position.z - 1.0), 0.0, 1.0);

    return position;
}

vec4 normalize2DColor(vec4 inColor) {
    return inColor / 255.0;
}