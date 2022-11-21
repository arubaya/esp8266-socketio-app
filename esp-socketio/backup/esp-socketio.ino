#include <Arduino.h>
#include <SocketIoClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define USER_SERIAL Serial

const char *ssid = "RUN System Office V";
const char *pass = "12345678";

int button = 5;
int led = 4;
int LED = D7;
int status = false; // to trigger led high or low
char toEmit[5];     // to emit to server; hold status to be converted from int to char

SocketIoClient webSocket;

void setup()
{

  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

  USER_SERIAL.begin(115200);

  connectWiFi();

  // uncomment for your environment
  // this is for local dev
  webSocket.begin("192.168.43.10", 3000);

  // for prod; not working with https?
  //  webSocket.begin("socket-api.rltech.xyz");

  webSocket.on("message", controlled);
  webSocket.on("msg", controlledButton);
  webSocket.on("getStatus", returnStatus);
  webSocket.on("broadcast-new-user", printToSerial);

  webSocket.on("led-status-message", printToSerial);
  webSocket.on("led-status", controlled);
  pinMode(led, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  digitalWrite(LED, LOW);
}

void loop()
{
  webSocket.loop();

  // if button is pressed
  //if (digitalRead(button) == true)
  //{
  // negate
  //status = !status;
  //webSocket.emit("manual", itoa(status, toEmit, 5));
  //digitalWrite(led, status); // then on off led
  //}
  //while (digitalRead(button) == true)
  //; // button will loop here because it is pull up
}

void printToSerial(const char *message, size_t length)
{
  pinMode(LED_BUILTIN, HIGH);
  USER_SERIAL.println(message);
  delay(500);
  pinMode(LED_BUILTIN, LOW);
}

// return status
void returnStatus(const char *message, size_t length)
{
  // emit to server what is the status always
  webSocket.emit("manual", itoa(status, toEmit, 5));
}

// when controlled using the web/mobile app
void controlled(const char *message, size_t length)
{
    

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  short val = doc["status"];
  const char* valChar = message;
  //status = !status; // negate
  USER_SERIAL.println(val);
//  digitalWrite(LED, LOW);

  if (val == 1)
  {
    digitalWrite(LED, LOW); // then on off led
  }
  else
  {
    digitalWrite(LED, HIGH); // then on off led
  }
}

// when controlled manually by button
void controlledButton(const char *message, size_t length)
{
  //  USER_SERIAL.println(message);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  // emit to server
  webSocket.emit("logs");

  short val = doc["value"];
  if (val == 1)
  {
    digitalWrite(led, HIGH); // then on off led
  }
  else
  {
    digitalWrite(led, LOW); // then on off led
  }
}

void connectWiFi()
{
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    USER_SERIAL.print(".");
    delay(1000);
  }

  USER_SERIAL.print("");
  USER_SERIAL.println("WiFi connected");
  USER_SERIAL.print("IP Address : ");
  USER_SERIAL.println(WiFi.localIP());
}
