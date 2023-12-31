<h1>Comp 3016 course work 2 github page</h1>
<h2></h2>
youtube video: https://youtu.be/XoH1ISV4niU<br>
Github: https://github.com/Spenu12994/3016CW2.git<br>

<h2>What I started with</h2><br>
Starting with an adapted version of lab 5, the main was built up from the solution code for model loading and adapted for better model loading, as well as the rest of the application. 

<h2>Application Contents</h2>
<h3>Theme</h3>
The theme for this project was to generate a simple forest using procedural generation, this application uses perlin noise to generate a small world, populated with foliage and rocks for the user to browse. <br>

<h3>Executable Usage</h3>

![image](https://github.com/Spenu12994/3016CW2/assets/91668500/f3ca3644-b84b-4487-9c34-eb22d6f205d0)<br>

The executable zip file comes as above, starting the app can simply be done by opening Project1.exe.
To generate a new random world, close and reopen the application.

<h3>Moving Pieces</h3>
Our main 4 working components are the following:

<h4>Main</h4>
Our main function handles most of the application, staring our opengl window and keeping our game loop active.
Object/Mesh locations, transformations, animations, world generation and more are all managed within the main.cpp file

<h4>Model.h (model Loader)</h4>
Our model loader (adapted from learnOpenGl's model loader) allows us to load in Obj's aswell as supporting textures, and texture maps.
Fetching Files, mapping and mixing textures, and model drawing are all handled within our model class.

<h4>FastNoiseLite (Noise Generator)</h4>
Our FastNoiseList file (from the module labs sessions) handle the perlin noise generation for our procedural noise generation.

<h4>Shaders</h4>
2 sets of vertex and fragment shaders for our general shading and for our terrain shading handle the rendering shaders for our objects. 
the first set of shaders (vertex and fragment) handle our pbr lighting, model shading and most shading for the application.
the second set of shaders(terrainFrag, terrainVert) are made to specifically handle our colour data for our randomly generated vertices and indices, allowing us to uniquely colour biomes.

<h3>Gameplay</h3>
The game is controlled with the following:<br>
WASD - Movement<br>
Mouse - Aim<br>
R and F - Adjust Signiture Model attributes (animation speed and scale);<br>
Esc - Quit<br>
<br>
<h3>Exception Handling</h3>
Our model loading searches for 

![image](https://github.com/Spenu12994/3016CW2/assets/91668500/bc24a00d-6279-4754-be38-89ea22eac34b)<br>

![image](https://github.com/Spenu12994/3016CW2/assets/91668500/13861b20-c7fc-4e74-b197-cf259c7b25bf)<br>


<h3>Inspirations</h3>
as this application is very close to a software demo, not much creative freedom was employed. As such inspiration came in the form of other openGL applications.<br>

The guides on learnOpenGl gave a great understanding of the workings of various bits of code, and these deep understandings allowed for further experimentation and adaption of code that i couldn't even comprehend at the start of this project.<br>

https://learnopengl.com/<br>

This whole playlist was a great inspiration to see how a world is grown from pieces in OpenGL to eventually form a game:<br>
https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=15<br>
![image](https://github.com/Spenu12994/3016CW2/assets/91668500/067ae517-be93-41da-b2b8-6f9c847b39dd)<br>

Generated Terrain:<br>
https://www.gamedev.net/blogs/entry/2276775-a-small-walk-inside-my-procedurally-generated-terrain/ <br>
https://opengl-notes.readthedocs.io/en/latest/topics/complex-objects/procedural-generation.html<br>
https://github.com/stanfortonski/Procedural-Terrain-Generator-OpenGL/tree/master <br>





resources used:

Comp 3016 lab
LearnOpenGL (Was a great tool for learning the page and alot of the code was either inspired or adapted from the lessons on this page)

various youtube videos that are either tutorial or inspiration for features:

https://www.youtube.com/watch?v=AB_f4slL2H0&t=21s&pp=ygUHb3BlbiBnbA%3D%3D
https://www.youtube.com/watch?v=HI3cMe0UFLA&t=17s
https://www.youtube.com/watch?v=l6PEfzQVpvM&t=18s
https://www.youtube.com/watch?v=dswvOhBuEQY&t=62s
https://www.youtube.com/watch?v=GovbphOagoQ&t=387s
https://www.youtube.com/watch?v=7D8lLbp9_rQ&t=548s
https://www.youtube.com/watch?v=-reE9Y81E-c&t=285s
https://www.youtube.com/watch?v=FFxoQlUPca8&t=145s
https://www.youtube.com/watch?v=CpEfHhSpQrw&t=1238s
https://www.youtube.com/watch?v=W_Ey_YPUjMk&t=160s
https://www.youtube.com/watch?v=sP_kiODC25Q&t=1258s

Stack overflow for error solving(some examples):
https://stackoverflow.com/questions/39377679/scaling-and-moving-3d-object-with-opengl

Textures:
https://images.pexels.com/photos/326311/pexels-photo-326311.jpeg?cs=srgb&dl=pexels-pixabay-326311.jpg&fm=jpg
