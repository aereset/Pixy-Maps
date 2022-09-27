/*                   PIXY-MAPS: Motor Service                */
/*                                                           */
/* version  : V1.0  [26/01/2022]                             */
/* creation :       [26/01/2022]                             */
/*___________________________________________________________*/
// DESCRIPCIÓN:
//
//   Este archivo incluye funciones para la gestión de los
// motores del robot móvil.
/*___________________________________________________________*/
#include <Arduino.h>
#include <MotorService.h>

/**
 * @brief Incializa los motores Izquierdo y Derecho.
 * 
 */
void setupMotors(){
    
    // Setup Left Motor:
    pinMode(MOTOR_LA, OUTPUT);
    pinMode(MOTOR_LB, OUTPUT);
    pinMode(MOTOR_LS, OUTPUT);

    // Setup Right Motor:
    pinMode(MOTOR_RA, OUTPUT);
    pinMode(MOTOR_RB, OUTPUT);
    pinMode(MOTOR_RS, OUTPUT);
}

/**
 * @brief Establece el PWM de un motor dado.
 * 
 * @param motor   Izquierdo: 0 (MOTOR_L), Derecho: 1 (MOTOR_R).
 * @param speed   Valor entre -255 y 255.
 */
void moveMotor(uint8_t motor, int16_t speed){
    int a, b, s;

    switch (motor){
        case MOTOR_L:
            a = MOTOR_LA;
            b = MOTOR_LB;
            s = MOTOR_LS;
            break;
        case MOTOR_R:
            a = MOTOR_RA;
            b = MOTOR_RB;
            s = MOTOR_RS;
            break;    
        default:
            return;
    }

    if (speed < 0){
        digitalWrite(a, HIGH);
        digitalWrite(b, LOW);
        speed = -speed;
    }
    else if(speed > 0){
        digitalWrite(a, LOW);
        digitalWrite(b, HIGH);
    }
    else{
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
    }
    analogWrite(s, speed);
}

/**
 * @brief Returns the rear position of the car as a tuple<int, int>.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @return std::tuple<int, int> 
 */
std::tuple<int,int> getRearPos(String payload){
    // Payload structure: {RearPos:[int,int];FrontPos:[int,int];Trajectory(n):[[int,int],[int,int],...]}
    // Extraccion del string con FrontPos:
    int pos1 = payload.indexOf("RearPos");
    int pos2 = payload.indexOf("FrontPos");
    String currPos = payload.substring(pos1 , pos2);

    // Extracción de x e y:
    pos1 = currPos.indexOf("[");
    pos2 = currPos.indexOf(",");    

    int x = currPos.substring(pos1+1, pos2).toInt();
    
    pos1 = currPos.indexOf("]");
    int y = currPos.substring(pos2+1, pos1).toInt();

    return std::tuple<int,int> {x,y};
}

/**
 * @brief Returns the front position of the car as a tuple<int, int>.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @return std::tuple<int, int> 
 */
std::tuple<int,int> getFrontPos(String payload){
    // Payload structure: {RearPos:[int,int];FrontPos:[int,int];Trajectory(n):[[int,int],[int,int],...]}
    // Extraccion del string con FrontPos:
    int pos1 = payload.indexOf("FrontPos");
    int pos2 = payload.indexOf("Trajectory");
    String currPos = payload.substring(pos1 , pos2);

    // Extracción de x e y:
    pos1 = currPos.indexOf("[");
    pos2 = currPos.indexOf(",");    

    int x = currPos.substring(pos1+1, pos2).toInt();
    
    pos1 = currPos.indexOf("]");
    int y = currPos.substring(pos2+1, pos1).toInt();

    return std::tuple<int,int> {x,y};
}


/**
 * @brief Returns the middle position of the car as a tuple<float, float>.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @return std::tuple<int, int> 
 */
std::tuple<float, float> getPos(String payload){
    std::tuple<int,int> rear = getRearPos(payload);
    std::tuple<int,int> front = getFrontPos(payload);

    float x = std::get<0>(rear) + std::get<0>(front);
    float y = std::get<1>(rear) + std::get<1>(front);

    return std::tuple<int,int> {x/2.0,y/2.0};
}

/**
 * @brief Get the angle between the car and the target point.
 * 
 * @param payload 
 * @param target 
 * @return float Angle in radians
 */
float getOrientation(String payload, std::tuple<int,int> target){
    std::tuple<int,int> rear = getRearPos(payload);
    std::tuple<int,int> front = getFrontPos(payload);

    float ux = std::get<0>(front) - std::get<0>(rear);
    float uy = std::get<1>(front) - std::get<1>(rear);

    float vx = std::get<0>(target) - std::get<0>(rear);
    float vy = std::get<1>(target) - std::get<1>(rear);

    float modu = sqrt(ux*ux + uy*uy);
    float modv = sqrt(vx*vx + vy*vy);

    return asin((ux*vy - uy*vx)/(modu*modv));
}

/**
 * @brief Returns the number of points in the trajectory.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @return std::tuple<int, int> 
 */
int getTrajPoints(String payload){
    // Payload structure: {RearPos:[int,int];FrontPos:[int,int];Trajectory(n):[[int,int],[int,int],...]}
    int pos1 = payload.indexOf("(");
    int pos2 = payload.indexOf(")");

    return payload.substring(pos1+1, pos2).toInt();
}

/**
 * @brief Returns the desired point of the trajectory as a tuple<int, int>.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @param index Index of the point in the trajectory.
 * @return std::tuple<int, int> 
 */
std::tuple<int,int> getPoint(String payload, int index){
    // Payload structure: {RearPos:[int,int];FrontPos:[int,int];Trajectory(n):[[int,int],[int,int],...]}
    int nPoints = getTrajPoints(payload);

    // Si el índice no es correcto:
    if(index >= nPoints)
    {
        Serial.println("Índice no válido! Retorno: [0,0].");
        return std::tuple<int,int> {0,0};
    }

    int pos1 = payload.indexOf("):[") + 2;
    int pos2 = payload.indexOf("]}");

    // String con todos los puntos:
    String currPos = payload.substring(pos1 + 1, pos2);

    int n = 0;
    do{
        pos1 = currPos.indexOf("[");
        ++n;
    }while(n < index);

    currPos = currPos.substring(pos1);

    pos2 = currPos.indexOf(",");
    int x = currPos.substring(1, pos2).toInt();

    pos1 = currPos.indexOf("]");
    int y = currPos.substring(pos2+1, pos1).toInt();

    return std::tuple<int,int> {x,y};
}

/**
 * @brief Returns the nearest point of the trajectory as a tuple<int, int>.
 * 
 * @param payload String with the blocks detected by the Pixy cam.
 * @return std::tuple<int, int> 
 */
std::tuple<int,int> getNearestPoint(String payload, String reachedPoints)
{
    std::tuple<float,float> currentPos = getPos(payload);
    std::tuple<int,int> nearestPoint = currentPos;
    std::tuple<int,int> auxPoint = currentPos;

    int nPoints = getTrajPoints(payload);
    float distance = 0.0;
    float nearestDistance = 10000.0;

    for(int i=0; i<nPoints; ++i)
    {
        auxPoint = getPoint(payload, i);
        distance =  (std::get<0>(currentPos) - std::get<0>(auxPoint)) * (std::get<0>(currentPos) - std::get<0>(auxPoint));
        distance += (std::get<1>(currentPos) - std::get<1>(auxPoint)) * (std::get<1>(currentPos) - std::get<1>(auxPoint));
        distance = sqrt(distance);

        if(distance > 20)
        {
            if(distance < nearestDistance && !(reachedPoints.indexOf("," + String(i) + ",") >= 0))
            {
                nearestPoint = auxPoint;
            }
        }
    }


    return nearestPoint;
}