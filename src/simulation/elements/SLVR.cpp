#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_SLVR()
{
	Identifier = "DEFAULT_PT_SLVR";
	Name = "SLVR";
	Colour = 0x818181_rgb;
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
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 0;
	Weight = 100;

	HeatConduct = 251;
	Description = "Silver,catalyzes reactions,sparkles,and conducts through long gaps.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 960.5f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;
	Create = &create;
}

static void sand_reactions(int sand1_id, UPDATE_FUNC_ARGS)
{
	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				int r = pmap[y + ry][x + rx];
				if (!r || ID(r) == sand1_id)
					continue;
				int rt = TYP(r);

				// SAND + GUN -> TNT
				if (rt == PT_GUNP)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_BANG);
					sim->kill_part(sand1_id);
					return;
				}
				// SAND + PSTE-> CNCT
				else if (rt == PT_PSTE)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_CNCT);
					sim->kill_part(sand1_id);
					return;
				}
				// SAND + DUST-> EQVE
				else if (rt == PT_DUST)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_E116);
					sim->kill_part(sand1_id);
					return;
				}
			}
		}
	}
}

static void dust_reactions(int dust1_id, UPDATE_FUNC_ARGS)
{
	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				int r = pmap[y + ry][x + rx];
				if (!r || ID(r) == dust1_id)
					continue;
				int rt = TYP(r);

				// EXOT + DUST -> GLOW + ANAR
				if (rt == PT_EXOT)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_GLOW);
					sim->part_change_type(dust1_id, (int)(parts[dust1_id].x + 0.5f), (int)(parts[dust1_id].y + 0.5f), PT_ANAR);
					return;
				}
			}
		}
	}
}

static int update(UPDATE_FUNC_ARGS)
{
	int sand1_id = -1; // Id of a sand particle for hydrogen multi-particle reactions
	int dust1_id = -1; // same but dust

	// Fast conduction (like PTNM)
	if (!parts[i].life)
	{
		for (int j = 0; j < 13; j++)
		{
			static const int checkCoordsX[] = {-6,-4,-2,0,2,4,6,0,0,0,0,0,0};
			static const int checkCoordsY[] = {0,0,0,0,0,0,0,6,4,2,-2,-4,-6};
			int rx = checkCoordsX[j];
			int ry = checkCoordsY[j];
			int r = pmap[y + ry][x + rx];
			if (r && TYP(r) == PT_SPRK && parts[ID(r)].life && parts[ID(r)].life < 4)
			{
				sim->part_change_type(i, x, y, PT_SPRK);
				parts[i].life = 4;
				parts[i].ctype = PT_SLVR;
			}
		}
	}

	// Single element reactions
	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				int r = pmap[y + ry][x + rx];
				if (!r)
					continue;
				int rt = TYP(r);

				if (rt == PT_SAND && sand1_id < 0)
					sand1_id = ID(r);

				if (rt == PT_DUST && dust1_id < 0)
					dust1_id = ID(r);

				// These reactions will occur instantly in contact with PTNM
				// --------------------------------------------------------

				// Shield instantly grows (even without SPRK)
				if (!parts[ID(r)].life && (rt == PT_SHLD1 || rt == PT_SHLD2 || rt == PT_SHLD3))
				{
					int next = PT_SHLD1;
					switch (rt)
					{
					case PT_SHLD1: next = PT_SHLD2; break;
					case PT_SHLD2: next = PT_SHLD3; break;
					case PT_SHLD3: next = PT_SHLD4; break;
					}
					sim->part_change_type(ID(r), x + rx, y + ry, next);
					parts[ID(r)].life = 7;
					continue;
				}
				// These reactions are dependent on temperature
				// Probability goes quadratically from 0% / frame to 100% / frame from 0 C to 1500 C
				// --------------------------------------------------------
				float prob = std::min(1.0f, parts[i].temp / (273.15f + 1500.0f));
				prob *= prob;

				if (sim->rng.uniform01() <= prob)
				{
					switch (rt)
					{
					case PT_SMKE: // SMKE + <= 122 C -> DUST
						if (parts[ID(r)].temp <= 122.0f + 273.15f)
						{
							sim->part_change_type(ID(r), x + rx, y + ry, PT_INSL);
						}
						break;
					}
				}
			}
		}
	}

	// Sand reactions
	if (sand1_id >= 0)
	{
		sand_reactions(sand1_id, UPDATE_FUNC_SUBCALL_ARGS);
	}

	// Dust reactions
	if (dust1_id >= 0)
	{
		dust_reactions(dust1_id, UPDATE_FUNC_SUBCALL_ARGS);
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp)
		*pixel_mode |= PMODE_FLARE;
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	if (sim->rng.chance(1, 15))
		sim->parts[i].tmp = 1;
}
