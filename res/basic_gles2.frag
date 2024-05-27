precision mediump float;

uniform vec3 u_Col;

void main()
{
    gl_FragColor = vec4( u_Col, 1.0 );
} 