#ifndef __WEB_SERVICE__
    #define __WEB_SERVICE__

    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #include <Secret.h>
    #include <WiFiUdp.h>

    #define PORT 80

    void ConnectWiFi_STA(bool useStaticIP);
    void ConnectWiFi_AP(bool useStaticIP);

    void ProcessPacket(String response);
    void ConnectUDP();
    void SendUDP_ACK();
    void SendUDP_Packet(String content);
    void GetUDP_Packet(bool sendACK);

#endif