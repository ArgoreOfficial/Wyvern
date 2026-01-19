#define defineVertexBuffer(_vertexType) \
	layout(buffer_reference, std430) \
	readonly buffer VertexBuffer{ _vertexType vertices[]; };

#define getVertex(_index) vertexBuffer.vertices[_index]