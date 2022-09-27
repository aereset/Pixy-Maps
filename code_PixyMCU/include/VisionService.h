#ifndef __VISION_SERVICE__
    #define __VISION_SERVICE__
    
    #include <Wire.h>
    #include "Pixy/Pixy.h"

    // COLORES RGB
    #define RGB_RED 255,0,0
    #define RGB_GREEN 0,255,0
    #define RGB_BLUE 0,0,255

    // COLORES PIXY
    #define SIG_RED     1
    #define SIG_BLUE    2
    #define SIG_YELL    3
    
    // CIRCUITO
    #define SIG_TRAJ    SIG_RED     // Código de los puntos de la trayectoria.
    #define SIG_CAR_R   SIG_BLUE    // Código del punto trasero del coche (rear).
    #define SIG_CAR_F   SIG_YELL    // Código del punto delantero del coche (front).
    #define TRAJ_POINTS 6

    /**
     * @brief Estructura para describir puntos 2D.
     * 
     */
    typedef struct Point{
        uint16_t x;
        uint16_t y;
    } Point;

#endif