#include <IRremote.h>
#include <IRremoteInt.h>
#include <SPI.h>
#include <Ethernet.h>

IRsend irsend;

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 251); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String HTTP_req;            // stores the HTTP request

int val;
int tempPin = 5;

unsigned int tv_on[23] = {900, 850, 900, 850, 1800, 850, 900, 850, 900, 850, 900, 850, 900, 850, 900, 850, 900, 1750, 900, 850, 1800, 800, 950};
unsigned int tv_volup[23] = {900, 900, 1700, 850, 900, 900, 850, 900, 850, 900, 850, 900, 850, 900, 850, 1800, 1750, 900, 850, 900, 850, 850, 900};
unsigned int tv_voldn[23] = {900, 850, 900, 850, 1850, 800, 900, 850, 900, 850, 950, 800, 950, 800, 900, 1750, 1800, 800, 950, 800, 950 ,1700, 950};
unsigned int tv_seven[25] = {900, 850, 900, 850, 1800, 800, 950, 800, 950, 850, 900, 850, 900, 850, 900, 850, 900, 850, 900, 1750, 900, 850, 900, 850, 900};
unsigned int tv_dot[23] = {1750, 850, 900, 850, 900, 850, 950, 800, 950, 1700, 900, 850, 950, 800, 950, 800, 950, 850, 1750, 850, 900, 850, 900};
unsigned int tv_one[25] = {900, 850, 900, 850, 1800, 800, 900, 850, 950, 800, 950, 850, 900, 850, 900, 850, 900, 850, 900, 850, 900, 850, 900, 1750, 900};
unsigned int xbox_on[68]={ 23620, 2650, 900, 500, 450, 350, 500, 400, 900, 500, 850, 1250, 950, 400, 500, 350, 500, 400, 500, 400, 500, 400, 500, 350, 500, 400, 500, 400, 500, 350, 500, 400, 500, 850, 500, 350, 500, 400, 500, 400, 500, 400, 500, 350, 500, 400, 500, 400, 950, 800, 950, 400, 500, 400, 450, 400, 500, 400, 500, 400, 500, 800, 500, 400, 950, 400, 500, 350};
unsigned int clima_on[100]={3850,1350,650,1100,650,1050,650,400,650,350,650,400,650,1050,650,400,650,350,650,1100,650,1100,650,350,650,1100,650,350,650,400,650,1100,600,1100,650,400,650,1100,600,1100,650,400,600,400,650,1100,650,350,650,400,650,1050,650,400,650,350,650,400,650,350,650,400,600,400,650,400,600,400,650,350,650,400,650,350,650,400,650,350,650,400,600,400,650,400,600,400,650,350,650,400,650,350,650,1100,650,400,600,400,650,1100};

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    //Serial.begin(9600);       // for diagnostics
    pinMode(3, INPUT);        // switch is attached to Arduino pin 3
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: application/json;charset=utf-8");
                    client.println("Server: Arduino");
                    client.println("Connection: keep-alive");
                    client.println();
                    // AJAX request for switch state
                    if (HTTP_req.indexOf("tvon") > -1) {
                        // read switch state and send appropriate paragraph text
                        tvon(client);
                    }
                    if (HTTP_req.indexOf("temperatura") > -1) {
                        // read switch state and send appropriate paragraph text
                        temperatura(client);
                    }
                    if (HTTP_req.indexOf("volup") > -1) {
                        // read switch state and send appropriate paragraph text
                        volup(client);
                    }
                    if (HTTP_req.indexOf("voldown") > -1) {
                        // read switch state and send appropriate paragraph text
                        voldown(client);
                    }
                    if (HTTP_req.indexOf("azteca_hd") > -1) {
                        // read switch state and send appropriate paragraph text
                        azteca_hd(client);
                    }
                    if (HTTP_req.indexOf("despertador") > -1) {
                        // read switch state and send appropriate paragraph text
                        despertador(client);
                    }                                        
                    // display received HTTP request on serial port
                    //Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// send the state of the switch to the web browser
void tota(EthernetClient cl){
  cl.println("Tota"); 
}
void temperatura(EthernetClient cl){
  val = analogRead(tempPin);
  float mv = ( val/1024.0)*5000; 
  float cel = mv/10;
  cl.print("[{\"tempIn\":");
  cl.print(cel);
  cl.print(", \"unit\":\"Celcius\" }]");
}
void tvon(EthernetClient cl){
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_on,23,38);
    //Serial.println("Tv ON");
    delay(40);
  }
  cl.println("[{\"Action\":\"TV_ON\",""Status\":\"ok\" }]"); 
}

void volup(EthernetClient cl){
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_volup,23,38);
    //Serial.println("Volumen Up");
    delay(40);
  }
  cl.println("[{\"Action\":\"VOL_UP\",""Status\":\"ok\" }]"); 
}

void voldown(EthernetClient cl){
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_voldn,23,38);
    //Serial.println("Volumen Down");
    delay(40);
  }
  cl.println("[{\"Action\":\"VOL_DOW\",""Status\":\"ok\" }]"); 
}

void azteca_hd(EthernetClient cl){
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_seven,25,38);
    //Serial.println("Azteca HD");
    delay(40);
  }
    for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_dot,23,38);
    //Serial.println("Azteca HD");
    delay(40);
  }
    for (int i = 0; i < 3; i++) {
    irsend.sendRaw(tv_one,25,38);
    //Serial.println("Azteca HD");
    delay(40);
  }
  cl.println("[{\"Action\":\"AZTECA_HD\",""Status\":\"ok\" }]"); 
}

void xbox_onoff(EthernetClient cl){
   for (int i = 0; i < 3; i++) {
    irsend.sendRaw(xbox_on,68,38);
    //Serial.println("Xbox ON");
    delay(40);
  }
  cl.println("[{\"Action\":\"TV_ON\",""Status\":\"ok\" }]"); 
}

void clima_onoff(EthernetClient cl){
     for (int i = 0; i < 3; i++) {
      irsend.sendRaw(clima_on,100,38);
      //Serial.println("Clima ON");
      delay(40);
    }
    cl.println("[{\"Action\":\"TV_ON\",""Status\":\"ok\" }]"); 
  }
void despertador(EthernetClient cl){
    tvon(cl);
    delay(15000);
    azteca_hd(cl);
    cl.println("[{\"Action\":\"Despertador\",""Status\":\"ok\" }]"); 
  }
