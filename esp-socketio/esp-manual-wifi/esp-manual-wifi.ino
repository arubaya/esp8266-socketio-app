#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
 
//Variables
int BUTTON_CLEAR = D1;
int LED_CLEAR_INDICATOR = D2;
int buttonState = 0;
int i = 0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;
 
 
//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
 
//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);
 
void setup()
{
 
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);

  //Setup for Clear EEPROM
  pinMode(BUTTON_CLEAR, INPUT);
  pinMode(LED_CLEAR_INDICATOR, OUTPUT);
  digitalWrite(LED_CLEAR_INDICATOR, HIGH);

  Serial.println();
  Serial.println();
  Serial.println("Startup");
  //---------------------------------------- clear EEPROM for SSID and pass
  // String esid;
  // String epass = "";
  // for (int i = 0; i < 512; i++) {
  //   EEPROM.write(i, 0);
  // }
 
  //---------------------------------------- Read EEPROM for SSID and pass
  Serial.println("Reading EEPROM ssid");
 
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
 
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
 
 
  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
    return;
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    setupAP();// Setup HotSpot
    launchWeb();
  }
 
  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
 
}
void loop() {
  if ((WiFi.status() == WL_CONNECTED))
  {
    buttonState = digitalRead(BUTTON_CLEAR);
    if (buttonState == LOW) {
      digitalWrite(LED_CLEAR_INDICATOR, LOW);
      Serial.println("Button is pressed");  
      delay(100);
      digitalWrite(LED_CLEAR_INDICATOR, HIGH);
    }
 
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
 
  }
  else
  {
  }
 
}

void clearEEPROM() {
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  ESP.reset();
}
 
 
//-------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
 
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
 
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol class=\"list-ssid-ol\">";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
 
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("ESP8266", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}
 
void createWebServer()
{
 {
    server.on("/", []() {
 
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE html><html lang=\"en\">";
      // head
      content += "<head>";
      content += "<meta charset=\"UTF-8\" />";
      content += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />";
      content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />";
      content += "<style>";
      content += "* {margin: 0;padding: 0;box-sizing: border-box;font-family: Arial, Helvetica, sans-serif;}";
      content += "body { width: 100%; min-height: 100vh; display: flex; align-items: center; justify-content: center; }";
      content += "input { background-color: #fff; border-radius: 4px; padding: 4px 7px; border: 1px solid #56718d; }";
      content += ".card { min-width: 270px; min-height: 300px; display: flex; flex-direction: column; border-radius: 7px; box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -4px rgba(0, 0, 0, 0.1); }";
      content += ".card-padding { padding-left: 15px; padding-right: 15px; }";
      content += ".card-header { background-color: #56718d; width: 100%; padding-top: 10px; padding-bottom: 10px; color: #fff; border-radius: 7px 7px 0 0; } .card-header > p { font-weight: 600; } .card-body { padding-top: 10px; padding-bottom: 18px; display: flex; flex-direction: column; } .card-body > p, label { font-size: 12px; }";
      content += ".form-scan { margin-top: 10px; } .list-ssid { margin-top: 10px; } .list-ssid-ol { padding-left: 20px; } .list-ssid-ol > li { font-weight: 400; font-size: 12px; } .form-connect { margin-top: 10px; display: flex; flex-direction: column; width: 100%; } .form-connect > input { margin-top: 5px; } .form-connect > label { margin-top: 5px; }";
      content += "</style>";
      content += "<title>ESP8266 Access Point</title>";
      content += "</head>";
      content += "<body><div class=\"card\">";
      content += "<div class=\"card-padding card-header\"><p>ESP8266 Connect to Wifi</p></div>";
      content += "<div class=\"card-padding card-body\">";
      content += "<p>Connect to: 192.168.4.1 for setup</p>";
      content += "<form class=\"form-scan\" action=\"/scan\" method=\"POST\"> <input type=\"submit\" value=\"scan\" /> </form>";
      content += "<p class=\"list-ssid\">SSID list:</p>";
      content += st;
      content += "<form class=\"form-connect\" method=\"get\" action=\"setting\"> <label>SSID: </label> <input name=\"ssid\" length=\"32\" /> <label>Pass: </label> <input name=\"pass\" length=\"64\" /> <input type=\"submit\" /> </form>";
      content += "</div>";
      content += "</div></body>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
 
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
 
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
 
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
 
    });
  } 
}