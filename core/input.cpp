#include "input.h"

namespace Input{

    namespace{
        GLFWwindow* mWindow;
    }

    void setWindow(GLFWwindow* window){
        mWindow = window;
    }

    int getKey(int keycode){
        if (keycode == GLFW_MOUSE_BUTTON_1 ||
            keycode == GLFW_MOUSE_BUTTON_2 ||
            keycode == GLFW_MOUSE_BUTTON_3)
            return glfwGetMouseButton(mWindow, keycode);
        else
            return glfwGetKey(mWindow, keycode);
    }
    void getMousePos(double* xpos, double* ypos){
        glfwGetCursorPos(mWindow, xpos, ypos);
    }
}

