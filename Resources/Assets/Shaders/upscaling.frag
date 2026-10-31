uniform sampler2D texture;
uniform vec2 textureSize;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec2 pixel = uv * textureSize;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = max(fwidth(pixel), vec2(1e-5));


    pixel = seam + clamp((pixel - seam) / dudv, -0.5, 0.5);

    vec2 finalUv = pixel / textureSize;
    vec4 color = gl_Color * texture2D(texture, finalUv);
    color.a = step(0.5, color.a);

    gl_FragColor = color;
}