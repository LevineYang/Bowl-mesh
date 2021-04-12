#version 330 core

in vec2 textCoord;

out vec4 FragColor;

uniform sampler2D useTexture;


void main()
{ 
    //FragColor = vec4(1.f, 0.5f, 0.2f, 1.f);
    FragColor = texture(useTexture, textCoord);
}