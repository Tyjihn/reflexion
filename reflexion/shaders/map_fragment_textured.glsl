uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(color.r * 0.91, color.g * 0.9, color.b * 1.2, 0.95);
}
