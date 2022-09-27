#include <VisionService.h>
#include <WebService.h>

extern ESP8266WebServer server;
extern Block car;
extern Block *traj;

WiFiUDP UDP;
unsigned int localPort = 8888;
unsigned int remotePort = 8889;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

void ConnectWiFi_STA(bool useStaticIP = false)
{
   Serial.println("");
   WiFi.mode(WIFI_STA);
   WiFi.begin(SSID, PASSWORD);
   if(useStaticIP) WiFi.config(IPAddress(LOCAL_IP), IPAddress(GATEWAY), IPAddress(SUBNET));
   while (WiFi.status() != WL_CONNECTED) 
   { 
     delay(100);  
     Serial.print('.'); 
   }
 
   Serial.println("");
   Serial.print("Iniciado STA:\t");
   Serial.println(SSID);
   Serial.print("IP address:\t");
   Serial.println(WiFi.localIP());
}

void ConnectWiFi_AP(bool useStaticIP = false)
{ 
   Serial.println("");
   WiFi.mode(WIFI_AP);
   while(!WiFi.softAP(SSID, PASSWORD))
   {
     Serial.println(".");
     delay(100);
   }
   if(useStaticIP) WiFi.softAPConfig(IPAddress(LOCAL_IP), IPAddress(GATEWAY), IPAddress(SUBNET));

   Serial.println("");
   Serial.print("Iniciado AP:\t");
   Serial.println(SSID);
   Serial.print("IP address:\t");
   Serial.println(WiFi.softAPIP());
}

void ProcessPacket(String response)
{
   Serial.println(response);
}

void ConnectUDP()
{
   Serial.println();
   Serial.println("Starting UDP");

   // in UDP error, block execution
   if (UDP.begin(localPort) != 1) 
   {
      Serial.println("Connection failed");
   }

   Serial.println("UDP successful");
}

void SendUDP_ACK()
{
   UDP.beginPacket(UDP.remoteIP(), remotePort);
   UDP.write("ACK");
   UDP.endPacket();
}

void SendUDP_Packet(String content)
{
   UDP.beginPacket(UDP.remoteIP(), remotePort);
   UDP.write(content.c_str());
   UDP.endPacket();
}

void GetUDP_Packet(bool sendACK = true)
{
   int packetSize = UDP.parsePacket();
   if (packetSize)
   {
      // read the packet into packetBufffer
      UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

      Serial.println();
      Serial.print("Received packet of size ");
      Serial.print(packetSize);
      Serial.print(" from ");
      Serial.print(UDP.remoteIP());
      Serial.print(":");
      Serial.println(UDP.remotePort());
      Serial.print("Payload: ");
      Serial.write((uint8_t*)packetBuffer, (size_t)packetSize);
      Serial.println();
      ProcessPacket(String(packetBuffer));

      //// send a reply, to the IP address and port that sent us the packet we received
      if(sendACK) SendUDP_ACK();
   }
   delay(10);
}