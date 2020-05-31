#include <WiFiClient.h>
#include <ESP32WebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <SabertoothSimplified.h>

SabertoothSimplified ST(Serial2); // use TX2 for communication with the Sabertooth

const char* ssid = "Robot";
const char* password = "123456789";

int driveSpeed = 0;

ESP32WebServer server(80);  //Default port number

void handleRoot()
{
  File myFile = SPIFFS.open("/index.html");
  if (myFile)
  {
    size_t sent = server.streamFile(myFile, "text/html");
    myFile.close();
  }
  else
    Serial.println("Error opening index.html");
}

void handleMotors()
{
  String motorState = "OFF";
  String t_state = server.arg("motorState");

  if (t_state.startsWith("U")) //Drive Forward (UP Arrow)
  {
    Serial.println("Forwards");
    driveSpeed += 1;
  }
  else if (t_state.startsWith("D")) //Reverse (DOWN Arrow)
  {
    Serial.println("Backwards");
    driveSpeed -= 1;
  }
  else if (t_state.startsWith("R")) //Turn Right (Right Arrow)
  {
    Serial.println("Right");
    ST.motor(2, 127);
    delay(250);
    ST.motor(2, 0);
  }
  else if (t_state.startsWith("L")) //Turn Left (LEFT Arrow)
  {
    Serial.println("Left");
    ST.motor(2, -127);
    delay(250);
    ST.motor(2, 0);
  }
  else if (t_state.startsWith("E")) //E-Stop (Space Bar)
  {
    Serial.println("E-Stop");
    driveSpeed = 0;
  }

  if (driveSpeed > 127)
    driveSpeed = 127;
  if (driveSpeed < -127)
    driveSpeed = -127;

  // Update Sabertooth
  Serial.print("Drive Speed: ");
  Serial.print(driveSpeed);

  ST.motor(1, driveSpeed);

  server.send(200, "text/plain", motorState); //Send web page
}

void handleNotFound()
{
  server.send(404, "text/plain", "File Not Found\n\n");
}

void setup() {
  Serial.begin(9600); // for debugging
  Serial2.begin(9600); // for Sabertooth communication

  // Start access point
  WiFi.softAP(ssid, password);
  // Print our IP address
  Serial.println();
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println();

  server.on("/", handleRoot);
  server.on("/setMotors", handleMotors);
  server.onNotFound(handleNotFound);
  server.begin(); //Start the web server
  Serial.println("HTTP server started");

  Serial.print("Initializing SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    while (true) {
      /*loop indefinitely*/
    }
  }
  Serial.println("initialization done.");
}

void loop() {
  server.handleClient();
}
