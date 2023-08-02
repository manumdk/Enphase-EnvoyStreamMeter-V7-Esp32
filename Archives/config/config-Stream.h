#ifndef CONFIGSTREAM
#define CONFIGSTREAM

#include <Arduino.h>

#define debug true


// It's quick and easy but all these variables shouldn't be public. It's a pain to provider getters for each one but that would
// prevent accidental modification of these variables. Or, the developer can be careful not to make a mistake. Those never happen!

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