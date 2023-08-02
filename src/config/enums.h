#ifndef ENUMS
#define ENUMS

#include <Arduino.h>
#include <Preferences.h>

// The state in which the device can be. This mainly affects what
// is drawn on the display.
enum DEVICE_STATE
{
  CONNECTING_WIFI,
  CONNECTING_AWS,
  FETCHING_TIME,
  UP,
  DOWN,
  AP_MODE,
};

bool bLog = false;
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


// struct Configwifi  // TODO pour plus tard
// {

//   Preferences preferences;

//   char SID[32];
//   char passwd[64];

// public:
//   bool sauve_wifi()
//   {
//     preferences.begin("credentials", false);
//     preferences.putString("ssid", SID);
//     preferences.putString("password", passwd);
//     preferences.end();
//     return true;
//   }

// public:
//   bool recup_wifi()
//   {
//     preferences.begin("credentials", false);
//     String tmp;
//     tmp = preferences.getString("ssid", "AP");
//     tmp.toCharArray(SID, 32);
//     tmp = preferences.getString("password", "");
//     tmp.toCharArray(passwd, 64);
//     preferences.end();
//     if (strcmp(SID, "") == 0)
//     {
//       return false;
//     }
//     return true;
//   }
// };


#if DEBUG == true
#define serial_print(x) Serial.print(x)
#define serial_printf(x) Serial.printf(x)
#define serial_println(x) Serial.println(x)
#else
#define serial_print(x)
#define serial_printf(x)
#define serial_println(x)
#endif

#endif
