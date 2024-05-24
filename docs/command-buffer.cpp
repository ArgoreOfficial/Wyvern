/* ... */
class cBuffer;
struct sPipelineStateDesc;
class cPipelineState;
struct sCommandBufferDesc;

sPipelineStateDesc ps_desc;
ps_desc.type         = LO_PRIMITIVE_TRIANGLES;
ps_desc.vertexBufferDesc = &vb_desc;
ps_desc.indexBufferDesc  = nullptr;
ps_desc.hasAlpha = false;
ps_desc.program = &default_shader;
ps_desc.cullMode = LO_CULLMODE_NONE;

cPipelineState triangle = device->createPipelineState( &ps_desc );

/* create command buffer state */
sCommandBufferStateDesc cbs_desc;
cbs_desc.type = LO_COMMAND_BUFFER_STATE_DYNAMIC; /* command buffer will be updated every frame */
cbs_desc.clear_mode = LO_COMMAND_BUFFER_CLEAR_RGBA;
cbs_desc.swap_mode  = LO_COMMAND_BUFFER_SWAP;
cbs_desc.sort_mode  = LO_COMMAND_BUFFER_SORT_NONE;
device->setCommandBufferState( cbs_desc );


/* triangle draw command */
sCommandDesc triangle_cmd_desc;
triangle_cmd_desc.callback = nullptr; /* no uniforms, so no need for a callback */
triangle_cmd_desc.state = &triangle;
sCommand triangle_cmd = device->createCommand( triangle_cmd_desc );

sUniformBuffer* cubeCallback( sUniformBuffer* _uniform_buffer );

/* cube draw command */
sCommandDesc cube_cmd_desc;
cube_cmd_desc.callback = cubeCallback;
cube_cmd_desc.state = &cube;
sCommand cube_cmd = device->createCommand( cube_cmd_desc );





// update


/* create command buffer */
sCommand commands[] = {
    triangle_cmd,
    cube_cmd
};

/* dispatch command buffer */
sCommandBufferDesc cb_desc;
cb_desc.commands = commands;
cb_desc.numCommands = 2;
device->dispatchCommandBuffer( cb_desc );

/*



*/






