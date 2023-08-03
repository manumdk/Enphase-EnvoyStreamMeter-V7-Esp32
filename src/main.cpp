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
// #include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "HTTPClient.h"
#include <Preferences.h>
Preferences prefWifi;
Preferences prefEnvoy;
WiFiClientSecure client;
/* 1. Define the  credentials
At the first compilation plan to enter the credentials and the token
Information is saved in flash
*/

// #define DEBUG
// #ifdef DEBUG
// #define serial_print(x) Serial.print(x)
// #define serial.printf(x) Serial.printf(x)
// #define serial_println(x) Serial.println(x)
// #else
// #define serial_print(x)
// #define serial.printf(x)
// #define serial_println(x)
// #endif

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
  String host;   // IP de l'Envoy
  String port;   // port de l'envoy
  String type;   // type de l'envoy
  String mail;   // mail de l'indentifiant Enphase
  String pswd;   // pssw de l'indentifiant Enphase
  String token;  // token
  String serial; // numéro de série de l'Envoy
};

enum Phase
{
    pha, // Phase A
    phb, // Phase B
    phc  // Phase C
};

struct MesurePhase
{
    double p[3];  // Puissance Active
    double q[3];  // Puissance Réactive
    double s[3];  // Puissance Apparente
    double v[3];  // Tension
    double i[3];  // Intensité
    double pf[3]; // Coefficient
    double f[3];  // Fréquence
};

struct DataEnvoy
{
    MesurePhase Prod;  // Production des PV
    MesurePhase Net;   // Mesure vue au niveau Linky
    MesurePhase Conso; // Total consommé : Prod + Net
};


Credentials credentials;
Envoy envoy;





#define EnvoyJ "/auth/check_jwt"
#define EnvoyR "/api/v1/production"
#define EnvoyS "/production.json"
#define EnvoyStream "/stream/meter"

const char *enphase_conf = "/enphase.json";
bool bExit = false; // permet de gérer la sortie du menu

//////////////////////////////////// récupération des valeurs

HTTPClient https;
String SessionId;

//-------------------------------------------------------------------------------"


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
      if (payload.length() > 5)
        Serial.printf("[envoyTask] ligne %d Payload : lg %d \n%s\n", __LINE__, payload.length(), payload.c_str());

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
    Serial.printf("[Enphase_get_7_JWT] ligne %d httpCode : %d \n", __LINE__, httpCode);

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
  {client.setInsecure(); // skip verification
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
        //Serial.printf("[authEnphase] ligne %d Pas de token \n", __LINE__);
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
    if (SessionId.isEmpty() || Enphase_get_7_Stream() == false)
    { // Permet de lancer le contrôle du token une fois au démarrage (Empty SessionId)
      SessionId.clear();
      Serial.printf("[Enphase_get_7] Enphase version 7, token : %s \n", envoy.token.c_str());
      if (Enphase_get_7_JWT() == false){
        Enphase_get_7_Token();
      }
    }
  }
  else
  {
    Serial.println("Enphase version 7 : ERROR");
  }
}


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
  }
  delay(2000);

}

//-------------------------------------------------------------------------------"

void loop()
{
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