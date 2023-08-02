#ifndef DECOD_FUNCTIONS
#define DECOD_FUNCTIONS

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../config/config.h"
#include "../config/enums.h"
#include "../config/config-Stream.h"


// JSon routines

// Routine de gestion et impression des donnes JSON

int processingJsondata(String payload) {

DynamicJsonDocument doc(2000);          // Added from envoy exemple
     
// Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);
      
  // Test if parsing does not succeeds.
  if (error!=DeserializationError::Ok) {
    DEBUG_SERIAL.print(F("deserializeJson() failed: "));
    DEBUG_SERIAL.println(error.f_str());
    return 0;
  }

   JsonObject obj = doc.as<JsonObject>();

      if (obj.containsKey("production"))
      {
          if (obj["production"].containsKey("ph-a"))
          {
              AReal_power = obj["production"]["ph-a"]["p"]; 
              AReactive_power = obj["production"]["ph-a"]["q"];  
              AApparent_power = obj["production"]["ph-a"]["s"];
              Avoltage = obj["production"]["ph-a"]["v"];
              Acurrent = obj["production"]["ph-a"]["i"];
              pf = obj["production"]["ph-a"]["pf"];
              freq = obj["production"]["ph-a"]["f"];
              
                  DEBUG_SERIAL.print("\nPhase A - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(AReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(AReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(AApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(Avoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(Acurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
          }
          if (obj["production"].containsKey("ph-b"))
          {
              BReal_power = obj["production"]["ph-b"]["p"]; 
              BReactive_power = obj["production"]["ph-b"]["q"];  
              BApparent_power = obj["production"]["ph-b"]["s"];
              Bvoltage = obj["production"]["ph-b"]["v"];
              Bcurrent = obj["production"]["ph-b"]["i"];
              pf = obj["production"]["ph-b"]["pf"];
              freq = obj["production"]["ph-b"]["f"];
              
              if (Bvoltage>0) {
                
                  DEBUG_SERIAL.print("Phase B - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(BReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(BReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(BApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(Bvoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(Bcurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
              }
              else DEBUG_SERIAL.println("Phase B non connectée");
              
          }
          if (obj["production"].containsKey("ph-c"))
          {
              CReal_power = obj["production"]["ph-c"]["p"]; 
              CReactive_power = obj["production"]["ph-c"]["q"];  
              CApparent_power = obj["production"]["ph-c"]["s"];
              Cvoltage = obj["production"]["ph-c"]["v"];
              Ccurrent = obj["production"]["ph-c"]["i"];
              pf = obj["production"]["ph-c"]["pf"];
              freq = obj["production"]["ph-c"]["f"];
              
              if (Cvoltage >0)  {
                  DEBUG_SERIAL.print("Phase C - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(CReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(CReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(CApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(Cvoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(Ccurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
              }
              else DEBUG_SERIAL.println("Phase C non connectée");
              
          }
          
          Power = AReal_power+BReal_power+CReal_power;
                        
              DEBUG_SERIAL.print("\n  Power Produced:");
              DEBUG_SERIAL.print(Power);
              DEBUG_SERIAL.print("\tCurrent: ");
              DEBUG_SERIAL.print((Acurrent+Bcurrent+Ccurrent));
              DEBUG_SERIAL.print("\tPowerFactor:");
              DEBUG_SERIAL.print(pf);
              DEBUG_SERIAL.print("\tFrequency:");
              DEBUG_SERIAL.print(freq);
              DEBUG_SERIAL.println();
          
      } else {
             DEBUG_SERIAL.println("No production data");
             return 0;
        }

      if (obj.containsKey("total-consumption"))
      {
          if (obj["total-consumption"].containsKey("ph-a"))
          {
              tcAReal_power = obj["total-consumption"]["ph-a"]["p"]; 
              tcAReactive_power = obj["total-consumption"]["ph-a"]["q"];  
              tcAApparent_power = obj["total-consumption"]["ph-a"]["s"];
              tcAvoltage = obj["total-consumption"]["ph-a"]["v"];
              tcAcurrent = obj["total-consumption"]["ph-a"]["i"];
              pf = obj["total-consumption"]["ph-a"]["pf"];
              freq = obj["total-consumption"]["ph-a"]["f"];
              
                  DEBUG_SERIAL.print("\nPhase A - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(tcAReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(tcAReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(tcAApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(tcAvoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(tcAcurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
              
          }
          if (obj["total-consumption"].containsKey("ph-b"))
          {
              tcBReal_power = obj["total-consumption"]["ph-b"]["p"]; 
              tcBReactive_power = obj["total-consumption"]["ph-b"]["q"];  
              tcBApparent_power = obj["total-consumption"]["ph-b"]["s"];
              tcBvoltage = obj["total-consumption"]["ph-b"]["v"];
              tcBcurrent = obj["total-consumption"]["ph-b"]["i"];
              pf = obj["total-consumption"]["ph-b"]["pf"];
              freq = obj["total-consumption"]["ph-b"]["f"];
              
              if (tcBvoltage > 0) {
                  DEBUG_SERIAL.print("Phase B - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(tcBReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(tcBReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(tcBApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(tcBvoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(tcBcurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
              }
              else DEBUG_SERIAL.println("Phase B non connectée");
              
          }
          if (obj["total-consumption"].containsKey("ph-c"))
          {
              tcCReal_power = obj["total-consumption"]["ph-c"]["p"]; 
              tcCReactive_power = obj["total-consumption"]["ph-c"]["q"];  
              tcCApparent_power = obj["total-consumption"]["ph-c"]["s"];
              tcCvoltage = obj["total-consumption"]["ph-c"]["v"];
              tcCcurrent = obj["total-consumption"]["ph-c"]["i"];
              pf = obj["total-consumption"]["ph-c"]["pf"];
              freq = obj["total-consumption"]["ph-c"]["f"];
              
              if (tcCvoltage >0) {
                  DEBUG_SERIAL.print("Phase C - ");
                  DEBUG_SERIAL.print("Real Power: ");
                  DEBUG_SERIAL.print(tcCReal_power);
                  DEBUG_SERIAL.print("\tReact Power: ");
                  DEBUG_SERIAL.print(tcCReactive_power);
                  DEBUG_SERIAL.print("\tApparent Power: ");
                  DEBUG_SERIAL.print(tcCApparent_power);
                  DEBUG_SERIAL.print("\tVoltage: ");
                  DEBUG_SERIAL.print(tcCvoltage);
                  DEBUG_SERIAL.print("\tCurrent: ");
                  DEBUG_SERIAL.print(tcCcurrent);  
                  DEBUG_SERIAL.print("\tPower Factor: ");
                  DEBUG_SERIAL.print(pf);  
                  DEBUG_SERIAL.print("\tFrequency: ");
                  DEBUG_SERIAL.println(freq);
              }
              else DEBUG_SERIAL.println("Phase C non connectée");
              
          }
          
            tcAReal_power *= -1;
            tcBReal_power *= -1;
            tcCReal_power *= -1;
            Consumption= tcAReal_power + tcBReal_power + tcCReal_power;
            NetPower=Power+Consumption;
                    
              DEBUG_SERIAL.print("  Power Consumed: \t");
              DEBUG_SERIAL.println(Consumption);
              DEBUG_SERIAL.print("\t\t Net Power: \t");
              DEBUG_SERIAL.println(Power + Consumption);
              DEBUG_SERIAL.println();                      
          
      }
      else {
        DEBUG_SERIAL.println("No Consumption Data");
        return 0;
        }
  return 1;
 }

#endif