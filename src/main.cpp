/* -------------------------------------------------------------------------------------------------
Ce code permet de récupérer le flow de data envoyé toutes les secondes par la passerelle Envoy S du
fabricant Enphase de micro onduleur photovoltaïque.
Je l'ai adapté à la version de firmware de l'Envoy V7 déployé depuis peu par Enphase.
J'ai repris le principe de l'authentification JWT que Kristof @TofMassilia13320 a publié récemment
sur son fork du Pv router xlyric/pv-router-esp32 Pour comprendre le fonctionnement, j’ai utilisé un flow
sous NodeRed. https://community.jeedom.com/t/tuto-enphase-metered-node-red-jmqtt/87453
Le token doit être récupéré en se connectant sur la passerelle Envoy depuis un PC et rentré dans le code.
Pour simplifier le débogage, le partage et les updates, les infos SSID, PWD, URL, et Token sont sauvegardés via la librairie Preferences.
https://github.com/manumdk/Enphase-EnvoyStreamMeter-V7-Esp32
------------------------------------------------------------------------------------------------- */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include <Preferences.h>
#include "config/config.h"
#include "config/enums.h"
#include "tasks/serial_task.h"
#include "tasks/taskEnvoy.h"

Envoy envoy;
Configwifi configwifi;

//-------------------------------------------------------------------------------"

void setup()
{
  Serial.begin(115200);
  menu_setup();
  configwifi.recup_wifi();
  Serial.printf("[SETUP] ligne %d SID : %s \n", __LINE__, String(configwifi.SID));
  Serial.printf("[SETUP] ligne %d passwd : %s \n", __LINE__, String(configwifi.passwd));

  envoy.recup_envoy();
  Serial.printf("[SETUP] ligne %d host : %s \n", __LINE__, envoy.host.c_str());
  Serial.printf("[SETUP] ligne %d port : %s \n", __LINE__, envoy.port.c_str());
  Serial.printf("[SETUP] ligne %d type : %s \n", __LINE__, envoy.type.c_str());
  Serial.printf("[SETUP] ligne %d mail : %s \n", __LINE__, envoy.mail.c_str());
  Serial.printf("[SETUP] ligne %d pswd : %s \n", __LINE__, envoy.pswd.c_str());
  Serial.printf("[SETUP] ligne %d serial : %s \n", __LINE__, envoy.serial.c_str());
  Serial.printf("[SETUP] ligne %d token : %s \n", __LINE__, envoy.token.c_str());
  Serial.printf("[SETUP] ligne %d date du token : %d \n", __LINE__, envoy.token_timestamp);
  menu_setup();
  // ----------------------------------------------------------------
  // TASK: Lecture liaison série.
  // ----------------------------------------------------------------
  xTaskCreatePinnedToCore(
      serial_read_task,
      "serial_read_task", // Task name
      5000,               // Stack size (bytes)
      NULL,               // Parameter
      5,                  // Task priority
      NULL,               // Task handle
      0                   // Core0
  );

  if (configwifi.SID == "" || configwifi.passwd == "")
  {
    Serial.println("!!!!!!!!!!!!!!!!!!!! No values saved for ssid or password");
  }
  else
  {
    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(configwifi.SID, configwifi.passwd);
    Serial.print("Connecting to WiFi ..");
    int countwifi;
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print('.');
      delay(1000);
    }
    Serial.println(WiFi.localIP());

    // ----------------------------------------------------------------
    // TASK: Enphase
    // ----------------------------------------------------------------
    xTaskCreatePinnedToCore(
        envoyTask,
        "envoyTask",         // Task name
        50000,               // Stack size (bytes)
        NULL,                // Parameter
        5,                   // Task priority
        NULL,                // Task handle
        ARDUINO_RUNNING_CORE // Core1
    );
    delay(2000);
  }
}
//-------------------------------------------------------------------------------"

void loop()
{
}