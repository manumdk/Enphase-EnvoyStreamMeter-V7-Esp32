#ifndef DECOD_FUNCTIONS
#define DECOD_FUNCTIONS

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config-Stream.h"

DataEnvoy dataEnvoy;
enum Phase ph;
// JSon routines

// Routine de gestion et impression des donnes JSON

int processingJsondata(String payload)
{

    DynamicJsonDocument doc(2000); // Added from envoy exemple

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload);

    // Test if parsing does not succeeds.
    if (error != DeserializationError::Ok)
    {
        DEBUG_SERIAL.print(F("*************** deserializeJson() failed: "));
        DEBUG_SERIAL.println(error.f_str());
        DEBUG_SERIAL.printf("message recu, lg %i, message : %s \n",payload.length(), payload);
        return 0;
    }

    JsonObject obj = doc.as<JsonObject>();
    int numPh = 0;
    if (obj.containsKey("production"))
    {
        if (obj["production"].containsKey("ph-a"))
        {
            dataEnvoy.Prod.p[pha] = obj["production"]["ph-a"]["p"];
            dataEnvoy.Prod.q[pha] = obj["production"]["ph-a"]["q"];
            dataEnvoy.Prod.s[pha] = obj["production"]["ph-a"]["s"];
            dataEnvoy.Prod.v[pha] = obj["production"]["ph-a"]["v"];
            dataEnvoy.Prod.i[pha] = obj["production"]["ph-a"]["i"];
            dataEnvoy.Prod.pf[pha] = obj["production"]["ph-a"]["pf"];
            dataEnvoy.Prod.f[pha] = obj["production"]["ph-a"]["f"];
            if (bLog)
            {
                DEBUG_SERIAL.print("\nPhase A - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.p[pha]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.q[pha]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.s[pha]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.v[pha]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.i[pha]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.pf[pha]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Prod.f[pha]);
            }
        }
        if (obj["production"].containsKey("ph-b"))
        {
            dataEnvoy.Prod.p[phb] = obj["production"]["ph-b"]["p"];
            dataEnvoy.Prod.q[phb] = obj["production"]["ph-b"]["q"];
            dataEnvoy.Prod.s[phb] = obj["production"]["ph-b"]["s"];
            dataEnvoy.Prod.v[phb] = obj["production"]["ph-b"]["v"];
            dataEnvoy.Prod.i[phb] = obj["production"]["ph-b"]["i"];
            dataEnvoy.Prod.pf[phb] = obj["production"]["ph-b"]["pf"];
            dataEnvoy.Prod.f[phb] = obj["production"]["ph-b"]["f"];

            if (dataEnvoy.Prod.v[phb] > 0 && bLog)
            {

                DEBUG_SERIAL.print("\nPhase B - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.p[phb]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.q[phb]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.s[phb]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.v[phb]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.i[phb]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.pf[phb]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Prod.f[phb]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase B non connectée");
        }
        if (obj["production"].containsKey("ph-c"))
        {
            dataEnvoy.Prod.p[phc] = obj["production"]["ph-c"]["p"];
            dataEnvoy.Prod.q[phc] = obj["production"]["ph-c"]["q"];
            dataEnvoy.Prod.s[phc] = obj["production"]["ph-c"]["s"];
            dataEnvoy.Prod.v[phc] = obj["production"]["ph-c"]["v"];
            dataEnvoy.Prod.i[phc] = obj["production"]["ph-c"]["i"];
            dataEnvoy.Prod.pf[phc] = obj["production"]["ph-c"]["pf"];
            dataEnvoy.Prod.f[phc] = obj["production"]["ph-c"]["f"];

            if (dataEnvoy.Prod.v[phc] > 0 && bLog)
            {

                DEBUG_SERIAL.print("\nPhase C - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.p[phc]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.q[phc]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.s[phc]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.v[phc]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.i[phc]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Prod.pf[phc]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Prod.f[phc]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase C non connectée");
        }

        if (bLog)
        {
            DEBUG_SERIAL.print("\n  Power Produced:");
            DEBUG_SERIAL.print(dataEnvoy.Prod.p[pha] + dataEnvoy.Prod.p[pha] + dataEnvoy.Prod.p[phc]);
            DEBUG_SERIAL.print("\tCurrent: ");
            DEBUG_SERIAL.print(dataEnvoy.Prod.i[pha] + dataEnvoy.Prod.i[pha] + dataEnvoy.Prod.i[phc]);
            DEBUG_SERIAL.print("\tPowerFactor:");
            DEBUG_SERIAL.print(dataEnvoy.Prod.pf[pha]);
            DEBUG_SERIAL.print("\tFrequency:");
            DEBUG_SERIAL.print(dataEnvoy.Prod.f[pha]);
            DEBUG_SERIAL.println();
        }
    }

    else if (bLog)

    {
        DEBUG_SERIAL.println("No production data");
        return 0;
    }

    if (obj.containsKey("net-consumption"))
    {
        if (obj["net-consumption"].containsKey("ph-a"))
        {
            dataEnvoy.Net.p[pha] = obj["net-consumption"]["ph-a"]["p"];
            dataEnvoy.Net.q[pha] = obj["net-consumption"]["ph-a"]["q"];
            dataEnvoy.Net.s[pha] = obj["net-consumption"]["ph-a"]["s"];
            dataEnvoy.Net.v[pha] = obj["net-consumption"]["ph-a"]["v"];
            dataEnvoy.Net.i[pha] = obj["net-consumption"]["ph-a"]["i"];
            dataEnvoy.Net.pf[pha] = obj["net-consumption"]["ph-a"]["pf"];
            dataEnvoy.Net.f[pha] = obj["net-consumption"]["ph-a"]["f"];
            if (bLog)
            {
                DEBUG_SERIAL.print("\nPhase B - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[pha]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[pha]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[pha]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[pha]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[pha]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[pha]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[pha]);
            }
        }
        if (obj["net-consumption"].containsKey("ph-b"))
        {
            dataEnvoy.Net.p[phb] = obj["net-consumption"]["ph-b"]["p"];
            dataEnvoy.Net.q[phb] = obj["net-consumption"]["ph-b"]["q"];
            dataEnvoy.Net.s[phb] = obj["net-consumption"]["ph-b"]["s"];
            dataEnvoy.Net.v[phb] = obj["net-consumption"]["ph-b"]["v"];
            dataEnvoy.Net.i[phb] = obj["net-consumption"]["ph-b"]["i"];
            dataEnvoy.Net.pf[phb] = obj["net-consumption"]["ph-b"]["pf"];
            dataEnvoy.Net.f[phb] = obj["net-consumption"]["ph-b"]["f"];

            if (dataEnvoy.Net.v[phb] > 0 && bLog)
            {
                DEBUG_SERIAL.print("\nPhase A - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[phb]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[phb]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[phb]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[phb]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[phb]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[phb]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[phb]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase B non connectée");
        }
        if (obj["net-consumption"].containsKey("ph-c"))
        {
            dataEnvoy.Net.p[phc] = obj["net-consumption"]["ph-c"]["p"];
            dataEnvoy.Net.q[phc] = obj["net-consumption"]["ph-c"]["q"];
            dataEnvoy.Net.s[phc] = obj["net-consumption"]["ph-c"]["s"];
            dataEnvoy.Net.v[phc] = obj["net-consumption"]["ph-c"]["v"];
            dataEnvoy.Net.i[phc] = obj["net-consumption"]["ph-c"]["i"];
            dataEnvoy.Net.pf[phc] = obj["net-consumption"]["ph-c"]["pf"];
            dataEnvoy.Net.f[phc] = obj["net-consumption"]["ph-c"]["f"];

            if (dataEnvoy.Net.v[phc] > 0 && bLog)
            {

                DEBUG_SERIAL.print("\nPhase C - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[phc]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[phc]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[phc]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[phc]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[phc]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[phc]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[phc]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase C non connectée");
        }
        if (bLog)
        {
            DEBUG_SERIAL.print("\n  Power Produced:");
            DEBUG_SERIAL.print(dataEnvoy.Net.p[pha] + dataEnvoy.Net.p[pha] + dataEnvoy.Net.p[phc]);
            DEBUG_SERIAL.print("\tCurrent: ");
            DEBUG_SERIAL.print(dataEnvoy.Net.i[pha] + dataEnvoy.Net.i[pha] + dataEnvoy.Net.i[phc]);
            DEBUG_SERIAL.print("\tPowerFactor:");
            DEBUG_SERIAL.print(dataEnvoy.Net.pf[pha]);
            DEBUG_SERIAL.print("\tFrequency:");
            DEBUG_SERIAL.print(dataEnvoy.Net.f[pha]);
            DEBUG_SERIAL.println();
        }
    }

    else if (bLog)

    {
        DEBUG_SERIAL.println("No net-consumption data");
        return 0;
    }
    if (obj.containsKey("total-consumption"))
    {
        if (obj["total-consumption"].containsKey("ph-a"))
        {
            dataEnvoy.Net.p[pha] = obj["total-consumption"]["ph-a"]["p"];
            dataEnvoy.Net.q[pha] = obj["total-consumption"]["ph-a"]["q"];
            dataEnvoy.Net.s[pha] = obj["total-consumption"]["ph-a"]["s"];
            dataEnvoy.Net.v[pha] = obj["total-consumption"]["ph-a"]["v"];
            dataEnvoy.Net.i[pha] = obj["total-consumption"]["ph-a"]["i"];
            dataEnvoy.Net.pf[pha] = obj["total-consumption"]["ph-a"]["pf"];
            dataEnvoy.Net.f[pha] = obj["total-consumption"]["ph-a"]["f"];
            if (bLog)
            {
                DEBUG_SERIAL.print("\nPhase B - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[pha]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[pha]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[pha]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[pha]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[pha]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[pha]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[pha]);
            }
        }
        if (obj["total-consumption"].containsKey("ph-b"))
        {
            dataEnvoy.Net.p[phb] = obj["total-consumption"]["ph-b"]["p"];
            dataEnvoy.Net.q[phb] = obj["total-consumption"]["ph-b"]["q"];
            dataEnvoy.Net.s[phb] = obj["total-consumption"]["ph-b"]["s"];
            dataEnvoy.Net.v[phb] = obj["total-consumption"]["ph-b"]["v"];
            dataEnvoy.Net.i[phb] = obj["total-consumption"]["ph-b"]["i"];
            dataEnvoy.Net.pf[phb] = obj["total-consumption"]["ph-b"]["pf"];
            dataEnvoy.Net.f[phb] = obj["total-consumption"]["ph-b"]["f"];

            if (dataEnvoy.Net.v[phb] > 0 && bLog)
            {

                DEBUG_SERIAL.print("\nPhase A - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[phb]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[phb]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[phb]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[phb]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[phb]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[phb]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[phb]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase B non connectée");
        }
        if (obj["total-consumption"].containsKey("ph-c"))
        {
            dataEnvoy.Net.p[phc] = obj["total-consumption"]["ph-c"]["p"];
            dataEnvoy.Net.q[phc] = obj["total-consumption"]["ph-c"]["q"];
            dataEnvoy.Net.s[phc] = obj["total-consumption"]["ph-c"]["s"];
            dataEnvoy.Net.v[phc] = obj["total-consumption"]["ph-c"]["v"];
            dataEnvoy.Net.i[phc] = obj["total-consumption"]["ph-c"]["i"];
            dataEnvoy.Net.pf[phc] = obj["total-consumption"]["ph-c"]["pf"];
            dataEnvoy.Net.f[phc] = obj["total-consumption"]["ph-c"]["f"];

            if (dataEnvoy.Net.v[phc] > 0 && bLog)
            {

                DEBUG_SERIAL.print("\nPhase C - ");
                DEBUG_SERIAL.print("Real Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.p[phc]);
                DEBUG_SERIAL.print("\tReact Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.q[phc]);
                DEBUG_SERIAL.print("\tApparent Power: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.s[phc]);
                DEBUG_SERIAL.print("\tVoltage: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.v[phc]);
                DEBUG_SERIAL.print("\tCurrent: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.i[phc]);
                DEBUG_SERIAL.print("\tPower Factor: ");
                DEBUG_SERIAL.print(dataEnvoy.Net.pf[phc]);
                DEBUG_SERIAL.print("\tFrequency: ");
                DEBUG_SERIAL.println(dataEnvoy.Net.f[phc]);
            }
        }
        else if (bLog)
        {
            DEBUG_SERIAL.println("Phase C non connectée");
        }

        if (bLog)
        {
            DEBUG_SERIAL.print("\n  Power Produced:");
            DEBUG_SERIAL.print(dataEnvoy.Net.p[pha] + dataEnvoy.Net.p[phb] + dataEnvoy.Net.p[phc]);
            DEBUG_SERIAL.print("\tCurrent: ");
            DEBUG_SERIAL.print(dataEnvoy.Net.i[pha] + dataEnvoy.Net.i[phb] + dataEnvoy.Net.i[phc]);
            DEBUG_SERIAL.print("\tPowerFactor:");
            DEBUG_SERIAL.print(dataEnvoy.Net.pf[pha]);
            DEBUG_SERIAL.print("\tFrequency:");
            DEBUG_SERIAL.print(dataEnvoy.Net.f[pha]);
            DEBUG_SERIAL.println();
        }
    }

    else
    {
        if (bLog)
        {
            DEBUG_SERIAL.println("No total-consumption data");
            return 0;
        }
    }
    return 1;
}

#endif
