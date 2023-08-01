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
// #include "functions/serialFunction.h"
Preferences prefWifi;
Preferences prefEnvoy;

/* 1. Define the  credentials
At the first compilation plan to enter the credentials and the token
Information is saved in flash
*/

#ifdef SaveCredentials
const char *cssid = "yourssid";
const char *cpassword = "yourpasswd";
const char *chostname = "ip de l'envoy";
const char *cport = "80";
const char *ctype = "S";
const char *cmail = "yourmail@fai.com";
const char *cpwdenphase = "pwdenphase";
const char *ctoken = "yourToken";

#else

#endif

Credentials credentials;
Envoy envoy;

const char *enphase_conf = "/enphase.json";
bool bExit = false; // permet de gérer la sortie du menu

//////////////////////////////////// récupération des valeurs

HTTPClient https;
String SessionId;

//-------------------------------------------------------------------------------"

//-------------------------------------------------------------------------------"

void setup()
{
  Serial.begin(115200);
  menu_setup();
#ifdef SaveCredentials
  prefWifi.begin("credentials", false);
  prefWifi.putString("ssid", cssid);
  prefWifi.putString("password", cpassword);
  prefWifi.end();

  prefEnvoy.begin("envoy", false);
  prefEnvoy.putString("hostname", chostname);
  prefEnvoy.putString("port", cport);
  prefEnvoy.putString("type", ctype);
  prefEnvoy.putString("token", ctoken);
  prefEnvoy.putString("pswd", cpswd);
  prefEnvoy.putString("mail", cmail);
  Serial.println("prefEnvoy Saved using Preferences");
  prefEnvoy.end();
  Serial.println("Recompile the code without SaveCredentials");
  while (1)
    ;
#endif

  prefWifi.begin("credentials", false);
  credentials.ssid = prefWifi.getString("ssid", "");
  credentials.password = prefWifi.getString("password", "");
  prefWifi.end();

  prefEnvoy.begin("envoy", false);
  envoy.host = prefEnvoy.getString("host", "10.190.2.120");
  envoy.port = prefEnvoy.getString("port", "80");
  envoy.type = prefEnvoy.getString("type", "S");
  envoy.token = prefEnvoy.getString("token", "");
  envoy.serial = prefEnvoy.getString("serial", "");
  envoy.pswd = prefEnvoy.getString("pswd", "pass-a-saisir");
  envoy.mail = prefEnvoy.getString("mail", "envoy@enphase.tutu");

  prefEnvoy.end();

  Serial.printf("[SETUP] ligne %d host : %s \n", __LINE__, envoy.host.c_str());
  Serial.printf("[SETUP] ligne %d port : %s \n", __LINE__, envoy.port.c_str());
  Serial.printf("[SETUP] ligne %d type : %s \n", __LINE__, envoy.type.c_str());
  Serial.printf("[SETUP] ligne %d mail : %s \n", __LINE__, envoy.mail.c_str());
  Serial.printf("[SETUP] ligne %d pswd : %s \n", __LINE__, envoy.pswd.c_str());
  Serial.printf("[SETUP] ligne %d serial : %s \n", __LINE__, envoy.serial.c_str());
  Serial.printf("[SETUP] ligne %d token : %s \n", __LINE__, envoy.token.c_str());

  if (credentials.ssid == "" || credentials.password == "")
  {
    Serial.println("No values saved for ssid or password");
  }
  else
  {
    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(credentials.ssid.c_str(), credentials.password.c_str());
    Serial.print("Connecting to WiFi ..");
    int countwifi;
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print('.');
      delay(1000);
    }
    Serial.println(WiFi.localIP());
    // ----------------------------------------------------------------
    // TASK: Gestino ThEcs.
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
    // ----------------------------------------------------------------
    // TASK: Enphase
    // ----------------------------------------------------------------
    xTaskCreatePinnedToCore(
        envoyTask,
        "envoyTask",         // Task name
        5000,                // Stack size (bytes)
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
    while (1)
    {
      /* code */
    }
  }