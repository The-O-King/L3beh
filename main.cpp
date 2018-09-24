#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <iostream>
#include "world.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    World gameWorld;
    double lastTime = glfwGetTime();
    double deltaTime = 0;
    double currTime = 0;
    double totalTime = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        totalTime += deltaTime;
        lastTime = currTime;

        for(System s : gameWorld.getSystems()){
            std::cout << deltaTime << std::endl;
            s.update(deltaTime);
        }
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}