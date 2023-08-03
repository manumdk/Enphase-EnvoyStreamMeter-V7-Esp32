#ifndef CONFIGSTREAM
#define CONFIGSTREAM

#include <Arduino.h>

#define debug true


// It's quick and easy but all these variables shouldn't be public. It's a pain to provider getters for each one but that would
// prevent accidental modification of these variables. Or, the developer can be careful not to make a mistake. Those never happen!



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