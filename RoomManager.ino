#include <WiFi.h>
#include <WiFiUdp.h>

#define ssid "FABLAB"
#define pass "F@bl@b2017"

#define ledOnBoard 2

WiFiUDP udpServer;

String _buffer = "";

void setup() {
    Serial.begin(115200);
    pinMode(ledOnBoard, OUTPUT);

    WiFi.mode(WIFI_STA);
    if (WiFi.begin(ssid, pass)) {
        Serial.println("Connected");
        Serial.println(WiFi.softAPIP());
    }

    udpServer.begin(554);
}

void loop() {
    // tcpManager();
    listen();
}

// void tcpManager(){
//     if(client.connected()){
//         if(client.available() > 0){
//             _buffer = "";
//             while(client.available() > 0){
//                 char z = client.read();
//                 _buffer += z;
//             }

//             Serial.print("\nUm cliente enviou uma mensagem");
//             Serial.print("\n...IP do cliente: ");
//             Serial.print(client.remoteIP());
//             Serial.print("\n...IP do servidor: ");
//             Serial.print(WiFi.softAPIP());
//             Serial.print("\n...Mensagem do cliente: " + _buffer + "\n");

//             client.print("\nO servidor recebeu sua mensagem");
//             client.print("\n...Seu IP: ");
//             client.print(client.remoteIP());
//             client.print("\n...IP do Servidor: ");
//             client.print(WiFi.softAPIP());
//             client.print("\n...Sua mensagem: " + _buffer + "\n");
//         }
//     }else{
//         client = server.available();
//         delay(1);
//     }
// }

void listen()  // Sub-rotina que verifica se há pacotes UDP's para serem lidos.
{
    if (udpServer.parsePacket() > 0)  // Se houver pacotes para serem lidos
    {
        _buffer = "";  // Reseta a string para receber uma nova informaçao
        while (udpServer.available() >
               0)  // Enquanto houver dados para serem lidos
        {
            char z = udpServer.read();  // Adiciona o byte lido em uma char
            _buffer += z;               // Adiciona o char à string
        }

        // Após todos os dados serem lidos, a String estara pronta.

        Serial.println(_buffer);  // Printa a string recebida no Serial monitor.

        digitalWrite(ledOnBoard, 0);  //-
        delay(5);                     //-
        digitalWrite(ledOnBoard,
                     1);  // Pisca o LED rapidamente apos receber a string.
        delay(5);
        digitalWrite(ledOnBoard, 0);
    }
}