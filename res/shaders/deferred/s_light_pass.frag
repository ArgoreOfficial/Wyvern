#version 430 core

layout (location = 0) out vec4  light_Position;
layout (location = 1) out vec4  light_Normal;
layout (location = 2) out vec4  light_Albedo;
layout (location = 3) out vec4  light_MetallicRoughness;
layout (location = 4) out float light_Depth;
layout (location = 5) out vec4  light_Light;

in vec2 TexCoord;

layout( binding = 0 ) uniform sampler2D gbuffer_Position;
layout( binding = 1 ) uniform sampler2D gbuffer_Normal;
layout( binding = 2 ) uniform sampler2D gbuffer_Albedo;
layout( binding = 3 ) uniform sampler2D gbuffer_MetallicRoughness;
layout( binding = 4 ) uniform sampler2D gbuffer_Depth;

// CONSTS

const int   MAX_LIGHTS      = 100;

const uint  SSS_MAX_STEPS          = 16;
const float g_sss_ray_max_distance = 0.05f;
const float SSS_THICKNESS          = 0.2f;  
const float SSS_STEP_LEN           = g_sss_ray_max_distance / float(SSS_MAX_STEPS);

// UNIFORMS

layout (std140) uniform uboRenderInfo
{
   int   uRenderMode;
   vec3  uDirectionalLight;
   float uAmbientLightIntensity;
   float uDirectionalLightIntensity;
   int   uNumPointLights;
   vec3  uCameraDirection;
   vec3  uCameraPosition;
   mat4  uViewMatrix;
   mat4  uProjMatrix;
   vec4  uLightPos[ MAX_LIGHTS ];
   vec4  uLightCol[ MAX_LIGHTS ];
};



float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 calcSpecular( vec3 _frag_pos, vec3 _normal, vec3 _light_dir, float _dist, vec4 _light_color, float _shininess )
{
    vec3 view_dir    = normalize( uCameraPosition - _frag_pos );
    vec3 halfway_dir = normalize( _light_dir + view_dir );

    float spec      = pow(max(dot(vec3(_normal), halfway_dir), 0.0), _shininess );
    float intensity = (1.0 / pow(_dist, 2));
    return _light_color.xyz * intensity * spec * _light_color.w;
}

vec3 posToScreen( vec3 _pos )
{
    vec4 clip_space_pos = uProjMatrix * ( uViewMatrix * vec4(_pos, 1.0));

    if( clip_space_pos.w != 0 )
        return clip_space_pos.xyz / clip_space_pos.w;

    return vec3( -10, 0, 0 );
}

float linearizeDepth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

float computeScreenSpaceShadows( vec3 _light_dir )
{
    vec3 position = texture( gbuffer_Position, TexCoord ).xyz;
    
    vec3 ray_pos  = position;
    vec3 ray_dir  = _light_dir;
    vec3 ray_step = ray_dir * SSS_STEP_LEN;

    vec2 ray_uv;

    for(int i = 0; i < SSS_MAX_STEPS; i++)
    {
        ray_pos += ray_step;
        ray_uv = posToScreen( ray_pos ).xy;

        ray_uv.x = ray_uv.x * 0.5 + 0.5;
        ray_uv.y = ray_uv.y * 0.5 + 0.5;

        if( ray_uv.x < 0 || ray_uv.x > 1 || 
            ray_uv.y < 0 || ray_uv.y > 1 )
            continue;


        float ray_z = posToScreen( ray_pos ).z;

        float depth_z     = texture( gbuffer_Depth, ray_uv ).r;
        float depth_delta = ray_z - depth_z;
        
        //return depth_z;
        //return -depth_delta;
        //return ray_z;

        if( depth_delta > 0.0f && depth_delta < SSS_THICKNESS )
            return 0;
    }
    
    return 1;

}

void main()
{
	light_Position          = texture( gbuffer_Position,          TexCoord );
	light_Normal            = texture( gbuffer_Normal,            TexCoord );
	light_Albedo            = texture( gbuffer_Albedo,            TexCoord );
	light_MetallicRoughness = texture( gbuffer_MetallicRoughness, TexCoord );
	light_Depth             = texture( gbuffer_Depth,             TexCoord ).r;

	vec4 normal    = texture( gbuffer_Normal,   TexCoord );
    vec4 position  = texture( gbuffer_Position, TexCoord );
    vec4 albedo    = texture( gbuffer_Albedo,   TexCoord );
    vec4 met_rough = texture( gbuffer_MetallicRoughness, TexCoord ); // g:roughness, b:metallic
    
	// directional light
    float dirLight = dot( uDirectionalLight, normal.xyz ) * 0.5 + 0.5;

    // pointlights
    vec4 pointlight_accumulated = vec4( 0.0 );
    vec3 specular = vec3( 0.0 );
    for(int i = 0; i < uNumPointLights; i++)
    {
        vec3 light_to_frag = position.xyz - uLightPos[i].xyz;
        vec3 light_dir     = normalize( uLightPos[i].xyz - position.xyz );
        float dist         = length( light_to_frag );

        float intensity = 1.0 / pow( dist, 2 );

        specular += calcSpecular( position.xyz, normal.xyz, light_dir, dist, uLightCol[ i ], ( 1.0f - met_rough.g ) * 16.0f );

        float light_dot = dot( light_dir, normal.xyz );

        if( light_dot > 0 )
            pointlight_accumulated += uLightCol[ i ] * light_dot * intensity * uLightCol[ i ].w;

    }
    specular += calcSpecular( position.xyz, normal.xyz, uDirectionalLight, 1.0, vec4( uDirectionalLightIntensity ), ( 1.0f - met_rough.g ) * 16.0f ); // directional light specular
    
	// vec3 so that alpha isn't affected by lighting
    vec3 frag_light = vec3( 1.0 ) * ( uAmbientLightIntensity + dirLight ) + pointlight_accumulated.xyz * pointlight_accumulated.w + specular;
	
	light_Light.rgb = frag_light;
	light_Light.a = 1.0;
}