# Cave Generation System
Cave generation using cellular automata. Users can change multiple traits of the cave; the x width, z width, rock density, wireframe, flat mode, and generate by seed.

## How does the user interact with the executable?
Simply run the CaveGenerationSystem.exe file and the application will start. Use the 'W', 'S', 'A', and 'D' keys to move about the world, and move the mouse to aim the camera. Press 'F1' or '`'/'¬' to enter debug mode and change the cave system characteristics. Press 'ESC' to exit the program. 

![](https://media.giphy.com/media/S7d36xtgMRAlUGD40T/giphy.gif)

![](https://media.giphy.com/media/gnRF8sKGEearZRkk2J/giphy.gif)

## How does the program code work?
The 'Project.cpp' file is where the application starts. The three main objects that constitute the cave are 'caveWalls', 'caveCeiling', and 'caveFloor'. These three objects take the vertices generated and process them using buffer objects and array objects. The 'CaveGenerator' and 'MeshGenerator' classes are where the cellular automata algorithm and vertex generation happens. CaveGenerator created a 2D vector of integers representing walls and blank space. MeshGenerator then takes this 2D vector and creates vertices that OpenGL can use. VBO, VAO, Texture, and Shader classes are all used to ecnapsulate OpenGL processes that are used several times throughout the runtime of the application. 

## How does this program compare to other software?
This application can best be described as a prototype cave mesh generator. It can be used to visualise the cellular automata algorithm implemented. This cave system can be taken and used for games that find it appropriate to use such a system. 

## Video explanation
https://youtu.be/egxfYMwBVD0

## What I started with
 - [LearnOpenGL](https://learnopengl.com/)
 - [Sebastian Lague](https://www.youtube.com/c/SebastianLague)
