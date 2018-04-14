
// #ifndef TRACKBALL_H
// #define TRACKBALL_H
// #include "Base.h"
// #include "glm/ext.hpp" // debug
// /**
//     https://www.cs.unm.edu/~angel/CS433.S05/LECTURES/AngelCG15.pdf
//     These functions implement a simple trackball-like motion control.
//  */
// #define bool int /* if system does not support bool type */
// #define false 0
// #define true 1

// #ifndef M_PI
// #define M_PI 3.14159
// #endif

// class Trackball{
//     int window_width, window_height;

//     float angle = 0.0f;
//     Point3d axis;

//     bool tracking_mouse = false;
//     bool redraw_continue = false;
//     bool trackball_move = false;
//     bool start = true;

//     Point3d last_position = Point3d(0.0f, 0.0f, 0.0f);

//     int curx, cury;
//     int startX, startY;

//     int event_mouse;

//     public:
//         Trackball(int w, int h){
//             window_width = w;
//             window_height = h;
//         }

//         // reversion projection
//         void trackball_projection(int x, int y, int width, int height, Point3d v){
//             float d, a;

//             /* project x,y onto a hemisphere centered
//             within width, height , note z is up here*/
//             v.x() = (2.0f * x - width) / width;
//             v.y() = (height - 2.0f * y) / height;
//             d = sqrt(v.x() * v.x() + v.y() * v.y());
//             v.z() = cos((M_PI / 2.0f) * ((d < 1.0) ? d : 1.0f));
//             a = 1.0f / sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
//             v.x() *= a;
//             v.y() *= a;
//             v.z() *= a;
//         }

//         // // compute movement mouse
//         // glm::mat4 mouse_motion(int x, int y) {
//         //     float dx, dy, dz;

//         //     /* compute position on hemisphere */
//         //     Point3d current_position = trackball_projection(x, y, window_width, window_height);

//             // if(tracking_mouse) {
//             //     /* compute the change in position
//             //     on the hemisphere */
//             //     dx = current_position.x() - last_position.x();
//             //     dy = current_position.y() - last_position.y();
//             //     dz = current_position.z() - last_position.z();

//             //     if (dx || dy || dz) {
//             //         /* compute theta and cross product */
//             //         angle = 90.0f * sqrt(dx * dx + dy * dy + dz * dz);
//             //         axis.x() = (last_position.y() * current_position.z()) – (last_position.z() * current_position.y());
//             //         axis.y() = (last_position.z() * current_position.x()) – (last_position.x() * current_position.z());
//             //         axis.z() = (last_position.x() * current_position.y()) – (last_position.y() * current_position.x());

//             //         /* update position */
//             //         last_position.x() = current_position.x();
//             //         last_position.y() = current_position.y();
//             //         last_position.z() = current_position.z();
//             //     }
//             // }
//         //     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//         //     if (trackball_move)
//         //         return glm::rotate(glm::degrees(angle), glm::vec3(axis.x(), axis.y(), axis.z()));
//         //     return;
//         // }

//         // -------- IDLE AND DISPLAY CALLBACKS ----------------
//         // function to rotate object
//         glm::mat4 spin_object() {
//             if (redraw_continue){
//                 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//                 if (trackball_move)
//                     return glm::rotate(glm::degrees(angle), glm::vec3(axis.x(), axis.y(), axis.z()));
//                 return;
//             }
//             return;
//         }

//         // mouse callback
//         // void mouse_btn_callback(GLFWwindow * window, int button, int action, int mods){
//         //      event_mouse = ( action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT );
//         // }

//         // // Function that take position when arcball is activate (left button is pressed)
//         // void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
//         //     if(event_mouse == 0) // nothing pressed or released - no track
//         //         stop_motion(xpos, ypos);
//         //     else { // btn pressed
//         //         ypos = window_height - ypos;
//         //         start_motion(xpos, ypos);
//         //         event_mouse = 2;
//         //     }
//         // }

//         // // mouse callback
//         // void mouse_button(int button, int state, int x, int y) {
//         //     if(button == GLUT_RIGHT_BUTTON) exit(0);

//         //     /* holding down left button
//         //     allows user to rotate cube */
//         //     if(button == GLUT_LEFT_BUTTON) switch(state) {
//         //         case GLUT_DOWN:
//         //             y = window_height - y;
//         //             start_motion(x,y);
//         //             break;
//         //         case GLUT_UP:
//         //             stop_motion( x,y);
//         //             break;
//         //     }
//         // }

//         // start function
//         void start_motion(int x, int y) {
//             tracking_mouse = true;
//             redraw_continue = false;
//             startX = x;
//             startY = y;
//             curx = x;
//             cury = y;
//             trackball_projection(x, y, window_width, window_height, last_position);
//             trackball_move = true;
//         }

//         // stop function
//         void stop_motion(int x, int y) {
//             Point3d current_position;
//             float dx, dy, dz;

//             trackball_projection(x, y, window_width, window_height, current_position);
//                 /* compute the change in position
//                 on the hemisphere */
//                 dx = current_position.x() - last_position.x();
//                 dy = current_position.y() - last_position.y();
//                 dz = current_position.z() - last_position.z();

//                 if (dx || dy || dz) {
//                     /* compute theta and cross product */
//                     angle = 90.0f * sqrt(dx * dx + dy * dy + dz * dz);
//                     // axis.x() = (last_position.y() * current_position.z()) – (last_position.z() * current_position.y());
//                     // axis.y() = (last_position.z() * current_position.x()) – (last_position.x() * current_position.z());
//                     // axis.z() = (last_position.x() * current_position.y()) – (last_position.y() * current_position.x());

//                     axis.x() = current_position.y() * last_position.z() - current_position.z() * last_position.y();
//                     axis.y() = current_position.z() * last_position.x() - current_position.x() * last_position.z();
//                     axis.z() = current_position.x() * last_position.y() - current_position.y() * last_position.x();

//                     /* update position */
//                     last_position.x() = current_position.x();
//                     last_position.y() = current_position.y();
//                     last_position.z() = current_position.z();
//                 }



//             // tracking_mouse = false;

//             // /* check if position has changed */
//             // if (startX != x || startY != y)
//             //     redraw_continue = true;
//             // else {
//             //     angle = 0.0f;
//             //     redraw_continue = false;
//             //     trackball_move = false;
//             // }
//         }

// };

// #endif