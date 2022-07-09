#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <array>
#include "input.h"
#include "runner.h"

int run(World& gameWorld, std::string worldConfigFile)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Needed for mac
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
    Input::setWindow(window);
    if (!gameWorld.loadWorld(worldConfigFile)) {
        std::cout << "Error loading world config txt file" << worldConfigFile << std::endl;
        return -1;
    }

    double lastTime = glfwGetTime();
    double deltaTime = 0;
    double currTime = 0;
    double totalTime = 0;
    int frames = 0;
    std::array<float, 50> frameWindow;
    
    for(System* s : gameWorld.getSystems()){
        s->init();
    }
    /* Loop until the user closes the window */
    float dt = 1.0/240;
    Input::resetKeyState();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        totalTime += deltaTime;
        lastTime = currTime;
        frames++; 
        
        frameWindow[frames % frameWindow.size()] = float(deltaTime);
        float totalWindowTime = 0;
        for (auto frameTime : frameWindow) {
            totalWindowTime += frameTime;
        }
        std::cout << "\rFrame Time: " << std::setprecision(3) << float(totalWindowTime / frameWindow.size() * 1000) << "ms" 
                  << "\tFPS: " << std::setprecision(3) << int(frameWindow.size() / totalWindowTime);
        
        for(System* s : gameWorld.getSystems()){
            s->update(dt);
        }     
        glfwSwapBuffers(window);
        gameWorld.destroyEntities();
        Input::resetKeyState();

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

