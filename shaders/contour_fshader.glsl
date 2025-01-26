#version 330
layout(location = 0) out vec4 fragColor;
uniform bool selected;
void main()
{
    if(selected)
    {
        fragColor = vec4(1,0,0,1);
    }
    else{
        fragColor = vec4(1,1,1,1);
    }

}