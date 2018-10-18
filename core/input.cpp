#include "input.h"
#include <string.h>

namespace Input{

    namespace{
        GLFWwindow* mWindow;
        int keyState[400] = {-1};
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
            keyState[key] = action;              
        }
        void mouse_callback(GLFWwindow* window, int key, int action, int mods){
            keyState[key] = action;                
        }
    }

    void setWindow(GLFWwindow* window){
        mWindow = window;
        glfwSetKeyCallback(mWindow, key_callback);
        glfwSetMouseButtonCallback(mWindow, mouse_callback);
    }

    bool getKey(int keycode){
        return glfwGetKey(mWindow, keycode) ;
    }

    bool getKeyDown(int keycode){
        return keyState[keycode] == 1;
    }

    bool getKeyUp(int keycode){
        return keyState[keycode] == 0;
    }

    bool getMouseButton(int keycode){
        return glfwGetMouseButton(mWindow, keycode);
    }

    bool getMouseButtonDown(int keycode){
        return keyState[keycode] == 1;        
    }

    bool getMouseButtonUp(int keycode){
        return keyState[keycode] == 0;
    }

    void resetKeyState(){
        memset(keyState, -1, sizeof(keyState));
    }

    void getMousePos(double* xpos, double* ypos){
        glfwGetCursorPos(mWindow, xpos, ypos);
    }
}

