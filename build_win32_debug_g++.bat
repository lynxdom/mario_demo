g++ -o .\bin\mario_demo ^
	.\src\main.cpp^
	.\src\ShaderTemplate.cpp^
	.\src\ImageFileReader.cpp^
	-g^
	-I..\library\win32\GLFW\include^
	-I..\library\win32\glm\include^
	-I..\library\win32\glew\include^
	-L..\library\win32\glew\lib^
	-L..\library\win32\GLFW\lib^
	-lglfw3 -lglew32 -lgdi32 -lopengl32