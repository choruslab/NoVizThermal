#ifndef SERVER_H
#define SERVER_H

#include "Server.h"
#include <WiFiS3.h>
#include "arduino_secrets.h" 

class Server {
    private:
        // char ssid[];
        // char password[];
        int server_port;
        int status;
        WifiServer server;
        WifiClient client;

    public:
        Server(int server_port);
        int getServerPort();
        int getStatus()
        void setStatus(int status);
        void listenRequests();
        String toString();


};

#endif