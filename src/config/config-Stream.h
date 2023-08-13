#ifndef CONFIGSTREAM
#define CONFIGSTREAM

#include <Arduino.h>

#define debug true

#define EnvoyJ "/auth/check_jwt"
#define EnvoyR "/api/v1/production"
#define EnvoyS "/production.json"
#define EnvoyStream "/stream/meter"

#define EnphaseServ "entrez.enphaseenergy.com"
#define EnphaseLogin "/login" // Server URL
#define EnphaseTokens "/entrez_tokens"
#define EnphaseJwt "/auth/check_jwt"
#define EnphaseStream "/stream/meter"

// It's quick and easy but all these variables shouldn't be public. It's a pain to provider getters for each one but that would
// prevent accidental modification of these variables. Or, the developer can be careful not to make a mistake. Those never happen!

String server = "entrez.enphaseenergy.com"; // Server URL
String server2;
String url = "https://entrez.enphaseenergy.com/login"; // Server URL
String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
String url_3;
String url_4;

String enphase_user; //
String enphase_pwd;  //
String enphase_entez;
String enphase_serial;
String data2;
String data1;
String token1;
String JWTTokentoken;
String sessionID_local;

//----------------------------------------------------------
//Datas propre à l'envoy stockées en flash
//----------------------------------------------------------

struct Envoy
{
  Preferences prefEnvoy;

  String host;   // IP de l'Envoy
  String port;   // port de l'envoy
  String type;   // type de l'envoy
  String mail;   // mail de l'indentifiant Enphase
  String pswd;   // pssw de l'indentifiant Enphase
  String token;  // token
  String serial; // numéro de série de l'Envoy
  uint32_t token_timestamp; //permet de stocker la date unix de la récupération du token //TODO pour version future avec NTP

public:
  bool recup_envoy()
  {
    prefEnvoy.begin("envoy", false);

    String tmp;
    host = prefEnvoy.getString("host", "10.190.2.120");
    port = prefEnvoy.getString("port", "80");
    mail = prefEnvoy.getString("mail", "envoy@enphase.tutu");
    pswd = prefEnvoy.getString("pswd", "pass-a-saisir");
    token = prefEnvoy.getString("token", "hkjhgJhgjFjfjGJfJGJHvgJH");
    serial = prefEnvoy.getString("serial", "123324556");

    prefEnvoy.end();
    return true;
  }

public:
  bool raz_envoy()
  {
    prefEnvoy.begin("envoy", false);
    prefEnvoy.clear();
    prefEnvoy.end();
    return true;
  }

public:
  bool sauve_envoy()
  {
    prefEnvoy.begin("envoy", false);
    prefEnvoy.putString("host", host);
    prefEnvoy.putString("port", port);
    prefEnvoy.putString("mail", mail);
    prefEnvoy.putString("pswd", pswd);
    prefEnvoy.putString("token", token);
    prefEnvoy.putString("serial", serial);

    prefEnvoy.end();
  Serial.printf("[sauve_envoy] ligne %d host : %s \n", __LINE__, host.c_str());
  Serial.printf("[sauve_envoy] ligne %d port : %s \n", __LINE__, port.c_str());
  Serial.printf("[sauve_envoy] ligne %d type : %s \n", __LINE__, type.c_str());
  Serial.printf("[sauve_envoy] ligne %d mail : %s \n", __LINE__, mail.c_str());
  Serial.printf("[sauve_envoy] ligne %d pswd : %s \n", __LINE__, pswd.c_str());
  Serial.printf("[sauve_envoy] ligne %d serial : %s \n", __LINE__, serial.c_str());
  Serial.printf("[sauve_envoy] ligne %d token : %s \n", __LINE__, token.c_str());
  Serial.printf("[sauve_envoy] ligne %d date du token : %d \n", __LINE__, token_timestamp);

    return true;
  }
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

#endif