#ifndef RUNNER_H
#define RUNNER_H

#define GLFW_DLL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "world.h"
#include "input.h"

inline int run(World& gameWorld, std::string worldConfigFile)
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

    Input::setWindow(window);
    gameWorld.baseWorldGen(worldConfigFile);
    double lastTime = glfwGetTime();
    double deltaTime = 0;
    double currTime = 0;
    double totalTime = 0;
    
    for(System* s : gameWorld.getSystems()){
        s->init();
    }
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        totalTime += deltaTime;
        lastTime = currTime;
        std::cout << deltaTime << std::endl;
        
        for(System* s : gameWorld.getSystems()){
            s->update(deltaTime);
        }
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif