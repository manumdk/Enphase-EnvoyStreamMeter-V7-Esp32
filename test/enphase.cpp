#include <WiFiClientSecure.h>

const char *ssid = « babnat »;                                            // your network SSID (name of wifi network)<<<<<<<<----- ssid de ton wifi
const char *password = « J61_ < XXXXXXXXXXXXXXXXXXXXXXXXXXmewBdna7 - E »; // your network password<<<<<<<<<------- clef de ton wifi

const char *server = « entrez.enphaseenergy.com »; // Server URL
const char *server2 = « 192.168.0.2 »;             // Server URL
String url = « https :                             // entrez.enphaseenergy.com/login »; // Server URL
                       String url_2 = « Enphase Authentication | Home »;
String url_3 = « https :                          // 192.168.0.2/auth/check_jwt »;<<<<<<---------------
                         String url_4 = « https : //{{envoyaddress}}/stream/meter »;
                                                  String enphase_user = "nXXXXXXX@free.fr";
< < < < < < < < -- -- -- -- -- -- -- -- -non utilisateur enphase
                                            String enphase_pwd = « XXXXXXXXXXX »;
< < < < < < < -- -- -- -- -- -- -- -- -- --mot de passe enphase
                                             String enphase_entez = « authFlow = entrezSession »;
String enphase_serial = « 122012XXXXXX »;
String data2 = « serialNum = »+ enphase_serial;
String data1 = « username = »+ enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
String token1 = « Set - Cookie : SESSION = NTgzOWIwYTYtMjNmMC00OTZjLWI4NGUtMmQ5YzRiOTg1Yjdk;
»;
< < < < < < c’est juste pour créer la variable c’est modifié aprés la requette

                WiFiClientSecure client;

void setup()
{
    // Initialize serial and wait for port to open:
    Serial.begin(115200);
    delay(100);

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());

    Serial.println("\nStarting connection to Enphae server…1");
    client.setInsecure(); // skip verification

    if (!client.connect(server, 443))
        Serial.println(« Connection failed ! »);
    else
    {
        Serial.println(« Connected to Enphase server 1 ! »);
        client.println("POST « + url + " HTTP / 1.1 »);
client.println(« User-Agent: got (GitHub - sindresorhus/got: 🌐 Human-friendly and powerful HTTP request library for Node.js) »);
client.println(« Content - Type
               : application / x - www - form - urlencoded »);
client.print(F("Content-Length: "));
client.println(data1.length());
// Serial.println(data.length());
client.println(« Host
               : entrez.enphaseenergy.com »);
client.println(« Connection
               : close »);
client.println();
client.println(data1);

while (client.connected())
{
    String line = client.readStringUntil('\n');
    // Serial.println("section client.connected ");
    // Serial.println(line);

    char linechar[line.length() + 1];
    line.toCharArray(linechar, line.length() + 1); // récupère le param dans le tableau de char

    // String SESSION = "SESSION";
    const char *lineconstchar = linechar;
    // Serial.println(lineconstchar);
    if (!strstr(lineconstchar, "SESSION"))
    {
        // Serial.println("ce n'est pas la ligne session");
    }
    else
    {
        // Serial.println("c'est la ligne session");
        // Serial.println(lineconstchar);
        // Serial.println(line.indexOf("SESSION"));
        // Serial.println(line.indexOf(";"));
        // Serial.println(line.substring(line.indexOf("SESSION"), line.indexOf(";")));
        token1 = line.substring(line.indexOf("SESSION"), line.indexOf(";"));
    }

    // Serial.println(session);

    /*if (line == "\r") {
      Serial.println("headers received");
      break;
    }*/
}
// if there are incoming bytes available
// from the server, read them and print them:
/*while (client.available()) {
  //Serial.println("section client.avaible ");
  char c = client.read();
  //Serial.write(c);
}*/
Serial.println(token1);
////premiere partie ok

client.stop();
    }
    // deuxiéme partie
    delay(3000);

    Serial.println("\nStarting connection to Enphae server…2");
    client.setInsecure(); // skip verification

    if (!client.connect(server, 443))
Serial.println(« Connection failed ! »);
    else
    {
Serial.println(« Connected to Enphase server 2 ! »);
client.println("POST « + url_2 + " HTTP / 1.1 »);
// Serial.println("POST « + url_2 + " HTTP/1.1 »);
client.println(« User-Agent: got (GitHub - sindresorhus/got: 🌐 Human-friendly and powerful HTTP request library for Node.js) »);
// Serial.println(« User-Agent: got (GitHub - sindresorhus/got: 🌐 Human-friendly and powerful HTTP request library for Node.js) »);
client.println("Cookie: " + token1);
// Serial.println("Cookie: "+ token1);
client.print(F("Content-Length: "));
// Serial.println(F("Content-Length: "));
client.println(data2.length());
// Serial.println(data2.length());
client.println(« Content - Type
               : application / x - www - form - urlencoded »);
// Serial.println(« Content-Type: application/x-www-form-urlencoded »);
//  Serial.println(data.length());
client.println(« Host
               : entrez.enphaseenergy.com »);
// Serial.println(« Host: entrez.enphaseenergy.com »);
client.println(« Connection
               : close »);
// Serial.println(« Connection: close »);
client.println();
client.println(data2);
// Serial.println(data2);

while (client.connected())
{
    String line = client.readStringUntil('\n');
    // Serial.println(line);

    // Serial.println(session);

    if (line == "\r")
    {
        // Serial.println("headers received");
        break;
    }
}

// if there are incoming bytes available
// from the server, read them and print them:
while (client.available())
{
    // Serial.println("section client.avaible ");
    // char c = client.read();
    // Serial.write(c);
    String line = client.readStringUntil('\n');
    // Serial.println(line);

    char linechar[line.length() + 1];
    line.toCharArray(linechar, line.length() + 1); // récupère le param dans le tableau de char

    // String SESSION = "SESSION";
    const char *lineconstchar = linechar;

    if (!strstr(lineconstchar, "<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >"))
    {
        // Serial.println("ce n'est pas la ligne session");
    }
    else
    {
        Serial.println(line);
        Serial.println("c'est la ligne token");
        // Serial.println(lineconstchar);
        // Serial.println(line.indexOf("SESSION"));
        // Serial.println(line.indexOf(";"));
        Serial.println(line.substring(line.indexOf("<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >"), line.indexOf("</tex")));
        // token1 = line.substring(line.indexOf("<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >"), line.indexOf("<"));
    }
}
// Serial.println(token1);
////premiere partie ok

client.stop();
    }
}

void loop()
{
    // do nothing
}