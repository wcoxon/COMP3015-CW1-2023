# COMP3015 CW1 2023
 


The C++  / CPU-side

Movement/input
In the scenerunner main loop function, the exposed window handle is used to check for keyboard and mouse input.
The horizontal mouse position determines the rotation of the directional light source. The boat and camera can be moved using the keyboard controls, and hotkeys are set up for shader settings such as wireframe and fog.

The getInputVector and getCameraInputVector build a normalized direction based on keys pressed, which are used to control the boat and camera.


model class
The model class contains the handle to the shader program, VAO and VBO buffers, and contains functionality for loading and drawing a mesh

The model also stores a material, which contains attributes to be sent to the shader program via uniforms for lighting operations.


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


video:
https://www.youtube.com/watch?v=IwkkAI-7tA8
