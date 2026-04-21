#define DEFINE_POSITION_BUFFER() \
	layout(buffer_reference, std430) \
	readonly buffer PositionBuffer{ float positions[]; }

#define DEFINE_VERTEX_BUFFER(_vertexType) \
	layout(buffer_reference, std430) \
	readonly buffer VertexBuffer{ _vertexType vertices[]; }

#define DEFINE_MATERIAL_BUFFER(_elementType) \
	layout(buffer_reference, std430) \
	readonly buffer MaterialBuffer{ _elementType materials[]; }

#define getPosition(_index) vec3(positionBuffer.positions[_index * 3], positionBuffer.positions[_index * 3 + 1], positionBuffer.positions[ _index * 3 + 2])
#define getVertex(_index) vertexBuffer.vertices[_index]
#define getMaterial(_index) materialBuffer.materials[_index]

#define unpackFloat2(_value) vec2(_value[0], _value[1])
#define unpackFloat3(_value) vec3(_value[0], _value[1], _value[2])