
/*                      MotorService.h                       */
/*                                                           */
/* version  : V1.0  [26/01/2022]                             */
/* creation :       [26/01/2022]                             */
/*___________________________________________________________*/
// DESCRIPCIÓN:
//
//   Esta librería incluye la cabecera de las funciones del
// archivo "motorService.cpp", así como macros para la gestión
// de los motores del robot móvil.
/*___________________________________________________________*/

#ifndef __MOTOR_SERVICE__
    #define __MOTOR_SERVICE__

    #define MOTOR_L 0
    #define MOTOR_R 1

    // PINOUT:
    #define MOTOR_LA D4  // Left motor, input A.
    #define MOTOR_LB D3  // Left motor, input B.
    #define MOTOR_LS D5  // Left motor, speed (PWM).

    #define MOTOR_RA D7  // Right motor, input A.
    #define MOTOR_RB D8  // Right motor, input B.
    #define MOTOR_RS D6  // Right motor, speed (PWM).
    
    #include <tuple>

    // FUNCTIONS:
    void setupMotors();
    void moveMotor(uint8_t motor, int16_t speed);
    std::tuple<int, int> getRearPos(String payload);
    std::tuple<int, int> getFrontPos(String payload);
    std::tuple<float, float> getPos(String payload);
    float getOrientation(String payload, std::tuple<int,int> target);
    int getTrajPoints(String payload);
    std::tuple<int, int> getPoint(String payload, int index);
    std::tuple<int,int> getNearestPoint(String payload, String reachedPoints);
#endif