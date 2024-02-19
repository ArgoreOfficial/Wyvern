#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

const int NUM_LIGHTS = 1000;

layout (std140) uniform uboRenderInfo
{
   int   uRenderMode;
   vec3  uDirectionalLight;
   float uAmbientLightIntensity;
   float uDirectionalLightIntensity;
   int   uNumPointLights;
   vec3  uCameraDirection;
   vec3  uCameraPosition;
   vec4  uLightPos[ NUM_LIGHTS ];
   vec4  uLightCol[ NUM_LIGHTS ];
};

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    vec4 normal   = texture( gNormal, TexCoord );
    vec4 position = texture( gPosition, TexCoord );
    vec4 albedo   = texture( gAlbedo, TexCoord );

	// directional light
    float dirLight = dot( uDirectionalLight, vec3(normal) ) * 0.5 + 0.5;

    // pointlights
    vec4 pointlight_accumulated = vec4(0,0,0,0);
    for(int i = 0; i < uNumPointLights; i++)
    {
        vec3 relative = vec3( position ) - vec3( uLightPos[ i ] );
        
        float dist = length( vec3( position ) - vec3( uLightPos[ i ] ) );

        if( dist < 1.0 )
            pointlight_accumulated += (1.0 - dist) * uLightCol[ i ];
    }

    // specular
    vec3 vertex_to_eye = normalize(uCameraPosition - vec3(position));
    vec3 dirlight_reflect = normalize( reflect( -uDirectionalLight, vec3(normal) ) );
    float specular = dot( vertex_to_eye, dirlight_reflect );
    if( specular > 0 )
        specular = pow( specular, 32 ) * 0.8;
    else
        specular = 0;

	// vec3 so that alpha isn't affected by lighting
    vec3 frag_light = vec3( 1.0, 1.0, 1.0) * ( uAmbientLightIntensity + dirLight + specular ) + vec3( pointlight_accumulated );
	vec3 surface_color = vec3(albedo) * frag_light;
	
	// render modes
    if     ( uRenderMode == 1 ) FragColor = vec4( surface_color, albedo.w );
    else if( uRenderMode == 2 ) FragColor = normal;
    else if( uRenderMode == 3 ) FragColor = position;
    else if( uRenderMode == 4 ) FragColor = albedo;
    else if( uRenderMode == 5 ) FragColor = vec4( frag_light, 1.0 );

}