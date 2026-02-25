#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_MDST()
{
	Identifier = "DEFAULT_PT_MDST";
	Name = "MDST";
	Colour = 0xF0F000_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = -0.05f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.95f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 40.0f;
	Diffusion = 0.00f;
	HotAir = 1.0f* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 101;

	HeatConduct = 150;
	Description = "Most destructive Bomb, Use sparingly.";

	Properties = TYPE_PART|PROP_LIFE_DEC|PROP_LIFE_KILL_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;
	int rx = sim->rng.between(-5, 5);
	int ry = sim->rng.between(-5, 5);
	int r = pmap[y+ry][x+rx];
	if (!r)
		return 0;
	int rt = TYP(r);
	if (rt == PT_DEST || rt == PT_DMND  || rt == PT_CLNE  || rt == PT_PCLN  || rt == PT_MDST)
		return 0;

	if (parts[i].life<=0 || parts[i].life>137)
	{
		parts[i].life = sim->rng.between(30, 49);
		sim->pv[y/CELL][x/CELL]+=600.0f;
	}
	if (rt == PT_PLUT || rt == PT_DEUT)
	{
		sim->pv[y/CELL][x/CELL]+=200.0f;
		sim->create_part(ID(r), x+rx, y+ry, PT_NEUT);
		parts[ID(r)].temp = MAX_TEMP;
		sim->pv[y/CELL][x/CELL] += 10.0f;
		parts[i].life-=4;
	}
	else if (rt == PT_INSL)
	{
		sim->create_part(ID(r), x+rx, y+ry, PT_PLSM);
	}
	else if (sim->rng.chance(1, 30))
	{
		sim->kill_part(ID(r));
		parts[i].life -= 4*((elements[rt].Properties&TYPE_SOLID)?3:1);
		if (parts[i].life<=0)
			parts[i].life=1;
	}
	else if (!sd.IsHeatInsulator(parts[ID(r)]))
		parts[ID(r)].temp = MAX_TEMP;
	parts[i].temp=MAX_TEMP;
	sim->pv[y/CELL][x/CELL] = restrict_flt(sim->pv[y/CELL][x/CELL] + 80.0f, MIN_PRESSURE, MAX_PRESSURE);

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if(cpart->life)
	{
		*pixel_mode |= PMODE_LFLARE;
	}
	else
	{
		*pixel_mode |= PMODE_SPARK;
	}
	return 0;
}
