#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "Servo.h"

Servo servo;

AsyncWebServer server(80);

const char* ssid = "1283-NET";
const char* pass = "meus2229";

#define in1 D0
#define in2 D1
#define in3 D2
#define in4 D3
#define en D4
#define en2 D5

void forward()
{
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
}

void backward()
{
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
}

void stop()
{
  digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
}

void left()
{
  digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
}

void right()
{
  digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
}

void center()
{
    digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Popierdalacz</title>
</head>
<body>
    <script>
        window.addEventListener("gamepadconnected", ()=>{
            setInterval(() => {
                const gamepad = navigator.getGamepads()[0];
                let axes1 = Math.floor(gamepad.axes[0]*90);
                let speed = gamepad.buttons[7].value*255;
                let speed2 = gamepad.buttons[6].value*255;
                console.log(speed2);
                const request = new XMLHttpRequest();
                request.open("GET","/update?value=" + axes1 + "&value2=" + speed + "&value3=" + speed2,true);
                request.send();
            }, 100);
        });
        </script>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

int pos;


void setup() {
Serial.begin(9600);
/*if(WiFi.softAP(ssid, pass))
  {
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Unable to Create Access Point");
  }*/

pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
pinMode(en, OUTPUT);
pinMode(en2, OUTPUT);
analogWrite(en2, 255);

servo.attach(D7);


  Serial.begin(9600);
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connecting...");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    String axis,speed,speed2;

    if(request->hasParam("value")){
      axis = request->getParam("value")->value();
            speed = request->getParam("value2")->value();
            speed2 = request->getParam("value3")->value();
            //Serial.println(axis);
                       // Serial.println(speed2);
                        pos = axis.toInt();

      analogWrite(en,speed.toInt());
      if(speed.toInt() == 0)
      {
        stop();
      }
      else
      {
        forward();
      }
      if(speed2.toInt() > 0)
      {
              analogWrite(en,speed2.toInt());
        backward();
      }

    }
    
    request->send(200);
  });


  server.onNotFound(notFound);
  server.begin();
}

void loop() {
servo.write(pos);
Serial.println(pos);
}