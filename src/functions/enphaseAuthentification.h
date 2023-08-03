#ifndef AUTH_FUNCTIONS
#define AUTH_FUNCTIONS

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config-Stream.h"

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

extern Envoy envoy;

const char *ssid = "xxxxxxx";                                                           // your network SSID (name of wifi network)     <----------------------
const char *password = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // your network password  <----------------------

const char *server = "entrez.enphaseenergy.com";       // Server URL
const char *server2 = "192.168.10.179";                // Server URL
String url = "https://entrez.enphaseenergy.com/login"; // Server URL
String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
String url_3 = "https://" + String(server2) + "/auth/check_jwt";
String url_4 = "https://" + String(server2) + "/stream/meter";

String enphase_entez = "authFlow=entrezSession";
// String enphase_serial = "xxxxxxxx";    //
String data2 = "serialNum=" + envoy.serial;
String data1 = "username=" + envoy.mail + "&password=" + envoy.pswd + "&" + enphase_entez;
String token1 = "Set-Cookie: SESSION=NTgzOWIwYTYtMjNmMC00OTZjLWI4NGUtMmQ5YzRiOTg1Yjdk;";
String JWTTokentoken = "\"JWTToken\" cols=\"30\" rows=\"10\" >eyJraWQiOiI3ZDEwMDA1ZC03ODk5LTRkMGQtYmNiNC0yNDRmOThlZTE1NmIiLCJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9.eyJhdWQiOiIxMjIwMTIxMDM1ODMiLCJpc3MiOiJFbnRyZXoiLCJlbnBoYXNlVXNlciI6Imluc3RhbGxlciIsImV4cCI6MTY5MDc3OTgyMSwiaWF0IjoxNjkwNzM2NjIxLCJqdGkiOiJmYWFkNTUwNC02MDI2LTQ0NGQtODRhZC0yOWZkYjJlZTQ3N2MiLCJ1c2VybmFtZSI6Im5hdGhhbi53YWlsbHlAZnJlZS5mciJ9.rD-vASYfTMtTxBs0MgobfNBhVx5BNgRBAvOYYFbtsfp8wMIqI09hTo9VajKXCDTYG-XUITzfRjTGGDzj";
String sessionID_local = "et-Cookie: sessionId=81dLa0vZLeST5yfbAqsTbELINEajtcb6; Secure; HttpOnly; pat";

WiFiClientSecure client;

void setup_Auth()
{
  client.setInsecure(); // skip verification
  Serial.printf("[authEnphase] ligne %d data1 : %s\n", __LINE__, data1.c_str());

  // Serial.printf("[authEnphase] ligne %d data2 : %s\n", __LINE__, data2.c_str());

  Serial.printf("[authEnphase] ligne %d \n", __LINE__);
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else
  {
    Serial.printf("[authEnphase] ligne %d Connected to Enphase server 1!\n", __LINE__);
    client.println("POST " + url + " HTTP/1.1");
    client.println("User-Agent: got (https://github.com/sindresorhus/got)");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print(F("Content-Length: "));
    client.println(data1.length());
    client.println("Host: entrez.enphaseenergy.com");
    client.println("Connection: close");
    client.println();
    client.println(data1);

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

  if (!client.connect(server, 443))
  {
    Serial.printf("\n[authEnphase] ligne %d Connection failed!  \n", __LINE__);
  }
  else
  {
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