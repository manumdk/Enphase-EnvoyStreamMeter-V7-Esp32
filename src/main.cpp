


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include <Preferences.h>
Preferences prefWifi;
Preferences prefEnvoy;

/* 1. Define the  credentials
At the first compilation plan to enter the credentials and the token
Information is saved in flash
*/

// #define SaveCredentials

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

struct Credentials
{
  String ssid;
  String password;
};

struct Envoy
{
  String host;  // IP de l'Envoy
  String port;  // port de l'envoy
  String type;  // type de l'envoy
  String mail;  // mail de l'indentifiant Enphase
  String pswd;  // pssw de l'indentifiant Enphase
  String token; // token
};

Credentials credentials;
Envoy envoy;

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
  Serial.printf("[envoyTask]  ligne %d https://", __LINE__);
  Serial.println(adr + url);

  if (https.begin("https://" + adr + url))
  {
    Serial.printf("[envoyTask] --------  ligne %d Begin OK \n", __LINE__);
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

      Serial.printf("[envoyTask] --------  ligne %d Payload : lg %d \n%s\n", __LINE__, payload.length(), payload.c_str());

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
    Serial.printf("[Enphase_get_7_JWT] %d httpCode : %d \n", __LINE__, httpCode);

    // httpCode will be negative on error
    // Serial.println("Enphase_Get_7 : httpcode : " + httpCode);
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        retour = true;
        // Token valide
        Serial.println("[Enphase_get_7_JWT] Enphase contrôle tocket : TOKEN VALIDE ");
        SessionId.clear();
        SessionId = https.header("Set-Cookie");
      }
      else
      {
        Serial.println("[Enphase_get_7_JWT] Enphase contrôle tocket : TOKEN INVALIDE !!!");
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
      Serial.printf("[Enphase_get_7] Enphase version 7, token : %s \n", envoy.token.c_str());
      Enphase_get_7_JWT();
    }
  }
  else
  {
    Serial.println("Enphase version 7 : ERROR");
  }
}

void serial_read()
{
  String message_get;
  int watchdog;

  // while (Serial.available() > 0 || watchdog > 256)
  while (Serial.available() > 0)
  {
    message_get = Serial.readStringUntil('\n');
    message_get.replace("\n", "");
    message_get.replace("\r", "");
    watchdog++;
  }

  if (message_get.length() != 0)
  {
    /// test du message
    int index = message_get.indexOf("reboot");
    if (index != -1)
    {
      Serial.println("commande reboot reçue");
      ESP.restart();
    }

    index = message_get.indexOf("razwifi");
    if (index != -1)
    {
      Serial.println("commande raz wifi reçue");
      prefWifi.clear();
      delay(2000);
      Serial.println("reboot suite au raz des préférences");
      ESP.restart();
    }

    index = message_get.indexOf("razenvoy");
    if (index != -1)
    {
      Serial.println("commande raz envoy reçue");
      prefEnvoy.clear();
      delay(2000);
      Serial.println("reboot suite au raz des préférences");
      ESP.restart();
    }

    index = message_get.indexOf("ssid");
    if (index != -1)
    {
      credentials.ssid = message_get.substring(5, message_get.length());
      Serial.println("ssid enregistré: " + credentials.ssid);
      prefWifi.putString("ssid", credentials.ssid);
      return;
    }

    index = message_get.indexOf("pass");
    if (index != -1)
    {
      credentials.password = message_get.substring(5, message_get.length());
      Serial.println("password enregistré: " + credentials.ssid);
      prefWifi.putString("password", credentials.password);
      return;
    }

    index = message_get.indexOf("mail");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.mail = message_get.substring(5, message_get.length());
      Serial.println("mail enregistré: " + envoy.mail);
      prefEnvoy.putString("mail", envoy.mail);
      prefEnvoy.end();
      return;
    }

    index = message_get.indexOf("pswd");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.pswd = message_get.substring(5, message_get.length());
      Serial.println("pswd enregistré: " + envoy.pswd);
      prefEnvoy.putString("pswd", envoy.pswd);
      prefEnvoy.end();
      return;
    }

    index = message_get.indexOf("token");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.token = message_get.substring(5, message_get.length());
      envoy.token.trim();
      Serial.println("token enregistré: " + envoy.token);
      prefEnvoy.putString("token", envoy.token);
      prefEnvoy.end();
      return;
    }

    index = message_get.indexOf("host");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.host = message_get.substring(5, message_get.length());
      Serial.println("host enregistré: " + envoy.host);
      prefEnvoy.putString("host", envoy.host);
      prefEnvoy.end();
      return;
    }

    index = message_get.indexOf("port");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.port = message_get.substring(5, message_get.length());
      Serial.println("port enregistré: " + envoy.port);
      prefEnvoy.putString("port", envoy.port);
      prefEnvoy.end();
      return;
    }

    index = message_get.indexOf("type");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.type = message_get.substring(5, message_get.length());
      Serial.println("type enregistré: " + envoy.type);
      prefEnvoy.putString("type", envoy.type);
      prefEnvoy.end();
      return;
    }

    if (message_get.length() != 0)
    {
      Serial.println("Commande disponibles :");
      Serial.println("'reboot' pour redémarrer le routeur ");
      Serial.println("'razwifi' pour effacer les paramétres wifi ");
      Serial.println("'razenvoy' pour effacer les paramétres envoy ");
      Serial.println("'ssid' pour changer le SSID wifi");
      Serial.println("'pass' pour changer le mdp wifi");
      Serial.println("'mail' pour changer le mail de la connection Enphase");
      Serial.println("'pswd' pour changer le mdp de la connection Enphase");
      Serial.println("'token' pour changer le token");
      Serial.println("'log' pour afficher les logs serial");
    }
  }
}

void setup()
{
  Serial.begin(115200);
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
  Serial.println("Credentials Saved using Preferences");
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
  prefEnvoy.end();

  Serial.printf("[SETUP] ligne %d host : %s \n", __LINE__, envoy.host.c_str());
  Serial.printf("[SETUP] ligne %d port : %s \n", __LINE__, envoy.port.c_str());
  Serial.printf("[SETUP] ligne %d type : %s \n", __LINE__, envoy.type.c_str());
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
  }
  delay(2000);
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
  serial_read();
  delay(2000);
}