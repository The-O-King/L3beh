#ifndef INPUT_H
#define INPUT_H

#define GLFW_DLL
#include <GLFW/glfw3.h>

namespace Input{
    void setWindow(GLFWwindow* window);
    int getKey(int keycode);
    void getMousePos(double* xpos, double* ypos);
}

#endif