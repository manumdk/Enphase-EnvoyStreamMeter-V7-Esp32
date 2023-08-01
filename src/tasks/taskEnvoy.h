#include <Arduino.h>
#include "../config/config.h"
#include "../config/enums.h"
#include "../functions/enphaseFunction.h"

String SessionId;

void envoyTask(void *parameter)
{
  // delay(1000);
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  String url;
  if (String(configmodule.envoy) == "R")
  {
    url = String(EnvoyR);
    Serial.print("[Enphase_get_7_Production] type R ");
    Serial.println(url);
  }
  if (String(configmodule.envoy) == "S")
  {
    url = String(EnvoyS);
    Serial.print("[Enphase_get_7_Production] type S ");
    Serial.println(url);
  }
  const char *uri = "/stream/meter";

  // strncpy(installerPassword, ENVOY_PWD, 18);
  // Serial.print("[envoyTask] IP de l'envoy : ");
  // Serial.println(inet_ntoa(IP_envoy));
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("[envoyTask] Pas de connection Wifi ");
  }
  Serial.println("[envoyTask] Début boucle Envoy");
  for (;;)
  {
    Enphase_get_7();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
