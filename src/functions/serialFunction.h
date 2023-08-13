
#ifndef SERIAL_FUNCTIONS
#define SERIAL_FUNCTIONS

#include <Arduino.h>
#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config-Stream.h"
extern Configwifi configwifi;
extern Envoy envoy;

bool bExit = false;

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
      configwifi.raz_wifi();
      delay(2000);
      Serial.println("reboot suite au raz des préférences");
      ESP.restart();
    }

    index = message_get.indexOf("razenvoy");
    if (index != -1)
    {
      Serial.println("commande raz envoy reçue");
      envoy.raz_envoy();
      delay(2000);
      Serial.println("reboot suite au raz des préférences");
      ESP.restart();
    }

    index = message_get.indexOf("ssid");
    if (index != -1)
    {
      message_get = message_get.substring(5, message_get.length());
      message_get.toCharArray( configwifi.SID,message_get.length()+1);
      Serial.println("ssid enregistré: " + String(configwifi.SID));
      configwifi.sauve_wifi(); 
      return true;
    }

    index = message_get.indexOf("pass");
    if (index != -1)
    {
      message_get = message_get.substring(5, message_get.length());
      message_get.toCharArray( configwifi.passwd,message_get.length()+1);      
      Serial.println("password enregistré: " + String(configwifi.passwd));
      configwifi.sauve_wifi(); 
      return true;
    }

    index = message_get.indexOf("mail");
    if (index != -1)
    {
      envoy.mail = message_get.substring(5, message_get.length());
      Serial.println("mail enregistré: " + envoy.mail);
      envoy.sauve_envoy();
      return true;
    }

    index = message_get.indexOf("pswd");
    if (index != -1)
    {
      envoy.pswd = message_get.substring(5, message_get.length());
      Serial.println("pswd enregistré: " + envoy.pswd);
      envoy.sauve_envoy();
      return true;
    }

    index = message_get.indexOf("token");
    if (index != -1)
    {
      envoy.token = message_get.substring(5, message_get.length());
      envoy.token.trim();
      Serial.printf("\n*************\ntoken enregistré:\n%s\n *********** \n", envoy.token.c_str());
      envoy.sauve_envoy();
      return true;
    }

    index = message_get.indexOf("host");
    if (index != -1)
    {
      envoy.host = message_get.substring(5, message_get.length());
      Serial.println("host enregistré: " + envoy.host);
      envoy.sauve_envoy();
      return true;
    }

    index = message_get.indexOf("port");
    if (index != -1)
    {
      envoy.port = message_get.substring(5, message_get.length());
      Serial.println("port enregistré: " + envoy.port);
      envoy.sauve_envoy();
      return true;
    }

    index = message_get.indexOf("type");
    if (index != -1)
    {
      envoy.type = message_get.substring(5, message_get.length());
      Serial.println("type enregistré: " + envoy.type);
      envoy.sauve_envoy();
      return true;
    }
    index = message_get.indexOf("serial");
    if (index != -1)
    {
      envoy.serial = message_get.substring(7, message_get.length());
      Serial.println("serial enregistré: " + envoy.serial);
      envoy.sauve_envoy();
      return true;
    }
    index = message_get.indexOf("LOG");
    if (index != -1)
    {
      Serial.println("Activation Log série");
      bLog = true;
      return false;
    }
    index = message_get.indexOf("log");
    if (index != -1)
    {
      Serial.println("Stop Log série");
      bLog = false;
      return false;
    }

    index = message_get.indexOf("quit");
    if (index != -1)
    {
      Serial.println("Quitte le menu ");
      bExit = true;
      return false;
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
  Serial.println("Commandes disponibles :");
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
    if (millis() - now > 3000)
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