#ifndef ENPHASE_FUNCTIONS
#define ENPHASE_FUNCTIONS

#include <Arduino.h>
#include "../config/config.h"
#include "../config/enums.h"
#include <HTTPClient.h>
#include "../functions/decodStream.h"

// extern Preferences prefWifi;
// extern Preferences prefEnvoy;
extern Envoy envoy;
String payload;
String SessionId;
HTTPClient https;

//-------------------------------------------------------------------------------"
bool Enphase_get_7_Stream(void)
{

  int httpCode;
  bool retour = false;
  String adr = String(envoy.host);
  String url = "/404.html";
  if (String(envoy.type) == "R")
  {
    url = String(EnvoyR);
    Serial.print("type R ");
    Serial.println(url);
  }
  if (String(envoy.type) == "S")
  {
    url = String(EnvoyStream);
    Serial.print("type S ");
    Serial.println(url);
  }

  // Serial.println("Enphase Get production : https://" + adr + url);

  Serial.printf("[envoyTask] ligne %d https://", __LINE__);
  Serial.println(adr + url);

  if (https.begin("https://" + adr + url))
  {
#ifdef DEBUG
    Serial.printf("[envoyTask] ligne %d Begin OK \n", __LINE__);
#endif
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    https.setAuthorizationType("Bearer");
    https.setAuthorization(envoy.token.c_str());
    https.addHeader("Accept-Encoding", "gzip, deflate, br");
    https.addHeader("User-Agent", "PvRouter/1.1.1");
    https.setReuse(true);
    if (!SessionId.isEmpty())
    {
      https.addHeader("Cookie", SessionId);
    }
  }
  httpCode = https.GET();
  Serial.printf("[envoyTask] ligne %d http code : %d \n", __LINE__, httpCode);

  // while (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
  while (httpCode == 200)
  {
    vTaskDelay(200 / portTICK_PERIOD_MS);
    int error = 0;
    WiFiClient *cl = https.getStreamPtr();
    int compteur = 10;
    do
    {
      cl->find("data: ");
      payload = cl->readStringUntil('\n');
      error = cl->getWriteError();
      // cl->flush();
      if (payload.length() > 10)
      {
        if (bLog || compteur == 10)
        {
          Serial.printf("[envoyTask] ligne %d Payload : lg %d \n%s\n", __LINE__, payload.length(), payload.c_str());
          compteur = 0;
        }
      }
      else
      {
        Serial.printf("[envoyTask] ligne %d Payload : lg %d \n%s\n", __LINE__, payload.length(), payload.c_str());
      }
      int retError = processingJsondata(payload);
      vTaskDelay(300 / portTICK_PERIOD_MS);
      compteur++;

    } while (error == 0);
    cl->stop();
    cl->clearWriteError();
    Serial.printf("[envoyTask] ligne %d Error : %d \n", __LINE__, error);

    // payload = https.getString();
    // Serial.printf("[envoyTask] ligne %d Payload : lg %d \n%s\n", __LINE__, payload.length(), payload.c_str());
    https.end();

    retour = true;
#ifdef DEBUG
    Serial.printf("[envoyTask] ligne %d fin de la réception\n", __LINE__);
#endif
    vTaskDelay(500 / portTICK_PERIOD_MS);

#ifdef DEBUG
    Serial.printf("[envoyTask] ligne %d begin du https://", __LINE__);
    Serial.println(adr + url);
#endif

    if (https.connected())
    {
      bool ret = https.begin("https://" + adr + url);
#ifdef DEBUG
      Serial.printf("[envoyTask] ligne %d Begin OK : %d \n", __LINE__, ret);
#endif
      https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
      https.setAuthorizationType("Bearer");
      https.setAuthorization(envoy.token.c_str());
      https.addHeader("Accept-Encoding", "gzip, deflate, br");
      https.addHeader("User-Agent", "PvRouter/1.1.1");
      https.setReuse(true);
      if (!SessionId.isEmpty())
      {
        https.addHeader("Cookie", SessionId);
      }
    }
    httpCode = https.GET();
#ifdef DEBUG
    Serial.printf("[envoyTask] ligne %d code http : %d \n", __LINE__, httpCode);
#endif
  } // fin du While

  Serial.printf("[envoyTask] ligne %d GET... failed , error: %d \n", __LINE__, httpCode);
  Serial.printf("[envoyTask] ligne %d GET... failed , error: %s \n", __LINE__, https.errorToString(httpCode).c_str());
  https.end();
  return retour;
}

//-------------------------------------------------------------------------------"

bool Enphase_get_7_JWT(void)
{

  bool retour = false;
  String url = "/404.html";
  url = String(EnvoyJ);
  String adr = String(envoy.host);
  Serial.println("[Enphase_get_7_JWT] Enphase contrôle tocken : https://" + adr + url);
  // Initializing an HTTPS communication using the secure client
  if (https.begin("https://" + adr + url))
  {
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    https.setAuthorizationType("Bearer");
    https.setAuthorization(envoy.token.c_str());
    https.addHeader("Accept-Encoding", "gzip, deflate, br");
    https.addHeader("User-Agent", "PvRouter/1.1.1");
    const char *headerkeys[] = {"Set-Cookie"};
    https.collectHeaders(headerkeys, sizeof(headerkeys) / sizeof(char *));
    int httpCode = https.GET();

    // httpCode will be negative on error
    // Serial.println("Enphase_Get_7 : httpcode : " + httpCode);
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        retour = true;
        // Token valide
        Serial.printf("[Enphase_get_7_JWT] ligne %d Enphase contrôle tocket : TOKEN VALIDE httpCode : %d \n", __LINE__, httpCode);
        SessionId.clear();
        SessionId = https.header("Set-Cookie");
      }
      else
      {
        Serial.printf("[Enphase_get_7_JWT] ligne %d Enphase contrôle tocket : TOKEN INVALIDE !!! httpCode : %d \n", __LINE__, httpCode);
        https.end();
      }
    }
    else
    {
      Serial.printf("[Enphase_get_7_JWT] ligne %d Enphase contrôle tocket : ERROR !!! httpCode : %d -> %s \n", __LINE__, httpCode, https.errorToString(httpCode));
    }
  }
  https.end();
  return retour;
}

//-------------------------------------------------------------------------------"

void Enphase_get_7(void)
{

  if (WiFi.isConnected())
  {
    // create an HTTPClient instance
    // retour
    if (SessionId.isEmpty() || Enphase_get_7_Stream() == false)
    { // Permet de lancer le contrôle du token une fois au démarrage (Empty SessionId)

      SessionId.clear();
      Serial.printf("[Enphase_get_7] Enphase version 7, token : -%s- \n", envoy.token.c_str());
      Enphase_get_7_JWT();
    }
  }
  else
  {
    Serial.println("Enphase version 7 : ERROR");
  }
}

#endif
