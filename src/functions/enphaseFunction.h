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

  Serial.printf("[envoyTask] ligne %d http://", __LINE__);
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
    String payload;
    WiFiClient *cl = https.getStreamPtr();
    int error = 0;
    do
    {
      cl->find("data: ");
      payload = cl->readStringUntil('\n');
      // cl->flush();
      // if (payload.length() > 5)
      error = cl->available();

      Serial.printf("[envoyTask] ligne %d errors %d Payload : lg %d \n%s\n", __LINE__, error, payload.length(), payload.c_str());
      processingJsondata(payload);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    } while (error);
    cl->stop();
    https.end();

    retour = true;
#ifdef DEBUG
    Serial.printf("[envoyTask] ligne %d fin de la réception\n", __LINE__);
#endif
    serial_read();
    vTaskDelay(200 / portTICK_PERIOD_MS);

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

void Enphase_get_7_Token()
{

  String server = "entrez.enphaseenergy.com";            // Server URL
  const char *server2 = envoy.host.c_str();              // Server URL
  String url = "https://entrez.enphaseenergy.com/login"; // Server URL
  String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
  String url_3 = "https://" + String(server2) + "/auth/check_jwt";
  String url_4 = "https://" + String(server2) + "/stream/meter";

  String enphase_user = envoy.mail; //
  String enphase_pwd = envoy.pswd;  //
  String enphase_entez = "authFlow=entrezSession";
  String enphase_serial = envoy.serial;
  String data2 = "serialNum=" + enphase_serial;
  String data1 = "username=" + enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
  String token1;
  String JWTTokentoken;
  String sessionID_local;

  // Serial.printf("[authEnphase] ligne %d data1 : %s\n", __LINE__, data1.c_str());
  // Serial.printf("[authEnphase] ligne %d data2 : %s\n", __LINE__, data2.c_str());

  Serial.printf("[authEnphase] ligne %d \n", __LINE__);
  client.setInsecure(); // skip verification

  if (!client.connect(server.c_str(), 443))
  {
    Serial.println("Connection failed!");
  }

  else
  {
    client.setInsecure(); // skip verification
                          // String data = String("username=") + enphase_user + String("&password=") + enphase_pwd + String("&") + enphase_entez;

    Serial.println("Connected to Enphase server 1!");
    client.println("POST " + url + " HTTP/1.1");
    client.println("User-Agent: got (https://github.com/sindresorhus/got)");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print(F("Content-Length: "));
    client.println(data1.length());
    client.println("Host: entrez.enphaseenergy.com");
    client.println("Connection: close");
    client.println();
    client.println(data1);
    Serial.printf("[authEnphase] ligne %d data : %s \n", __LINE__, data1.c_str());

    while (client.connected())
    {
      String line;
      line = client.readStringUntil('\n');

      char linechar[line.length() + 1];
      line.toCharArray(linechar, line.length() + 1);
      const char *lineconstchar = linechar;

      if (!strstr(lineconstchar, "SESSION"))
      {
        // Serial.printf("[authEnphase] ligne %d  Erreur Session \n", __LINE__);
      }
      else
      {
        // Serial.printf("[authEnphase] ligne %d  Token 1 \n", __LINE__);
        token1 = line.substring(line.indexOf("SESSION"), line.indexOf(";"));
      }
    }
    Serial.printf("[authEnphase] ligne %d token1 : %s \n", __LINE__, token1.c_str());

    client.stop();
  }
  // deuxiéme partie
  delay(100);

  Serial.println("\nStarting connection to Enphae server...2");
  // if (bLog)
  Serial.printf("\n[authEnphase] ligne %d Starting connection to Enphae server...2 :  \n", __LINE__);

  client.setInsecure(); // skip verification

  if (!client.connect(server.c_str(), 443))
  {
    Serial.printf("\n[authEnphase] ligne %d Connection failed!  \n", __LINE__);
  }
  else
  {
    client.setInsecure(); // skip verification
    Serial.printf("[authEnphase] ligne %d Connected to Enphase server 2!  \n", __LINE__);
    client.println("POST " + url_2 + " HTTP/1.1");
    client.println("User-Agent: got (https://github.com/sindresorhus/got)");
    client.println("Cookie: " + token1);
    client.print(F("Content-Length: "));
    client.println(data2.length());
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Host: entrez.enphaseenergy.com");
    client.println("Connection: close");
    client.println();

    client.println(data2);
    Serial.printf("[authEnphase] ligne %d data2 : %s \n", __LINE__, data2.c_str());

    while (client.connected())
    {
      String line = client.readStringUntil('\n');

      if (line == "\r")
      {
        Serial.printf("[authEnphase] ligne %d line : %s \n", __LINE__, line.c_str());
        break;
      }
    }

    while (client.available())
    {
      String line = client.readStringUntil('\n');

      char linechar[line.length() + 1];
      line.toCharArray(linechar, line.length() + 1);

      const char *lineconstchar = linechar;

      if (!strstr(lineconstchar, "<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >"))
      {
        // Serial.printf("[authEnphase] ligne %d Pas de token \n", __LINE__);
      }
      else
      {
        int debut = 64;
        debut += line.indexOf("<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >");
        JWTTokentoken = (line.substring(debut, line.indexOf("</tex")));
        Serial.printf("[authEnphase] ligne %d JWTTokentoken : %s \n", __LINE__, JWTTokentoken.c_str());
      }
      envoy.token = JWTTokentoken;
    }

    client.stop();
  }
}

//-------------------------------------------------------------------------------"

void Enphase_get_7(void)
{

  if (WiFi.isConnected())
  {
    // create an HTTPClient instance
    // retour

    String sessionID_local;

    if (SessionId.isEmpty() || Enphase_get_7_Stream() == false)
    {                               // Permet de lancer le contrôle du token une fois au démarrage (Empty SessionId)
      server2 = envoy.host.c_str(); // Server URL

      url_3 = "https://" + server2 + "/auth/check_jwt";
      url_4 = "https://" + server2 + "/stream/meter";

      enphase_user = envoy.mail; //
      enphase_pwd = envoy.pswd;  //
      enphase_entez = "authFlow=entrezSession";
      enphase_serial = envoy.serial;
      data2 = "serialNum=" + enphase_serial;
      data1 = "username=" + enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
      SessionId.clear();
      Serial.printf("[Enphase_get_7] Enphase version 7, token : -%s- \n", envoy.token.c_str());
      if (Enphase_get_7_JWT() == false)
      {
        Enphase_get_7_Token();
      }
    }
  }
  else
  {
    Serial.println("Enphase version 7 : ERROR");
  }
}

#endif
