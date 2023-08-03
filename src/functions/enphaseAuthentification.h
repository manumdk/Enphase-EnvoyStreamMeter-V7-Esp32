#ifndef AUTH_FUNCTIONS
#define AUTH_FUNCTIONS

// #include <Arduino.h>
// #include <ArduinoJson.h>

#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config-Stream.h"

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

extern Envoy envoy;

// const char *server = "entrez.enphaseenergy.com";       // Server URL
// const char *server2 = "192.168.10.179";                // Server URL
// String url = "https://entrez.enphaseenergy.com/login"; // Server URL
// String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
// String url_3 = "https://" + String(server2) + "/auth/check_jwt";
// String url_4 = "https://" + String(server2) + "/stream/meter";
// String enphase_entez = "authFlow=entrezSession";
// // String enphase_serial = "xxxxxxxx";    //

// String data1 = "username=" + envoy.mail + "&password=" + envoy.pswd + "&" + enphase_entez;
// String data2 = "serialNum=" + envoy.serial;
// String token1;
// String JWTTokentoken;
// String sessionID_local;

String server = "entrez.enphaseenergy.com";            // Server URL
const char *server2 = envoy.host.c_str();                // Server URL
String url = "https://entrez.enphaseenergy.com/login"; // Server URL
String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
String url_3 = "https://" + String(server2) + "/auth/check_jwt";
String url_4 = "https://" + String(server2) + "/stream/meter";

String enphase_user =  envoy.mail; //
String enphase_pwd =  envoy.pswd;//
String enphase_entez = "authFlow=entrezSession";
String enphase_serial = envoy.serial;
// String data2 = "serialNum=" + enphase_serial;
// String data1 = "username=" + enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
String token1 ;
String JWTTokentoken;
String sessionID_local ;

WiFiClientSecure client;

void setup_Auth()
{
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
String data1 = "username=" + enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
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
    Serial.println(data1.c_str());
    while (client.connected())
    {
      String line = client.readStringUntil('\n');
      Serial.printf("[authEnphase] ligne %d  linechar %s \n", __LINE__, line.c_str());
      char linechar[line.length() + 1];
      line.toCharArray(linechar, line.length() + 1);
      const char *lineconstchar = linechar;

      if (!strstr(lineconstchar, "SESSION"))
      {
        Serial.printf("[authEnphase] ligne %d  Erreur Session \n", __LINE__);
      }
      else
      {
        Serial.printf("[authEnphase] ligne %d  Token 1 \n", __LINE__);
        token1 = line.substring(line.indexOf("SESSION"), line.indexOf(";"));
      }
    }

    Serial.printf("[authEnphase] ligne %d token1 : %s \n", __LINE__, token1.c_str());

    client.stop();
  }
  // deuxiéme partie
  delay(100);

  // Serial.println("\nStarting connection to Enphae server...2");
  if (bLog)
    Serial.printf("\n[authEnphase] ligne %d Starting connection to Enphae server...2 :  \n", __LINE__);

  client.setInsecure(); // skip verification

  if (!client.connect(server.c_str(), 443))
  {
    Serial.printf("\n[authEnphase] ligne %d Connection failed!  \n", __LINE__);
  }
  else
  {
    Serial.printf("[authEnphase] ligne %d Connected to Enphase server 2!  \n", __LINE__);
    String data2 = "serialNum=" + enphase_serial;
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
        Serial.printf("[authEnphase] ligne %d Pas de token \n", __LINE__);
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

  // delay(100);

  // Serial.printf("\n[authEnphase] Starting connection to Enphae server...3 \n", __LINE__);

  // client.setInsecure(); // skip verification

  // if (!client.connect(server2, 443))
  //   Serial.printf("[authEnphase] ligne %d Connection failed! \n", __LINE__);

  // else
  // {
  //   Serial.printf("[authEnphase] ligne %d Connection to Envoy 1 \n", __LINE__);
  //   client.println("GET " + url_3 + " HTTP/1.1");
  //   client.println("User-Agent: got (https://github.com/sindresorhus/got)");
  //   client.println("Authorization: Bearer " + JWTTokentoken);
  //   client.println("Host: 192.168.0.2");
  //   client.println("Connection: close");
  //   client.println();

  //   while (client.connected())
  //   {
  //     String line = client.readStringUntil('\n');

  //     char linechar[line.length() + 1];
  //     line.toCharArray(linechar, line.length() + 1); // récupère le param dans le tableau de char

  //     const char *lineconstchar = linechar;

  //     if (!strstr(lineconstchar, "Set-Cookie: sessionId="))
  //     {
  //     }
  //     else
  //     {
  //       sessionID_local = (line.substring(line.indexOf("sessionId="), line.indexOf(";")));
  //       // Serial.println(sessionID_local );
  //       Serial.printf("[authEnphase] sessionID_local : %s \n", __LINE__, sessionID_local);
  //     }

  //     if (line == "\r")
  //     {
  //       break;
  //     }
  //   }
  //   while (client.available())
  //   {
  //     String line = client.readStringUntil('\n');
  //     // Serial.println(line);
  //   }

  //   client.stop();
  // }

  // delay(100);

  // int httpCode;

  // if (https.begin(url_4))
  // {
  //   Serial.println("Connected to Evoy /stream/meter!");
  //   https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  //   https.addHeader("Accept-Encoding", "gzip, deflate, br");
  //   https.addHeader("User-Agent", "PvRouter/1.1.1");
  //   https.addHeader("Cookie ", sessionID_local);
  //   https.addHeader("Authorization", "Bearer " + JWTTokentoken);
  //   https.setReuse(true);
  //   httpCode = https.GET();

  //   while (httpCode == 200)
  //   {
  //     String payload;
  //     WiFiClient *cl = https.getStreamPtr();
  //     payload = cl->readStringUntil('\n');
  //     Serial.println(payload);
  //   }
  // }
}

#endif