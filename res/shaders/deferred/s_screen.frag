#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

const int MAX_LIGHTS = 100;
const float GAMMA = 2.2;
const float EXPOSURE = 1.0;


layout (std140) uniform uboRenderInfo
{
   int   uRenderMode;
   vec3  uDirectionalLight;
   float uAmbientLightIntensity;
   float uDirectionalLightIntensity;
   int   uNumPointLights;
   vec3  uCameraDirection;
   vec3  uCameraPosition;
   vec4  uLightPos[ MAX_LIGHTS ];
   vec4  uLightCol[ MAX_LIGHTS ];
};



float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 calcSpecular( vec3 _frag_pos, vec3 _normal, vec3 _light_dir, vec4 _light_color )
{
    vec3 view_dir      = normalize( uCameraPosition - _frag_pos );
    vec3 halfway_dir   = normalize( _light_dir + view_dir );

    float shininess = 20;
    float spec = pow(max(dot(vec3(_normal), halfway_dir), 0.0), shininess );
    return _light_color.xyz * spec * _light_color.w;
}

void main()
{

    vec4 normal   = texture( gNormal, TexCoord );
    vec4 position = texture( gPosition, TexCoord );
    vec4 albedo = texture( gAlbedo, TexCoord );
    albedo.xyz = pow( albedo.xyz, vec3( GAMMA ) );
    
	// directional light
    float dirLight = dot( uDirectionalLight, normal.xyz ) * 0.5 + 0.5;

    // pointlights
    vec4 pointlight_accumulated = vec4( 0.0 );
    vec3 specular = vec3( 0.0 );

    // specular
    for(int i = 0; i < uNumPointLights; i++)
    {
        vec3 light_to_frag = position.xyz - uLightPos[i].xyz;
        vec3 light_dir     = normalize( uLightPos[i].xyz - position.xyz );
        
        specular += calcSpecular( position.xyz, normal.xyz, light_dir, uLightCol[ i ] );

        float dist = length( light_to_frag );
        if( dist < 1.0 )
            pointlight_accumulated += (1.0 - dist) * uLightCol[ i ];
    }
    specular += calcSpecular( position.xyz, normal.xyz, uDirectionalLight, vec4( uDirectionalLightIntensity ) ); // directional light specular
    
	// vec3 so that alpha isn't affected by lighting
    vec3 frag_light = vec3( 1.0 ) * ( uAmbientLightIntensity + dirLight ) + pointlight_accumulated.xyz * pointlight_accumulated.w + specular;
	vec3 surface_color = albedo.xyz * frag_light;
	
	// render modes
    if ( uRenderMode == 1 ) 
    {
        vec3 mapped = vec3(1.0) - exp(-surface_color * EXPOSURE); // reinhard tone mapping
        mapped = pow( mapped, vec3( 1.0 / GAMMA ) );                  // gamma correction 
        FragColor = vec4( mapped, 1.0 );
    }
    else if( uRenderMode == 2 ) FragColor = normal;
    else if( uRenderMode == 3 ) FragColor = position;
    else if( uRenderMode == 4 ) // albedo
    {
        vec3 mapped = pow( albedo.xyz, vec3( 1.0 / GAMMA ) ); // gamma correction 
        FragColor = vec4( mapped, 1.0 );
    }
    else if( uRenderMode == 5 ) // light
    {
        vec3 mapped = pow( frag_light, vec3( 1.0 / GAMMA ) ); // gamma correction 
        FragColor = vec4( mapped, 1.0 );
    }

}