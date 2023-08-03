#include <WiFiClientSecure.h>
#include <HTTPClient.h>

const char *ssid = "****************";           // your network SSID (name of wifi network)
const char *password = "******************"; // your network password

String server = "entrez.enphaseenergy.com";            // Server URL
const char *server2 = "**************************";                // Server URL
String url = "https://entrez.enphaseenergy.com/login"; // Server URL
String url_2 = "https://entrez.enphaseenergy.com/entrez_tokens";
String url_3 = "https://" + String(server2) + "/auth/check_jwt";
String url_4 = "https://" + String(server2) + "/stream/meter";

String enphase_user = "********************"; //
String enphase_pwd = "******************";//
String enphase_entez = "authFlow=entrezSession";
String enphase_serial = "****************";
String data2 = "serialNum=" + enphase_serial;
String data1 = "username=" + enphase_user + "&password=" + enphase_pwd + "&" + enphase_entez;
String token1 ;
String JWTTokentoken;
String sessionID_local ;

WiFiClientSecure client;
HTTPClient https;

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

    // Serial.println("\nStarting connection to Enphae server...1");

    client.setInsecure(); // skip verification

    if (!client.connect(server.c_str(), 443))
    {
        Serial.println("Connection failed!");
    }

    else
    {
            client.setInsecure(); // skip verification

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

            char linechar[line.length() + 1];
            line.toCharArray(linechar, line.length() + 1);
            const char *lineconstchar = linechar;
            if (!strstr(lineconstchar, "SESSION"))
            {
            }
            else
            {
                token1 = line.substring(line.indexOf("SESSION"), line.indexOf(";"));
            }
        }

        Serial.println(token1.c_str());

        client.stop();
    }
    // deuxiéme partie
    delay(100);

    // Serial.println("\nStarting connection to Enphae server...2");
    client.setInsecure(); // skip verification

    if (!client.connect(server.c_str(), 443))
        Serial.println("Connection failed!");
    else
    {
            client.setInsecure(); // skip verification

        Serial.println("Connected to Enphase server 2!");
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
            }
            else
            {
                int debut = 64;
                debut += line.indexOf("<textarea name=\"accessToken\" id=\"JWTToken\" cols=\"30\" rows=\"10\" >");
                JWTTokentoken = (line.substring(debut, line.indexOf("</tex")));
                Serial.println(JWTTokentoken.c_str() );
            }
        }
  Serial.println(JWTTokentoken.c_str() );
        client.stop();
    }

    delay(100);

    // Serial.println("\nStarting connection to Enphae server...3");
    client.setInsecure(); // skip verification

    if (!client.connect(server2, 443))
        Serial.println("Connection failed!");
    else
    {
        Serial.println("Connected to envoy 1");
        client.println("GET " + url_3 + " HTTP/1.1");
        client.println("User-Agent: got (https://github.com/sindresorhus/got)");
        client.println("Authorization: Bearer " + JWTTokentoken);
        client.println("Host: " + String(server2));
        client.println("Connection: close");
        client.println();

        while (client.connected())
        {
            String line = client.readStringUntil('\n');

            char linechar[line.length() + 1];
            line.toCharArray(linechar, line.length() + 1); // récupère le param dans le tableau de char

            const char *lineconstchar = linechar;

            if (!strstr(lineconstchar, "Set-Cookie: sessionId="))
            {
            }
            else
            {
                sessionID_local = (line.substring(line.indexOf("sessionId="), line.indexOf(";")));
                // Serial.println(sessionID_local );
            }

            if (line == "\r")
            {
                break;
            }
        }
        while (client.available())
        {
            String line = client.readStringUntil('\n');
            // Serial.println(line);
        }

        client.stop();
    }

    delay(100);

    int httpCode;

    if (https.begin(url_4))
    {
        Serial.println("Connected to Evoy /stream/meter!");
        https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        https.addHeader("Accept-Encoding", "gzip, deflate, br");
        https.addHeader("User-Agent", "PvRouter/1.1.1");
        https.addHeader("Cookie ", sessionID_local);
        https.addHeader("Authorization", "Bearer " + JWTTokentoken);
        https.setReuse(true);
        httpCode = https.GET();

        while (httpCode == 200)
        {
            String payload;
            WiFiClient *cl = https.getStreamPtr();
            payload = cl->readStringUntil('\n');
            Serial.println(payload);
        }
    }
}

void loop()
{
    // do nothing
}