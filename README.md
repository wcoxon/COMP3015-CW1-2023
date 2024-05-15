# COMP3015 CW2 report



Shadow Mapping
- 
point light shadowcasting is implemented by rendering the scene depth in 6 faces of a cubemap in the omniDepthShader.geom shader, the omniDepthShader.vert shader passes through vertices to geometry shader primitives, and omniDepthShader.frag renders the fragment depths.

directional light shadowmapping uses the same vert and frag shaders to perform the above describes functions in the pipeline, but uses a different geometry shader since a directional light doesn't cast shadows in all directions, and instead renders depths of each primitive once orthographically to a 2D texture.

these depth textures are then binded to uniforms in the base pass, allowing a fragment to have it's distance from the lights compared to their rendered depths towards it, indicating whether or not the light from each source reaches the current fragment.

Dynamic Tessellation
-
the water mesh is generated using the generatePatches() function in SceneBasic_Uniform.cpp, which procedurally builds the surface from patch quads. this is so that the tessellation level of the water can be localised to a small area on its surface, whilst other patches can have less detailed geometry.

in the tessellation control shader control.tesc, patches are tessellated with more vertices depending on the proximity of the boat and view. in the tessellation evaluation shader eval.tese, the tessellated vertices are given atrributes and displaced to animate the vertices on waves. boat's position and speed is also used to raise the vertices around the boat when it is travelling at speed.

Gaussian Blur
-
the guassian blur effect is processed in the post.frag shader, and this is invoked from C++ in the SceneBasic_Uniform render() method, the guassian blur effect is applied multiple times in a for loop, first rendering horizontally blurred to a back buffer, and then vertically blurring the back buffer and rendering the result the front buffer on each iteration.
the kernel weights are hardcoded into the uniform.

Outlines
-
The outlines effect is implemented in post.frag, in the case 0 of the pass_ID switch, the neighbouring texels are sampled, and gradients are calculated for the changes in depth at each sample. Based on a threshold, the fragment is coloured black if close to an edge.

Gamification
-
by controlling the boat, you must collect 10 of the pink orb as fast as you can and try to beat your personal best displayed in the console after winning.


the gamification is handled in the SceneBasic_Uniform update() method.
the different post effects are performed by rendering to the screen 'quad' after passing a different pass_ID uniform to the postProg program, which distinguishes which effect to apply
the backFBO and backTexture added to SceneBasic_Uniform serve the purpose of allowing the blurring shaders to sample one buffer whilst rendering to the other.

the wireframe mode can be enabled with 1 and disabled with shift+1 


# COMP3015 CW1 2023
 

The C++  / CPU-side

Movement/input
 In the scenerunner main loop function, the exposed window handle is used to check for keyboard and mouse input.
 The horizontal mouse position determines the rotation of the directional light source. The boat and camera can be moved using the keyboard controls, and hotkeys are set up for shader settings such as wireframe and fog.
 The getInputVector and getCameraInputVector functions produce directions based on keys pressed, which are used to control the boat and camera.


Model class
 The Model class contains the handle to the shader program, VAO and VBO buffers, and contains functionality for loading and drawing a mesh
 The Model also stores a material, which contains attributes to be sent to the shader program via uniforms for lighting operations.


initScene
In the initscene method, the models of the scene are initialized, and their textures and vertex attributes are loaded. The skybox is also constructed, point and directional lights and their textures, and then uniforms are sent to each shader.

Update
In the update method, objects in the scene are displaced with the waves, and the white point light is animated in a circle

Render
In the render method, the scene is first rendered to point light cubemaps, then to directional light depth maps, and then using these two renders to test for light occlusion, the base pass can be drawn with each model in the scene.


The shaders / GPU side

Vertex shaders

The water and standard vertex shaders are similar to a pass-through, though the water sends the vertex position through differently to account for displacements later on

The skybox vertex shader calculates the direction of the vertices relative to world space, and then allows these directions to be interpolated for the fragments which sample the skybox cubemap.

Geometry shader

The geometry shader is used to calculate face normals for flat shading, create a TBN matrix for normal mapping, and also compute light for gouraud shading.

Geometry shaders are also used for rendering each primitive in 6 directions for rendering to point light cubemaps faces.


Fragment shaders

Common.frag
The common.frag shader is preloaded other fragment shaders such as waterShader.frag and the standard basic_uniform.frag shaders, so that they can both use the functions defined in common.frag, rather than defining it in each.


lighting
The computeLight function contains calculations for diffuse, specular and ambient light as well as shadowcasting, and returns the colour of reflected light to be output as the fragment colour


Water
displacement
In the tessellation evaluation shader the positions are set using world coordinates for the wave sample, this means that other models in the scene can be displaced based on their world position to sit on the waves
normal map
In the fragment shader the normal map texture is sampled at 2 different positions which are moved in different directions with time to create the rippling effect when lit
texture mixing
The boat position and speed uniforms are used to mix the water texture with a foam texture at a radius and position

walls
 The scene contains 2 walls perpendicular to one another
normal and texture
 The walls use a normal map and colour texture sent to the shaders through uniforms. To combine the normal map samples with the normals of each vertex, a matrix is built to convert the sample from tangent space into world space. 

Gouraud
 In the geometry shader, the lighting can be calculated per vertex if the perFragment value of the material is false. 
 In the scene, the pink ball/buoy is using gouraud shading.

