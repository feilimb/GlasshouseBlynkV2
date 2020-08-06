#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

char ssid[] = "...";
char pass[] = "...";
char blynk_auth[] = "...";

uint count = 0;

int level_1 = 2;
int level_2 = 14;
int level_3 = 15;
int level_4 = 13;
int relayPin = 12;

int switchState1, switchState2, switchState3, switchState4;

int pollingPeriodSeconds = 5;
int newPeriodAsInt = 0;
int timerID;

BlynkTimer timer;
BlynkTimer timer2;

BLYNK_WRITE(V7) {
  String period = param.asStr();
  newPeriodAsInt = period.toInt();
}

void sendSensorData()
{
  switchState1 = digitalRead(level_1);
  switchState2 = digitalRead(level_2);
  switchState3 = digitalRead(level_3);
  switchState4 = digitalRead(level_4);
  Serial.print(">> SwitchState (1): ");
  Serial.println(switchState1);
  Serial.print(">> SwitchState (2): ");
  Serial.println(switchState2);
  Serial.print(">> SwitchState (3): ");
  Serial.println(switchState3);
  Serial.print(">> SwitchState (4): ");
  Serial.println(switchState4);
  
  count = count + 1;
  if (count > 100)
  {
    count = 1;
  }

  int wLevel = 0;
  if (switchState1 == HIGH)
  {
    wLevel += 10;
    if (switchState2 == HIGH)
    {
      wLevel += 25;
      if (switchState3 == HIGH)
      {
        wLevel += 25;
        if (switchState4 == HIGH)
        {
          wLevel += 20;
        }
      }
    }
  }
  Serial.print(">> Water Level: ");
  Serial.println(wLevel);
  Serial.print(">> Pump State: ");
  Serial.println(digitalRead(relayPin));
  Serial.println("=====================================");

  Blynk.virtualWrite(V0, wLevel);
  //Blynk.virtualWrite(V1, switchState1);
  //Blynk.virtualWrite(V2, switchState2);
  //Blynk.virtualWrite(V3, switchState3);
  //Blynk.virtualWrite(V4, switchState4);
  Blynk.virtualWrite(V6, count);
}

void updatePollMainTimer()
{
  if (newPeriodAsInt > 0)
  {
    pollingPeriodSeconds = newPeriodAsInt;
    newPeriodAsInt = 0;
    Serial.print(">> Update polling period now: ");
    Serial.println(pollingPeriodSeconds);
    //timerID = 
    timer.changeInterval(timerID, pollingPeriodSeconds * 1000);
  }  
}

void setup()
{
  pinMode(level_1, INPUT);
  pinMode(level_2, INPUT);
  pinMode(level_3, INPUT);
  pinMode(level_4, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  Serial.begin(115200);

  // ===== WIFI Initialisation =====
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.println("WiFi: Attempt connection now!");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // ===== BLYNK Initialisation =====
  Blynk.begin(blynk_auth, ssid, pass);

  // ===== ArduinoOTA Initialisation =====
  /*ArduinoOTA.setHostname("GlasshouseBlynk");
  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
  ArduinoOTA.onEnd([]() {
    Serial.println("\n>> ArduinoOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf(">> ArduinoOTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(">> ArduinoOTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println(">> ArduinoOTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println(">> ArduinoOTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println(">> ArduinoOTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println(">> ArduinoOTA End Failed");
  });
  ArduinoOTA.begin();*/

  // setup our blynk timer function
  timerID = timer.setInterval(pollingPeriodSeconds * 1000, sendSensorData);
  timer2.setInterval(1000, updatePollMainTimer);
}

void loop()
{
  Blynk.run();
  timer.run();
  timer2.run();
  //ArduinoOTA.handle();
}
