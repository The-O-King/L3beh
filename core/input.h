#ifndef INPUT_H
#define INPUT_H

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

namespace Input{
    void setWindow(GLFWwindow* window);
    bool getKey(int keycode);
    bool getKeyDown(int keycode);
    bool getKeyUp(int keycode);
    bool getMouseButton(int keycode);
    bool getMouseButtonDown(int keycode);
    bool getMouseButtonUp(int keycode);
    void getMousePos(double* xpos, double* ypos);
    void getMouseDelta(double* xpos, double* ypos);
    double getScrollDelta();
    void resetKeyState();
}

#endif