uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(color.r * 0.2, color.g * 0.2, color.b * 0.8, color.r);
}
