#ifndef CAMERA_H
#define CAMERA_H

#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// possible options for camera movement.
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float ROTATION_Y  = -90.0f; // clockwise rotation on axis y (PITCH) -- angle that depicts how much we're looking up or down
const float ROTATION_X  =  0.0f; // clockwise rotation on axis x (YAW) -- represents the magnitude we're looking to the left or to the right
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles (rotation_x, rotation_y, rotation_z), Vectors and Matrices for use in OpenGL
class Camera {

public:

    // Camera Attributes
    glm::vec3 Position; // camera's position
    glm::vec3 Front; // direction it's looking at
    glm::vec3 Up; // vector pointing upwards from the camera
    glm::vec3 Right; // a vector pointing to the right
    glm::vec3 WorldUp; // camera position in world space

    // Euler Angles
    float Rotation_y; // yaw
    float Rotation_x; // pitch

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    // it passes some default values that can be override
    // using a member initializer list - we initialize this values
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float rotat_y = ROTATION_Y, float rotat_x = ROTATION_X) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Rotation_y = rotat_y;
        Rotation_x = rotat_x;
        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float rotat_y, float rotat_x) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Rotation_y = rotat_y;
        Rotation_x = rotat_x;
        updateCameraVectors();
    }

    // Returns the view matrix calculated with LookAt Matrix
    glm::mat4 GetViewMatrix() {
        // Position, Direction, Up (of Global World)

        // you instruct glm to create a matrix that will look from cameraPos, at cameraPos + cameraFront, so just in front of the camera. 
        // The trick here is that you move both the position and target at the same time, with the same offset, as the second parameter 
        // is based on the first parameter. This is why the camera does not 'rotate', it has the effect of 'strafing' in First person shooter games.
        return glm::lookAt(Position, Position + Front, Up);
    }

    // ------------------- KEYBOARD ---------------------------
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // ------------------- MOUSE ---------------------------
    // To look around the scene we have to change the cameraFront vector based on the input of the mouse.

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainRotation_x = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Rotation_y += xoffset;
        Rotation_x += yoffset;

        // Make sure that when rotat_x is out of bounds, screen doesn't get flipped
        if (constrainRotation_x) {
            if (Rotation_x > 89.0f)
                Rotation_x = 89.0f;
            if (Rotation_x < -89.0f)
                Rotation_x = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:

    // Calculates the front vector (direction) from the Camera's (updated) Euler Angles
    void updateCameraVectors() {

        // Calculate the new Front vector --> direction of camera
        glm::vec3 front;

        /** 
            for pitch we are in xz plane, x, y and z depends on pitch
            direction x = cos(glm::radians(Rotation_x))
            direction y = sin(glm::radians(Rotation_x))
            direction z = cos(glm::radians(Rotation_x))

            y do not depend from yaw, instead x and z yes:
            direction x = cos(glm::radians(Rotation_y))
            direction z = cos(glm::radians(Rotation_y))
            then, adding them together:
        */
        front.x = cos(glm::radians(Rotation_y)) * cos(glm::radians(Rotation_x));
        front.y = sin(glm::radians(Rotation_x));
        front.z = sin(glm::radians(Rotation_y)) * cos(glm::radians(Rotation_x));

        Front = glm::normalize(front);

        // Also re-calculate the Right and Up vector
        // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Right = glm::normalize(glm::cross(Front, WorldUp)); 
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif