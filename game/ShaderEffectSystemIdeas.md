







## tex_scroll_rgb.wfx
```glsl
#name tex_scroll_rgb
#input Sampler2D _Tex
#input float _X
#input float _Y
#returns vec3

#effect Begin
return Texture2D($_Tex, vec2($_X, $_Y)).rgb;
#effect End
```

## Shader Setup
```cpp
auto common = getEffect( "effects/common_frag.wfx" );
auto scroll = getEffect( "effects/tex_scroll_rgb.wfx" )
scroll.bindInputToProperty( "_X", "g_Time" );
scroll.bindInputToConst( "_Y", "0.0" ); // we don't care about Y

auto pbr = getEffect( "effects/pbr.wfx" )
// use the scrolling texture as diffuse input
pbr.bindInputToEffect( "_ColorDiffuse", scroll );

ShaderFactory factory{ "ScrollingPBR" };
factory.addGlobals(); // u_Time, u_CameraPos, etc.
factory.addEffect( common );
factory.addEffect( scroll );
factory.addEffect( pbr );
ShaderModule shader = factory.compile();

wv::Print( "ScrollingPBR Inputs:" );
wv::Print( shader.getPropertiesStrVec() );
/*

ScrollingPBR Inputs:
[0] vec2 vertex_UV;
[1] Sampler2D tex_scroll_rgb_Tex;
[2] Sampler2D PBR_Normal;
[3] Sampler2D PBR_RM;

*/
```
## Final Shader Pseudocode
```glsl
in vec2 vertex_UV;
in flat sampler2D tex_scroll_rgb_Tex;
in flat sampler2D PBR_Normal;
in flat sampler2D PBR_RM;

vec3 tex_scroll_rgb(_Tex, _X) {
    return Texture2D(_Tex, vec2(_X, 0.0)).rgb;
}

// .. pbr stuff ..

void main() {
    vec3 c_0 = tex_scroll_rgb(tex_scroll_rgb_Tex, g_Time);
    vec3 c_1 = pbr( c_0, PBR_Normal, PBR_RM );
    gl_FragColor = c_1;
}
```

