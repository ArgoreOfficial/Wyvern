
lo::sInputLayoutElement layout[] = {
	lo::sInputLayoutElement{ 3, lo::LO_TYPE_FLOAT, false, 0 },               // position
	lo::sInputLayoutElement{ 3, lo::LO_TYPE_FLOAT, false, 0 },               // colour
	lo::sInputLayoutElement{ 2, lo::LO_TYPE_FLOAT, true, 0 },                // texcoord
};
lo::sInputLayoutInfo layoutinfo;
layoutinfo.elements = layout;
layoutinfo.num_elements = 3;

float vertices[] = {
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f
};

lo::sBufferInfo bufferinfo;
bufferinfo.type = lo::LO_BUFFER_TYPE_VERTEX;
bufferinfo.usage = lo::LO_BUFFER_USAGE_STATIC_DRAW;
bufferinfo.layout = &layoutinfo;
bufferinfo.data = vertices;
bufferinfo.size = sizeof( vertices );

lo::cBuffer* buffer = renderer.createBuffer( &bufferinfo );
