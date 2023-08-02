#ifndef CONFIGSTREAM
#define CONFIGSTREAM

#include <Arduino.h>
// #include "lwip/err.h"
// #include "lwip/dns.h"


#define debug true

int inverterWatts(String serialnumber);
int inverterWatts(int index);
int inverterMaxWatts(String serialnumber);
int inverterMaxWatts(int index);
String SerialNumber(int index);

//It's quick and easy but all these variables shouldn't be public. It's a pain to provider getters for each one but that would
//prevent accidental modification of these variables. Or, the developer can be careful not to make a mistake. Those never happen!

enum Phase{
    pha,
    phb,
    phc
};

struct MesurePhase {
    double p[3];
    double q[3];
    double s[3];
    double v[3];
    double i[3];
    double pf[3];
    double f[3];
};

struct DataEnvoy{
MesurePhase Prod;
MesurePhase Net;
MesurePhase Conso;
};


//stream/meter variables
double Apower;
double Avoltage;
double Acurrent;
double pf;
double freq;
double Bpower;
double Bvoltage;
double Bcurrent;
double Bpf;
double Bfreq;
double CApower;
double CBpower;
double NApower;
double NBpower;
double Power;
double Consumption;
double NetPower;

//production.json variables
double ProdPower;
double rmsCurrent;
double rmsVoltage;
double kWhToday;
double kWhLastSevenDays;
double MWhLifetime;
double ConPower;
double ConrmsCurrent;
double ConrmsVoltage;
double ConkWhToday;
double ConkWhLastSevenDays;
double ConMWhLifetime;
double NetConPower;

//inventory variables
String serialNumber[200];
int Watts[200];
int MaxWatts[200];
double InvTotalPower = 0;
double InvTotalMaxPower = 0;
int numInverters;
IPAddress envoyIP;

// String extractParam(String &authReq, const String &param, const char delimit);
// String getCNonce(const int len);
// String getDigestAuth(String &authReq, const String &username, const String &password, const String &uri, unsigned int counter);
// String serverString;
// String authorizationMeter;

// char installerPassword[20];
// char envoyName[20] = "envoy";       //the .local is automatically added so basically never explicitly do so anywhere.
//                                     //In multiple Envoy systems this would be envoy, envoy2, envoy3, etc.
// const char *username = "installer"; //login is always installer
// IPAddress IP_envoy = IPAddress(192, 168, 10, 179); // A déclarer si pb de DNS local sinon 0.0.0.0
float AReal_power = 0;
float AReactive_power = 0;
float AApparent_power = 0;

float BReal_power = 0;
float BReactive_power = 0;
float BApparent_power = 0;

float CReal_power = 0;
float CReactive_power = 0;
float CApparent_power = 0;
float Cvoltage = 0;
float Ccurrent = 0;

float tcAReal_power = 0;
float tcAReactive_power = 0;
float tcAApparent_power = 0;
float tcAvoltage = 0;
float tcAcurrent = 0;

float tcBReal_power = 0;
float tcBReactive_power = 0;
float tcBApparent_power = 0;
float tcBvoltage = 0;
float tcBcurrent = 0;

float tcCReal_power = 0;
float tcCReactive_power = 0;
float tcCApparent_power = 0;
float tcCvoltage = 0;
float tcCcurrent = 0;

#endif