#define GLFW_DLL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "world.h"

int main(void)
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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    World gameWorld;
    int baby = gameWorld.createEntity();
    TransformComponent newTC;
    newTC.x = 12;
    newTC.y = 24;
    gameWorld.addComponentToEntity<RenderComponent>(baby);
    gameWorld.addComponentToEntity<TransformComponent>(baby, newTC);
    TransformComponent retTC = gameWorld.getComponent<TransformComponent>(baby);   
    retTC.x = 200;
    TransformComponent retTC5 = gameWorld.getComponent<TransformComponent>(baby);   

    int baby2 = gameWorld.createEntity();
    TransformComponent newTC2;
    newTC2.x = 0;
    newTC2.y = 99;
    gameWorld.addComponentToEntity<TransformComponent>(baby2, newTC2);
    gameWorld.removeComponentFromEntity<TransformComponent>(baby);
    TransformComponent retTC2 = gameWorld.getComponent<TransformComponent>(baby2);   
    gameWorld.destroyEntity(baby2);

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