vec3 UnprojectPoint(vec3 inPoint, mat4 inProjection, mat4 inView) {
    vec4 worldPoint = inverse(inProjection * inView) * vec4(inPoint, 1.0);

    return worldPoint.xyz / worldPoint.w;
}