#include "input.h"
#include <string.h>

namespace Input{

    namespace{
        GLFWwindow* mWindow;
        double lastxpos, lastypos;
        double offsetxpos = 0, offsetypos = 0, offsetScroll = 0;
        int keyState[400] = {-1};
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
            keyState[key] = action;              
        }
        void mouse_callback(GLFWwindow* window, int key, int action, int mods){
            keyState[key] = action;                
        }
        void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos){
            offsetxpos = lastxpos - xpos;
            offsetypos = lastypos - ypos;
            lastxpos = xpos;
            lastypos = ypos;
        }
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
            offsetScroll = yoffset;
        }
    }

    void setWindow(GLFWwindow* window){
        mWindow = window;
        glfwSetKeyCallback(mWindow, key_callback);
        glfwSetMouseButtonCallback(mWindow, mouse_callback);
        glfwSetScrollCallback(mWindow, scroll_callback);
        glfwSetCursorPosCallback(mWindow, cursor_pos_callback);
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(mWindow, &lastxpos, &lastypos);
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
        offsetxpos = 0;
        offsetypos = 0;
        offsetScroll = 0;
    }

    void getMousePos(double* xpos, double* ypos){
        *xpos = lastxpos;
        *ypos = lastypos;
    }

    void getMouseDelta(double* xpos, double* ypos){
        *xpos = offsetxpos;
        *ypos = offsetypos;
    }

    double getScrollDelta(){
        return offsetScroll;
    }
}

