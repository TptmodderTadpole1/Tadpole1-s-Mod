#include "simulation/ElementCommon.h"

void Element::Element_HFNM()
{
    Identifier = "DEFAULT_PT_HFNM";
    Name = "HFNM";
    Colour = 0x23DAAD_rgb;
    MenuVisible = 1;
    MenuSection = SC_SOLIDS;
    Enabled = 1;

    Advection = 0.0f;
    AirDrag = 0.00f * CFDS;
    AirLoss = 0.90f;
    Loss = 0.00f;
    Collision = 0.0f;
    Gravity = 0.0f;
    Diffusion = 0.00f;
    HotAir = 0.000f * CFDS;
    Falldown = 0;

    Flammable = 0;
    Explosive = 0;
    Meltable = 0;
    Hardness = 0;

    Weight = 100;

    HeatConduct = 193;
    Description = "Hafnium,absorbs neutrons and converts it to electricity.Extremely high melting point.";

    Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_LIFE_DEC;

    LowPressure = IPL;
    LowPressureTransition = NT;
    HighPressure = IPH;
    HighPressureTransition = NT;
    LowTemperature = ITL;
    LowTemperatureTransition = NT;
    HighTemperature = 4215.0f + 273.15f ;
    HighTemperatureTransition = PT_LAVA;
}
