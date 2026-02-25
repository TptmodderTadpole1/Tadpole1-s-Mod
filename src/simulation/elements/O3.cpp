#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_O3()
{
	Identifier = "DEFAULT_PT_O3";
	Name = "OZON";
	Colour = 0x70B0EF_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 2.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.0f;
	Diffusion = 3.0f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 1;

	HeatConduct = 70;
	Description = "Ozone,absorbs energy particles,gets eaten by RFRG.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &can_move = sd.can_move;
	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry){
				auto r = pmap[y+ry][x+rx];
				if (!r){
					continue;
				}
				if (sim->photons[y+ry][x+rx]){
					sim->kill_part(ID(r));
				}
			}
		}
	}
	
	return 0;
}
