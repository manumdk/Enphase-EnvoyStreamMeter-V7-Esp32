
#ifndef SERIAL_FUNCTIONS
#define SERIAL_FUNCTIONS

#include <Arduino.h>
#include "../config/config.h"
#include "../config/enums.h"

extern Preferences prefWifi;
extern Preferences prefEnvoy;
extern Credentials credentials;



bool serial_read()
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
      return true;
    }

    index = message_get.indexOf("pass");
    if (index != -1)
    {
      credentials.password = message_get.substring(5, message_get.length());
      Serial.println("password enregistré: " + credentials.password);
      prefWifi.putString("password", credentials.password);
      return true;
    }

    index = message_get.indexOf("mail");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.mail = message_get.substring(5, message_get.length());
      Serial.println("mail enregistré: " + envoy.mail);
      prefEnvoy.putString("mail", envoy.mail);
      prefEnvoy.end();
      return true;
    }

    index = message_get.indexOf("pswd");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.pswd = message_get.substring(5, message_get.length());
      Serial.println("pswd enregistré: " + envoy.pswd);
      prefEnvoy.putString("pswd", envoy.pswd);
      prefEnvoy.end();
      return true;
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
      return true;
    }

    index = message_get.indexOf("host");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.host = message_get.substring(5, message_get.length());
      Serial.println("host enregistré: " + envoy.host);
      prefEnvoy.putString("host", envoy.host);
      prefEnvoy.end();
      return true;
    }

    index = message_get.indexOf("port");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.port = message_get.substring(5, message_get.length());
      Serial.println("port enregistré: " + envoy.port);
      prefEnvoy.putString("port", envoy.port);
      prefEnvoy.end();
      return true;
    }

    index = message_get.indexOf("type");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.type = message_get.substring(5, message_get.length());
      Serial.println("type enregistré: " + envoy.type);
      prefEnvoy.putString("type", envoy.type);
      prefEnvoy.end();
      return true;
    }
    index = message_get.indexOf("serial");
    if (index != -1)
    {
      prefEnvoy.begin("envoy", false);
      envoy.serial = message_get.substring(7, message_get.length());
      Serial.println("serial enregistré: " + envoy.serial);
      prefEnvoy.putString("serial", envoy.serial);
      prefEnvoy.end();
      return true;
    }
    index = message_get.indexOf("quit");
    if (index != -1)
    {
      Serial.println("Quitte le menu ");
      bExit = true;
      return  false;
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
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------------"


void menu_setup()
{
  bExit = false;
  Serial.println("-------------------------------------------------------------");
  Serial.println("-------------------------------------------------------------");
  Serial.println("Commande disponibles :");
  Serial.println("'reboot' pour redémarrer le routeur ");
  Serial.println("'razwifi' pour effacer les paramétres wifi ");
  Serial.println("'razenvoy' pour effacer les paramétres envoy ");
  Serial.println("'ssid' pour changer le SSID wifi");
  Serial.println("'pass' pour changer le mdp wifi");
  Serial.println("'mail' pour changer le mail de la connection Enphase");
  Serial.println("'pswd' pour changer le mdp de la connection Enphase");
  Serial.println("'token' pour changer le token");
  Serial.println("'serial' pour changer le numéro de série de l'enphase");
  Serial.println("'log' pour afficher les logs serial");
  Serial.println("'quit' pour quitter le setup");
  long now = millis();
  while (!bExit)
  {
    if (millis() - now > 10000)
    {
      bExit = true;
    }
    // serial_read();
    if (serial_read())
    {
      now = millis();
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

#endif