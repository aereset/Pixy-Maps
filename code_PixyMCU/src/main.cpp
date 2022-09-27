#include <ESP8266WiFi.h>
#include <Secret.h>
#include <WiFiUdp.h>
#include <VisionService.h>

// Variables de gestión de red:
WiFiUDP Udp;
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "OK";  // a reply string to send back
String payload;

// Variables de gestión de cámara Pixy:
Pixy pixy;

void setup()
{
  // Inicialización de comunicación Serial:
  Serial.begin(9600);

  // Incialización de Punto de Acceso:
  Serial.print("Configurando modo Soft-AP... ");
  Serial.println(WiFi.softAPConfig(LOCAL_IP, GATEWAY, SUBNET) ? "Listo" : "Falló!");

  Serial.print("Estableciendo modo Soft-AP... ");
  Serial.println(WiFi.softAP(SSID, PASSWORD) ? "Listo" : "Falló!");

  Serial.print("Dirección IP Soft-AP = ");
  Serial.println(WiFi.softAPIP());

  // Inicialización de comunicación UDP:
  Udp.begin(LOCAL_UDP_PORT);
  Serial.print("Puerto UDP = ");
  Serial.println(Udp.localPort());

  // Inicialización de la cámara Pixy:
  pixy.init();
  pixy.pixyBlink(RGB_GREEN, 150, 3);
  Serial.println("Cámara Pixy iniciada");
  Serial.println("____________________________________________________________");
}

void loop()
{ 
  static int i = 0;
  int j;
  uint16_t blocks;
  //char buf[32];
  
  // Lectura del mapa:
  blocks = pixy.getBlocks();
  Point traj[blocks-1];
  Point car_rear = {.x=0, .y=0};
  Point car_front = {.x=0, .y=0};
  
  // Si hay bloques, se procesa la información:
  if (blocks)
  {
    i++;

    //if (i%50==0) // Usar esta opción si se descomentan los printf!!!!!
    if (i%10==0)
    {
      //sprintf(buf, "Detected %d:\n", blocks);
      //Serial.print(buf);

      int k = 0;
      for (j=0; j<blocks; j++)
      {
        //sprintf(buf, "  block %d: ", j);
        //Serial.print(buf); 
        //pixy.blocks[j].print();

        uint16_t sig = pixy.blocks[j].signature;

        // Si es un punto de trayectoria:
        if(sig == SIG_TRAJ)
        {
          traj[k].x = pixy.blocks[j].x;
          traj[k].y = pixy.blocks[j].y;
          k++;
        }
        // Si es la posición trasera del coche:
        else if(sig == SIG_CAR_R)
        {
          car_rear.x = pixy.blocks[j].x;
          car_rear.y = pixy.blocks[j].y;
        }
        // Si es la posición delantera del coche:
        else if(sig == SIG_CAR_F)
        {
          car_front.x = pixy.blocks[j].x;
          car_front.y = pixy.blocks[j].y;
        }
      }      
  
      // Envío de datos:
      Udp.beginPacket(IPAddress(LOCAL_IP), LOCAL_UDP_PORT);
      payload = "{RearPos:["    + String(car_rear.x)  + ","    + String(car_rear.y)  + 
                "{FrontPos:["   + String(car_front.x) + ","    + String(car_front.y) + 
                "];Trajectory(" + String(blocks-2)    + "):[";

      for(j=0; j<blocks-3; j++)
      {
        payload = payload + "[" + String(traj[j].x) + "," + String(traj[j].y) + "],";
      }
      payload = payload + "[" + String(traj[j].x) + "," + String(traj[j].y) + "]]}";
      //Serial.println(payload);
      Udp.write(payload.c_str());
      Udp.endPacket();
    }
  } 
}