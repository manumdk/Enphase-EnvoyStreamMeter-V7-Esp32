#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include <Preferences.h>
Preferences preferences;

/* 1. Define the  credentials 
At the first compilation plan to enter the credentials and the token
Information is saved in flash
*/

//#define SaveCredentials

#ifdef SaveCredentials
const char *cssid = "yourssid";
const char *cpassword = "yourpasswd";
const char *chostname = "ip de l'envoy";
const char *cport = "80";
const char *ctype = "S";
const char *ctoken = "yourToken";
#else

#endif
String ssid;
String password;
String HOST_NAME;
String PORT;
String TYPE_ENVOY;
String TOKEN;

#define EnvoyJ "/auth/check_jwt"
#define EnvoyR "/api/v1/production"
#define EnvoyS "/production.json"
#define EnvoyStream "/stream/meter"

const char *enphase_conf = "/enphase.json";

//////////////////////////////////// récupération des valeurs

HTTPClient https;
String SessionId;

bool Enphase_get_7_Stream(void)
{

  int httpCode;
  bool retour = false;
  String adr = String(HOST_NAME);
  String url = "/404.html";
  if (String(TYPE_ENVOY) == "R")
  {
    url = String(EnvoyR);
    Serial.print("type R ");
    Serial.println(url);
  }
  if (String(TYPE_ENVOY) == "S")
  {
    url = String(EnvoyStream);
    Serial.print("type S ");
    Serial.println(url);
  }

  // Serial.println("Enphase Get production : https://" + adr + url);
  Serial.printf("[envoyTask]  ligne %d http://", __LINE__);
  Serial.println(adr + url);

  if (https.begin("https://" + adr + url))
  {
    Serial.printf("[envoyTask] --------  ligne %d Begin OK \n", __LINE__);
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    https.setAuthorizationType("Bearer");
    https.setAuthorization(TOKEN.c_str());
    https.addHeader("Accept-Encoding", "gzip, deflate, br");
    https.addHeader("User-Agent", "PvRouter/1.1.1");
    https.setReuse(true);
    if (!SessionId.isEmpty())
    {
      https.addHeader("Cookie", SessionId);
    }
  }
  httpCode = https.GET();
  Serial.printf("[envoyTask]  ligne %d http , error: %d \n", __LINE__, httpCode);

  // while (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
  while (httpCode == 200)
  {
    String payload;
    WiFiClient *cl = https.getStreamPtr();
    int error = 0;
    do
    {
      cl->find("data: ");
      payload = cl->readStringUntil('\n');
      // cl->flush();

      Serial.printf("[envoyTask] --------  ligne %d Payload : lg %d \n%s\n", __LINE__,payload.length(), payload.c_str());

    } while (error);
    cl->stop();
    https.end();

    retour = true;
    Serial.printf("[envoyTask]  ligne %d fin de la réception\n", __LINE__);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.printf("[envoyTask]  ligne %d begin du https://", __LINE__);
    Serial.println(adr + url);
    if (https.connected())
    {
      bool ret = https.begin("https://" + adr + url);
      Serial.printf("[envoyTask] ligne %d Begin OK : %d \n", __LINE__, ret);
      https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
      https.setAuthorizationType("Bearer");
      https.setAuthorization(TOKEN.c_str());
      https.addHeader("Accept-Encoding", "gzip, deflate, br");
      https.addHeader("User-Agent", "PvRouter/1.1.1");
      https.setReuse(true);
      if (!SessionId.isEmpty())
      {
        https.addHeader("Cookie", SessionId);
      }
    }
    httpCode = https.GET();
    Serial.printf("[envoyTask] ligne %d code http : %d \n", __LINE__, httpCode);
  } // fin du While

  Serial.printf("[envoyTask]  ligne %d GET... failed , error: %d \n", __LINE__, httpCode);
  Serial.printf("[envoyTask]  ligne %d GET... failed , error: %s \n", __LINE__, https.errorToString(httpCode).c_str());
  https.end();
  return retour;
}

bool Enphase_get_7_JWT(void)
{

  bool retour = false;
  String url = "/404.html";
  url = String(EnvoyJ);
  String adr = String(HOST_NAME);
  Serial.println("Enphase contrôle tocken : https://" + adr + url);
  // Initializing an HTTPS communication using the secure client
  if (https.begin("https://" + adr + url))
  {
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    https.setAuthorizationType("Bearer");
    https.setAuthorization(TOKEN.c_str());
    https.addHeader("Accept-Encoding", "gzip, deflate, br");
    https.addHeader("User-Agent", "PvRouter/1.1.1");
    const char *headerkeys[] = {"Set-Cookie"};
    https.collectHeaders(headerkeys, sizeof(headerkeys) / sizeof(char *));
    int httpCode = https.GET();
    Serial.printf("[Enphase_get_7_JWT] %d httpCode : %d \n", __LINE__, httpCode);

    // httpCode will be negative on error
    // Serial.println("Enphase_Get_7 : httpcode : " + httpCode);
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        retour = true;
        // Token valide
        Serial.println("Enphase contrôle tocket : TOKEN VALIDE ");
        SessionId.clear();
        SessionId = https.header("Set-Cookie");
      }
      else
      {
        Serial.println("Enphase contrôle tocket : TOKEN INVALIDE !!!");
        https.end();
      }
    }
  }
  https.end();
  return retour;
}

void Enphase_get_7(void)
{

  if (WiFi.isConnected())
  {
    // create an HTTPClient instance
    if (SessionId.isEmpty() || Enphase_get_7_Stream() == false)
    { // Permet de lancer le contrôle du token une fois au démarrage (Empty SessionId)
      SessionId.clear();
      Serial.printf("[Enphase_get_7] Enphase version 7, token : %s \n", TOKEN);
      Enphase_get_7_JWT();
    }
  }
  else
  {
    Serial.println("Enphase version 7 : ERROR");
  }
}
void setup()
{
  Serial.begin(115200);
#ifdef SaveCredentials
  preferences.begin("credentials", false);
  preferences.putString("ssid", cssid);
  preferences.putString("password", cpassword);
  preferences.putString("hostname", chostname);
  preferences.putString("port", cport);
  preferences.putString("type", ctype);
  preferences.putString("token", ctoken);
  Serial.println("Credentials Saved using Preferences");
  preferences.end();
  Serial.println("Recompile the code without SaveCredentials");
  while (1)
    ;
#endif

  preferences.begin("credentials", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  HOST_NAME = preferences.getString("hostname", "");
  PORT = preferences.getString("port", "80");
  TYPE_ENVOY = preferences.getString("type", "S");
  TOKEN = preferences.getString("token", "");

  if (ssid == "" || password == "")
  {
    Serial.println("No values saved for ssid or password");
  }
  else
  {
    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print('.');
      delay(1000);
    }
    Serial.println(WiFi.localIP());
  }
}

void loop()
{
  Serial.println("Passage LOOP");
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WIFI_OK");
    Enphase_get_7();
  }
  else
  {
    Serial.println("WIFI_HS");
  }

  delay(2000);
}