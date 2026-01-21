#define DEFINE_POSITION_BUFFER() \
	layout(buffer_reference, std430) \
	readonly buffer PositionBuffer{ float positions[]; }

#define DEFINE_VERTEX_BUFFER(_vertexType) \
	layout(buffer_reference, std430) \
	readonly buffer VertexBuffer{ _vertexType vertices[]; }

#define getPosition(_index) vec3(positionBuffer.positions[_index * 3], positionBuffer.positions[_index * 3 + 1], positionBuffer.positions[ _index * 3 + 2])
#define getVertex(_index) vertexBuffer.vertices[_index]