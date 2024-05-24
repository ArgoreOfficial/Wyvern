
# OpenGL Renderer 

### Pipelines
---
**Deferred**
* Sort Objects
* Iterate Objects:
    * Render Data (Depth, Albedo, Normal, Position) onto G-Buffer
* Render G-Buffer through Lightpass shader onto Back Buffer
* Swap Buffers

i.e
- Sort
- Render
- Light Postpass
- Swap
---
**Forward**
* Sort Objects
* Iterate Objects:
    * Iterate Lights:
        * Accumulate Light
    * Render Albedo × Accumulated Light onto Back Buffer
* Swap Buffers

i.e
- Sort
- Render
- Swap
---
**Forward+**
* Sort Objects
* Iterate Objects:
    * Render Depth Prepass
* Iterate Prepass Chunks:
    * Cull Light Intersections
* Iterate Objects:
    * Iterate Culled Lights:
        * Accumulate Light
    * Render Albedo × Accumulated Light onto Back Buffer
* Swap Buffers

i.e
- Sort
- Object Prepass
- Light Prepass
- Render
- Swap
---
**Raytracing**
* Iterate Pixels:
    * Traverse to Ray Intersection:
        * Iterate Lights:
            * Traverse to Light
            * Hit?
                * Accumulate Light + Albedo
    * Render Accumulated Color onto Back Buffer
* Swap Buffers

- Render (single screen shader)
- Swap
---

So the steps are
- Sort (optional)
- Prepasses (optional)
- Render
- Postpasses (optional)
- Swap (optional if rendering to a texture)


### Renderer Configuration Initial Idea
```cpp
sRenderPassInfo light_pass_info;
light_pass_info.type = RenderLightPassType_Shader;
light_pass_info.source = "res/shaders/deferred_light.glsl";

sRenderPipelineInfo pipeline_info;
pipeline_info.pre = nullptr;
pipeline_info.post = &light_pass;
pipeline_info.post_count = 1;

sRendererInfo renderer_info;
renderer_info.sort = true;
renderer_info.pipeline = pipeline_info;

cRenderer renderer{ renderer_info };
```

### Weekly Goals

Week 1:
* basic premake project setup, both PC and WebAssembly
* ultra basic rendering pipeline setup