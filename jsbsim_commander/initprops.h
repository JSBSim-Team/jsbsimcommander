#ifndef INITPROPS_H
#define INITPROPS_H

inline void InitProps(wxArrayString & source, wxArrayString & source_desc, wxArrayString & source_unit, wxArrayString & source_access, wxArrayString & source_comment, wxArrayString & destination, wxArrayString & destination_desc, wxArrayString & destination_unit, wxArrayString & destination_access, wxArrayString & destination_comment, wxArrayString & all, wxArrayString & all_desc, wxArrayString & all_unit, wxArrayString & all_access, wxArrayString & all_comment)
{
  source.Add(wxT("sim-time-sec"));
  source_desc.Add(_("time"));
  source_unit.Add(wxT("SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("output_delay"));
  source_desc.Add(_("go on output after optimize"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("default value is zero"));

  source.Add(wxT("output_date_rate"));
  source_desc.Add(_("dat file output rate"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("output a record every X times. default value is 5."));

  source.Add(wxT("signal/aeroelastic"));
  source_desc.Add(_("aeroelasticity"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("aeroelastic enable signal(0-false, 1-true)"));

  source.Add(wxT("signal/custom1"));
  source_desc.Add(_("custom signal 1"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("custom enable signal(0-false, 1-true)"));

  source.Add(wxT("signal/custom2"));
  source_desc.Add(_("custom signal 2"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("custom enable signal(0-false, 1-true)"));

  source.Add(wxT("signal/custom3"));
  source_desc.Add(_("custom signal 3"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("custom enable signal(0-false, 1-true)"));

  source.Add(wxT("signal/custom4"));
  source_desc.Add(_("custom signal 4"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("custom enable signal(0-false, 1-true)"));

  source.Add(wxT("signal/custom5"));
  source_desc.Add(_("custom signal 5"));
  source_unit.Add(wxT("NORM"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("custom enable signal(0-false, 1-true)"));

  source.Add(wxT("metrics/Sw-sqft"));
  source_desc.Add(_("wing area"));
  source_unit.Add(wxT("SQFT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/Sw-sqm"));
  source_desc.Add(_("wing area"));
  source_unit.Add(wxT("SQM"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/bw-ft"));
  source_desc.Add(_("wing span"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/bw-m"));
  source_desc.Add(_("wing span"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/cbarw-ft"));
  source_desc.Add(_("chord bar"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/cbarw-m"));
  source_desc.Add(_("chord bar"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/iw-deg"));
  source_desc.Add(_("wing incidence angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero/alpha-wing-rad = aero/alpha-rad + metrics/iw-deg * deg2rad"));

  source.Add(wxT("metrics/Sh-sqft"));
  source_desc.Add(_("Horizontal Tail Area"));
  source_unit.Add(wxT("SQFT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/Sh-sqm"));
  source_desc.Add(_("horizontal tail area"));
  source_unit.Add(wxT("SQM"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lh-ft"));
  source_desc.Add(_("horizontal tail arm"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lh-m"));
  source_desc.Add(_("horizontal tail arm"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lh-norm"));
  source_desc.Add(_("horizontal tail arm"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated, HTailArm / cbar"));

  source.Add(wxT("metrics/vbarh-norm"));
  source_desc.Add(_("H. Tail Volume"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("vbarh = HTailArm*HTailArea / (cbar*WingArea)"));

  source.Add(wxT("metrics/Sv-sqft"));
  source_desc.Add(_("vertical tail area"));
  source_unit.Add(wxT("SQFT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/Sv-sqm"));
  source_desc.Add(_("vertical tail area"));
  source_unit.Add(wxT("SQM"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lv-ft"));
  source_desc.Add(_("vertical tail arm"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lv-m"));
  source_desc.Add(_("vertical tail arm"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated"));

  source.Add(wxT("metrics/lv-norm"));
  source_desc.Add(_("vertical tail arm"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("deprecated, VTailArm / cbar"));

  source.Add(wxT("metrics/vbarv-norm"));
  source_desc.Add(_("V. Tail Volume"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("vbarv = VTailArm*VTailArea / (cbar*WingArea)"));

  source.Add(wxT("metrics/aero-rp-x-in"));
  source_desc.Add(_("X coordinate for ARP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/aero-rp-x-m"));
  source_desc.Add(_("X coordinate for ARP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/aero-rp-y-in"));
  source_desc.Add(_("Y coordinate for ARP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/aero-rp-y-m"));
  source_desc.Add(_("Y coordinate for ARP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/aero-rp-z-in"));
  source_desc.Add(_("Z coordinate for ARP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/aero-rp-z-m"));
  source_desc.Add(_("Z coordinate for ARP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Aero Referece Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-x-in"));
  source_desc.Add(_("X coordinate for EP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-x-m"));
  source_desc.Add(_("X coordinate for EP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-y-in"));
  source_desc.Add(_("Y coordinate for EP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-y-m"));
  source_desc.Add(_("Y coordinate for EP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-z-in"));
  source_desc.Add(_("Z coordinate for EP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/eyepoint-z-m"));
  source_desc.Add(_("Z coordinate for EP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Eye Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-x-in"));
  source_desc.Add(_("X coordinate for VRP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-x-m"));
  source_desc.Add(_("X coordinate for VRP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-y-in"));
  source_desc.Add(_("Y coordinate for VRP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-y-m"));
  source_desc.Add(_("Y coordinate for VRP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-z-in"));
  source_desc.Add(_("Z coordinate for VRP"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("metrics/visualrefpoint-z-m"));
  source_desc.Add(_("Z coordinate for VRP"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("Visual Reference Point in the structural frame"));

  source.Add(wxT("inertia/mass-slugs"));
  source_desc.Add(_("mass"));
  source_unit.Add(wxT("SLUGS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("inertia/mass-kg"));
  source_desc.Add(_("mass"));
  source_unit.Add(wxT("KG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("inertia/weight-lbs"));
  source_desc.Add(_("weight"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("inertia/weight-n"));
  source_desc.Add(_("weight"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("inertia/cg-x-in"));
  source_desc.Add(_("CG pos along the X axis"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("inertia/cg-x-m"));
  source_desc.Add(_("CG pos along the X axis"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("inertia/cg-y-in"));
  source_desc.Add(_("CG pos along the Y axis"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("inertia/cg-y-m"));
  source_desc.Add(_("CG pos along the Y axis"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("inertia/cg-z-in"));
  source_desc.Add(_("CG pos along the Z axis"));
  source_unit.Add(wxT("IN"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("inertia/cg-z-m"));
  source_desc.Add(_("CG pos along the Z axis"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("CG pos in the structural frame"));

  source.Add(wxT("atmosphere/T-R"));
  source_desc.Add(_("aero temperature"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the temperature in degrees Rankine"));

  source.Add(wxT("atmosphere/T-C"));
  source_desc.Add(_("aero temperature"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the temperature in degrees Celsius"));

  source.Add(wxT("atmosphere/T-K"));
  source_desc.Add(_("aero temperature"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the temperature in degrees Kelvin"));

  source.Add(wxT("atmosphere/T-sl-R"));
  source_desc.Add(_("temperature at sea level"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("sea level temperature in degrees Rankine"));

  source.Add(wxT("atmosphere/T-sl-C"));
  source_desc.Add(_("temperature at sea level"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("sea level temperature in degrees Celsius"));

  source.Add(wxT("atmosphere/T-sl-K"));
  source_desc.Add(_("aero temperature at sea level"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("sea level temperature in degrees Kelvin"));

  source.Add(wxT("atmosphere/theta-norm"));
  source_desc.Add(_("aero temperature"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the ratio of at-altitude temperature over the sea level value"));

  source.Add(wxT("atmosphere/rho-slugs_ft3"));
  source_desc.Add(_("aero density"));
  source_unit.Add(wxT("SLUGS/FT^3"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/rho-kg_m3"));
  source_desc.Add(_("aero density"));
  source_unit.Add(wxT("KG/M^3"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/rho-sl-slugs_ft3"));
  source_desc.Add(_("aero density at sea level"));
  source_unit.Add(wxT("SLUGS/FT^3"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/rho-sl-kg_m3"));
  source_desc.Add(_("aero density at sea level"));
  source_unit.Add(wxT("KG/M^3"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/sigma-norm"));
  source_desc.Add(_("aero density"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the ratio of at-altitude density over the sea level value"));

  source.Add(wxT("atmosphere/P-psf"));
  source_desc.Add(_("aero pressure"));
  source_unit.Add(wxT("PSF"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/P-pa"));
  source_desc.Add(_("aero pressure"));
  source_unit.Add(wxT("Pa"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/P-sl-psf"));
  source_desc.Add(_("aero pressure at sea level"));
  source_unit.Add(wxT("PSF"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/P-sl-pa"));
  source_desc.Add(_("aero pressure at sea level"));
  source_unit.Add(wxT("Pa"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/delta-norm"));
  source_desc.Add(_("aero pressure"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the ratio of at-altitude pressure over the sea level value"));

  source.Add(wxT("atmosphere/a-fps"));
  source_desc.Add(_("sound speed"));
  source_unit.Add(wxT("FT/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/a-mps"));
  source_desc.Add(_("sound speed"));
  source_unit.Add(wxT("M/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/a-sl-fps"));
  source_desc.Add(_("speed of sound  at sea level"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/a-sl-mps"));
  source_desc.Add(_("sound speed at sea level"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/a-norm"));
  source_desc.Add(_("sound speed"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the ratio of at-altitude sound speed over the sea level value"));

  source.Add(wxT("atmosphere/psiw-rad"));
  source_desc.Add(_("wind direction"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("0 <= psiw < 2*pi"));

  /**************************************
  source.Add(wxT("atmosphere/delta-T"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/T-sl-dev-F"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  ****************************************/

  source.Add(wxT("atmosphere/density-altitude"));
  source_desc.Add(_("density altitude in feet"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/density-altitude-m"));
  source_desc.Add(_("density altitude in meters"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("atmosphere/p-turb-rad_sec"));
  source_desc.Add(_("aero turbulence moment along the X axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence moment in the body frame"));

  source.Add(wxT("atmosphere/q-turb-rad_sec"));
  source_desc.Add(_("aero turbulence moment along the Y axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence moment in the body frame"));

  source.Add(wxT("atmosphere/r-turb-rad_sec"));
  source_desc.Add(_("aero turbulence moment along the Z axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence moment in the body frame"));

  source.Add(wxT("atmosphere/n-turb-fps"));
  source_desc.Add(_("aero turbulence force for the north axis"));
  source_unit.Add(wxT("FT/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("atmosphere/n-turb-mps"));
  source_desc.Add(_("aero turbulence force for the north axis"));
  source_unit.Add(wxT("M/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("atmosphere/e-turb-fps"));
  source_desc.Add(_("aero turbulence force for the east axis"));
  source_unit.Add(wxT("FT/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("atmosphere/e-turb-mps"));
  source_desc.Add(_("aero turbulence force for the east axis"));
  source_unit.Add(wxT("M/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("atmosphere/d-turb-fps"));
  source_desc.Add(_("aero turbulence force for the down axis"));
  source_unit.Add(wxT("FT/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("atmosphere/d-turb-mps"));
  source_desc.Add(_("aero turbulence force for the down axis"));
  source_unit.Add(wxT("M/SEC"));
  source_access.Add(wxT("RW"));
  source_comment.Add(_("aero turbulence force in the local frame"));

  source.Add(wxT("aero/cl-squared-norm"));
  source_desc.Add(_("square coefficient of lift"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic lift force divided by (qbar * WingArea)"));

  source.Add(wxT("aero/qbar-area"));
  source_desc.Add(_("qbar * WingArea"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("qbar multiplied by WingArea"));

  source.Add(wxT("aero/qbar-area-newton"));
  source_desc.Add(_("qbar * WingArea"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("qbar multiplied by WingArea"));

  source.Add(wxT("aero/alpha-max-deg"));
  source_desc.Add(_("accessible max alpha angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/alpha-min-deg"));
  source_desc.Add(_("accessible min alpha angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/bi2vel"));
  source_desc.Add(_("WingSpan divided by double velocity"));
  source_unit.Add(wxT("SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("velocities/p-rad_sec * aero/bi2vel = p-norm"));

  source.Add(wxT("aero/ci2vel"));
  source_desc.Add(_("chord divided by double velocity"));
  source_unit.Add(wxT("SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("velocities/q-rad_sec * aero/ci2vel = q-norm"));

  source.Add(wxT("aero/alpha-wing-rad"));
  source_desc.Add(_("alpha angle for the wing"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero/alpha-rad + metrics/iw-deg * deg2rad"));

  source.Add(wxT("systems/stall-warn-norm"));
  source_desc.Add(_("stall warn signal"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("10*(alpha/alphaclmax - 0.85)"));

  source.Add(wxT("aero/stall-hyst-norm"));
  source_desc.Add(_("stall hysteresis flag"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("if (alpha > alphahystmax) stall_hyst = 1; else stall_hyst = 0"));

  source.Add(wxT("aero/alpha-rad"));
  source_desc.Add(_("alpha angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/alpha-deg"));
  source_desc.Add(_("alpha angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/beta-rad"));
  source_desc.Add(_("beta angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/beta-deg"));
  source_desc.Add(_("beta angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/mag-beta-rad"));
  source_desc.Add(_("beta angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("absolute magnitude of beta angle"));

  source.Add(wxT("aero/mag-beta-deg"));
  source_desc.Add(_("beta angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("absolute magnitude of beta angle"));

  source.Add(wxT("aero/qbar-psf"));
  source_desc.Add(_("Q bar"));
  source_unit.Add(wxT("PSF"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/qbar-pa"));
  source_desc.Add(_("Q bar"));
  source_unit.Add(wxT("Pa"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  /****************************************
  source.Add(wxT("aero/qbarUW-psf"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/qbarUV-psf"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  *****************************************/

  source.Add(wxT("aero/alphadot-rad_sec"));
  source_desc.Add(_("alpha dot"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("d(alpha) / dt"));

  source.Add(wxT("aero/alphadot-deg_sec"));
  source_desc.Add(_("alpha dot"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("d(alpha) / dt"));

  source.Add(wxT("aero/betadot-rad_sec"));
  source_desc.Add(_("beta dot"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("d(beta) / dt"));

  source.Add(wxT("aero/betadot-deg_sec"));
  source_desc.Add(_("beta dot"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("d(beta) / dt"));

  /*************************************
  source.Add(wxT("aero/h_b-cg-ft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("aero/h_b-mac-ft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  *************************************/

  source.Add(wxT("forces/fbx-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the X axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fbx-aero-newton"));
  source_desc.Add(_("aerodynamic force along the X axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fby-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the Y axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fby-aero-newton"));
  source_desc.Add(_("aerodynamic force along the Y axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fbz-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the Z axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fbz-aero-newton"));
  source_desc.Add(_("aerodynamic force along the Z axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the body frame"));

  source.Add(wxT("forces/fwx-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the X axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/fwx-aero-newton"));
  source_desc.Add(_("aerodynamic force along the X axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/fwy-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the Y axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/fwy-aero-newton"));
  source_desc.Add(_("aerodynamic force along the Y axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/fwz-aero-lbs"));
  source_desc.Add(_("aerodynamic force along the Z axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/fwz-aero-newton"));
  source_desc.Add(_("aerodynamic force along the Z axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic force only in the wind frame"));

  source.Add(wxT("forces/lod-norm"));
  source_desc.Add(_("aerodynamic force load ratio"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic lift force divided by aerodynamic drag force"));

  source.Add(wxT("forces/nlf-norm"));
  source_desc.Add(_("longitude force load"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("longitude force divided by gravity, it is 1 normally."));

  source.Add(wxT("forces/mag-nlf-total-norm"));
  source_desc.Add(_("absolute magnitude of all force load"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force divided by gravity, it is 1 normally."));

  source.Add(wxT("forces/fbx-total-lbs"));
  source_desc.Add(_("total force along the X axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/fbx-total-n"));
  source_desc.Add(_("total force along the X axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/fby-total-lbs"));
  source_desc.Add(_("total force along the Y axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/fby-total-n"));
  source_desc.Add(_("total force along the Y axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/fbz-total-lbs"));
  source_desc.Add(_("total force along the Z axis"));
  source_unit.Add(wxT("LBS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/fbz-total-n"));
  source_desc.Add(_("total force along the Z axis"));
  source_unit.Add(wxT("N"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total force in the body frame"));

  source.Add(wxT("forces/nbx-total-norm"));
  source_desc.Add(_("total load along the X axis"));
  source_unit.Add(wxT("G"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total load in the body frame"));

  source.Add(wxT("forces/nby-total-norm"));
  source_desc.Add(_("total load along the Y axis"));
  source_unit.Add(wxT("G"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total load in the body frame"));

  source.Add(wxT("forces/nbz-total-norm"));
  source_desc.Add(_("total load along the Z axis"));
  source_unit.Add(wxT("G"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total load in the body frame, it is -1 normally."));

  /*******************************************
  source.Add(wxT("forces/nsx-total-norm"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("forces/nsy-total-norm"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("forces/nsz-total-norm"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("forces/fbx-gear-lbs"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("forces/fby-gear-lbs"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("forces/fbz-gear-lbs"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  ********************************************/

  source.Add(wxT("moments/l-aero-lbsft"));
  source_desc.Add(_("aerodynamic moment along the X axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/l-aero-n_m"));
  source_desc.Add(_("aerodynamic moment along the X axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/m-aero-lbsft"));
  source_desc.Add(_("aerodynamic moment along the Y axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/m-aero-n_m"));
  source_desc.Add(_("aerodynamic moment along the Y axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/n-aero-lbsft"));
  source_desc.Add(_("aerodynamic moment along the Z axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/n-aero-n_m"));
  source_desc.Add(_("aerodynamic moment along the Z axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aerodynamic moment in the body frame at ARP"));

  source.Add(wxT("moments/l-total-lbsft"));
  source_desc.Add(_("total moment along the X axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  source.Add(wxT("moments/l-total-nm"));
  source_desc.Add(_("total moment along the X axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  source.Add(wxT("moments/m-total-lbsft"));
  source_desc.Add(_("total moment along the Y axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  source.Add(wxT("moments/m-total-nm"));
  source_desc.Add(_("total moment along the Y axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  source.Add(wxT("moments/n-total-lbsft"));
  source_desc.Add(_("total moment along the Z axis"));
  source_unit.Add(wxT("LBS*FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  source.Add(wxT("moments/n-total-nm"));
  source_desc.Add(_("total moment along the Z axis"));
  source_unit.Add(wxT("N*M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total moment in the body frame at ARP"));

  /*******************************************************
  source.Add(wxT("moments/l-gear-lbsft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("moments/m-gear-lbsft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("moments/n-gear-lbsft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  ********************************************************/
    
  source.Add(wxT("accelerations/a-pilot-x-ft_sec2"));
  source_desc.Add(_("acceleration at pilot along the X axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/a-pilot-x-m_sec2"));
  source_desc.Add(_("acceleration at pilot along the X axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/a-pilot-y-ft_sec2"));
  source_desc.Add(_("acceleration at pilot along the Y axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/a-pilot-y-m_sec2"));
  source_desc.Add(_("acceleration at pilot along the Y axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/a-pilot-z-ft_sec2"));
  source_desc.Add(_("acceleration at pilot along the Z axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/a-pilot-z-m_sec2"));
  source_desc.Add(_("acceleration at pilot along the Z axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/n-pilot-x-norm"));
  source_desc.Add(_("load at pilot along the X axis"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("load in the body frame"));

  source.Add(wxT("accelerations/n-pilot-y-norm"));
  source_desc.Add(_("load at pilot along the Y axis"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("load in the body frame"));

  source.Add(wxT("accelerations/n-pilot-z-norm"));
  source_desc.Add(_("load at pilot along the Z axis"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("load in the body frame"));

  source.Add(wxT("accelerations/pdot-deg_sec"));
  source_desc.Add(_("angular acceleration along the X axis"));
  source_unit.Add(wxT("DEG/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/pdot-rad_sec"));
  source_desc.Add(_("angular acceleration along the X axis"));
  source_unit.Add(wxT("RAD/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/qdot-deg_sec"));
  source_desc.Add(_("angular acceleration along the Y axis"));
  source_unit.Add(wxT("DEG/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/qdot-rad_sec"));
  source_desc.Add(_("angular acceleration along the Y axis"));
  source_unit.Add(wxT("RAD/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/rdot-deg_sec"));
  source_desc.Add(_("angular acceleration along the Z axis"));
  source_unit.Add(wxT("DEG/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/rdot-rad_sec"));
  source_desc.Add(_("angular acceleration along the Z axis"));
  source_unit.Add(wxT("RAD/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular acceleration in the body frame"));

  source.Add(wxT("accelerations/udot-fps"));
  source_desc.Add(_("acceleration along the X axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/udot-mps"));
  source_desc.Add(_("acceleration along the X axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/vdot-fps"));
  source_desc.Add(_("acceleration along the Y axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/vdot-mps"));
  source_desc.Add(_("acceleration along the Y axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/wdot-fps"));
  source_desc.Add(_("acceleration along the Z axis"));
  source_unit.Add(wxT("FT/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("accelerations/wdot-mps"));
  source_desc.Add(_("acceleration along the Z axis"));
  source_unit.Add(wxT("M/SEC^2"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("acceleration in the body frame"));

  source.Add(wxT("velocities/vc-fps"));
  source_desc.Add(_("calibrated speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/vc-kts"));
  source_desc.Add(_("calibrated speed"));
  source_unit.Add(wxT("KTS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/vc-mps"));
  source_desc.Add(_("calibrated speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/ve-fps"));
  source_desc.Add(_("equivalent speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/ve-kts"));
  source_desc.Add(_("equivalent speed"));
  source_unit.Add(wxT("KTS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/ve-mps"));
  source_desc.Add(_("equivalent speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  /**************************************
  source.Add(wxT("velocities/machU"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  **************************************/

  source.Add(wxT("velocities/tat-r"));
  source_desc.Add(_("total temperature"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total temperature in degrees Rankine"));

  source.Add(wxT("velocities/tat-c"));
  source_desc.Add(_("total temperature"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("total temperature in degrees Celsius"));

  source.Add(wxT("velocities/pt-lbs_sqft"));
  source_desc.Add(_("total pressure"));
  source_unit.Add(wxT("PSF"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/pt-pa"));
  source_desc.Add(_("total pressure"));
  source_unit.Add(wxT("Pa"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/p-aero-deg_sec"));
  source_desc.Add(_("angular velocity along the X axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/p-aero-rad_sec"));
  source_desc.Add(_("angular velocity along the X axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/q-aero-deg_sec"));
  source_desc.Add(_("angular velocity along the Y axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/q-aero-rad_sec"));
  source_desc.Add(_("angular velocity along the Y axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/r-aero-deg_sec"));
  source_desc.Add(_("angular velocity along the Z axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/r-aero-rad_sec"));
  source_desc.Add(_("angular velocity along the Z axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular velocity in the body frame"));

  source.Add(wxT("velocities/phidot-deg_sec"));
  source_desc.Add(_("angular speed for the phi angle"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/phidot-rad_sec"));
  source_desc.Add(_("angular speed for the phi angle"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/thetadot-deg_sec"));
  source_desc.Add(_("angular speed for the theta angle"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/thetadot-rad_sec"));
  source_desc.Add(_("angular speed for the theta angle"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/psidot-deg_sec"));
  source_desc.Add(_("angular speed for the psi angle"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/psidot-rad_sec"));
  source_desc.Add(_("angular speed for the psi angle"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/u-aero-fps"));
  source_desc.Add(_("velocity along the X axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/u-aero-mps"));
  source_desc.Add(_("velocity along the X axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/v-aero-fps"));
  source_desc.Add(_("velocity along the Y axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/v-aero-mps"));
  source_desc.Add(_("velocity along the Y axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/w-aero-fps"));
  source_desc.Add(_("velocity along the Z axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/w-aero-mps"));
  source_desc.Add(_("velocity along the Z axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("aero velocity in the body frame, the wind is considered"));

  source.Add(wxT("velocities/vt-fps"));
  source_desc.Add(_("the true aero speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the wind is considered"));

  source.Add(wxT("velocities/vt-mps"));
  source_desc.Add(_("the true aero speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the wind is considered"));

  source.Add(wxT("velocities/mach-norm"));
  source_desc.Add(_("the mach number"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the wind is considered"));

  source.Add(wxT("velocities/vg-fps"));
  source_desc.Add(_("the ground speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("refer to the ground"));

  source.Add(wxT("velocities/vg-mps"));
  source_desc.Add(_("the ground speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("refer to the ground"));

  source.Add(wxT("velocities/h-dot-fps"));
  source_desc.Add(_("vertical speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/h-dot-mps"));
  source_desc.Add(_("vertical speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("velocities/v-north-fps"));
  source_desc.Add(_("northward speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/v-north-mps"));
  source_desc.Add(_("northward speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/v-east-fps"));
  source_desc.Add(_("eastward speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/v-east-mps"));
  source_desc.Add(_("eastward speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/v-down-fps"));
  source_desc.Add(_("downward speed"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/v-down-mps"));
  source_desc.Add(_("downward speed"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the local frame"));

  source.Add(wxT("velocities/u-fps"));
  source_desc.Add(_("velocity along the X axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/u-mps"));
  source_desc.Add(_("velocity along the X axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/v-fps"));
  source_desc.Add(_("velocity along the Y axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/v-mps"));
  source_desc.Add(_("velocity along the Y axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/w-fps"));
  source_desc.Add(_("velocity along the Z axis"));
  source_unit.Add(wxT("FPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/w-mps"));
  source_desc.Add(_("velocity along the Z axis"));
  source_unit.Add(wxT("MPS"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("speed in the body frame"));

  source.Add(wxT("velocities/p-deg_sec"));
  source_desc.Add(_("angular velocity along the X axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("velocities/p-rad_sec"));
  source_desc.Add(_("angular velocity along the X axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("velocities/q-deg_sec"));
  source_desc.Add(_("angular velocity along the Y axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("velocities/q-rad_sec"));
  source_desc.Add(_("angular velocity along the Y axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("velocities/r-deg_sec"));
  source_desc.Add(_("angular velocity along the Z axis"));
  source_unit.Add(wxT("DEG/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("velocities/r-rad_sec"));
  source_desc.Add(_("angular velocity along the Z axis"));
  source_unit.Add(wxT("RAD/SEC"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("angular speed in the body frame"));

  source.Add(wxT("attitude/phi-deg"));
  source_desc.Add(_("roll angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/phi-rad"));
  source_desc.Add(_("roll angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/roll-deg"));
  source_desc.Add(_("roll angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/roll-rad"));
  source_desc.Add(_("roll angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/theta-deg"));
  source_desc.Add(_("pitch angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/theta-rad"));
  source_desc.Add(_("pitch angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/pitch-deg"));
  source_desc.Add(_("pitch angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/pitch-rad"));
  source_desc.Add(_("pitch angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/psi-deg"));
  source_desc.Add(_("heading angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/psi-rad"));
  source_desc.Add(_("heading angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/heading-true-deg"));
  source_desc.Add(_("heading angle"));
  source_unit.Add(wxT("DEG"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("attitude/heading-true-rad"));
  source_desc.Add(_("heading angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("the Euler angle"));

  source.Add(wxT("position/epa-rad"));
  source_desc.Add(_("earch position angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("earthPosAngle += State->Getdt()*Inertial->omega();"));

  source.Add(wxT("position/h-sl-ft"));
  source_desc.Add(_("height from sea level"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("position/h-sl-m"));
  source_desc.Add(_("height from sea level"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("position/lat-gc-rad"));
  source_desc.Add(_("latitude"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("position/long-gc-rad"));
  source_desc.Add(_("longitude"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("position/h-agl-ft"));
  source_desc.Add(_("height from runway"));
  source_unit.Add(wxT("FT"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  source.Add(wxT("position/h-agl-m"));
  source_desc.Add(_("height from runway"));
  source_unit.Add(wxT("M"));
  source_access.Add(wxT("RO"));
  source_comment.Add(wxT(""));

  /************************************************
  source.Add(wxT("position/radius-to-vehicle-ft"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("metrics/runway-radius"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  ***********************************************/

  source.Add(wxT("flight-path/gamma-rad"));
  source_desc.Add(_("gamma angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("vertical flight path angle, from -pi/2 ~ pi/2"));

  source.Add(wxT("flight-path/psi-gt-rad"));
  source_desc.Add(_("psi angle"));
  source_unit.Add(wxT("RAD"));
  source_access.Add(wxT("RO"));
  source_comment.Add(_("horizontal flight path angle, from 0 ~ 2pi"));

  source.Add(wxT("fcs/aileron-cmd-norm"));
  source_desc.Add(_("aileron command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  source.Add(wxT("fcs/elevator-cmd-norm"));
  source_desc.Add(_("elevator command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1, must appear in FCS modal"));

  source.Add(wxT("fcs/rudder-cmd-norm"));
  source_desc.Add(_("rudder command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1, must appear in FCS modal"));

  source.Add(wxT("fcs/steer-cmd-norm"));
  source_desc.Add(_("steer command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  source.Add(wxT("fcs/flap-cmd-norm"));
  source_desc.Add(_("flap command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  source.Add(wxT("fcs/speedbrake-cmd-norm"));
  source_desc.Add(_("speedbrake"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  source.Add(wxT("fcs/spoiler-cmd-norm"));
  source_desc.Add(_("spoiler command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  source.Add(wxT("fcs/pitch-trim-cmd-norm"));
  source_desc.Add(_("pitch trim command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1, must appear in FCS modal"));

  source.Add(wxT("fcs/roll-trim-cmd-norm"));
  source_desc.Add(_("roll trim command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1, must appear in FCS modal"));

  source.Add(wxT("fcs/yaw-trim-cmd-norm"));
  source_desc.Add(_("yaw trim command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1, must appear in FCS modal"));

  source.Add(wxT("gear/gear-cmd-norm"));
  source_desc.Add(_("gear command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("range from 0 to 1"));

  /*****************************************
  source.Add(wxT("gear/num-units"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));


  *****************************************/

  /*****************************************
  source.Add(wxT("propulsion/magneto_cmd"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("propulsion/starter_cmd"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));

  source.Add(wxT("propulsion/cutoff_cmd"));
  source_desc.Add(wxT(""));
  source_unit.Add(wxT(""));
  source_access.Add(wxT(""));
  source_comment.Add(wxT(""));
  ********************************************/

  source.Add(wxT("propulsion/active_engine"));
  source_desc.Add(_("active engine"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("-1 means that all engines are running"));

  source.Add(wxT("fcs/throttle-cmd-norm"));
  source_desc.Add(_("throttle command"));
  source_unit.Add(wxT("norm"));
  source_access.Add(wxT("R/W"));
  source_comment.Add(_("from 0 to 1, it must appear in the FCS modal"));


  destination.Add(wxT("fcs/left-aileron-pos-deg"));
  destination_desc.Add(_("left aileron positon"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/left-aileron-pos-rad"));
  destination_desc.Add(_("left aileron positon"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/right-aileron-pos-deg"));
  destination_desc.Add(_("right aileron position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/right-aileron-pos-rad"));
  destination_desc.Add(_("right aileron position"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/elevator-pos-deg"));
  destination_desc.Add(_("elevator position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/elevator-pos-rad"));
  destination_desc.Add(_("elevator position"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/rudder-pos-deg"));
  destination_desc.Add(_("rudder position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/rudder-pos-rad"));
  destination_desc.Add(_("rudder position"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("it must appear in the FCS modal"));

  destination.Add(wxT("fcs/throttle-pos-norm"));
  destination_desc.Add(_("throttle position"));
  destination_unit.Add(wxT("norm"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("from 0 to 2, it must appear in the FCS modal"));

  destination.Add(wxT("propulsion/pitch-deg"));
  destination_desc.Add(_("throttle attitude"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("use propulsion/pitch-deg[1] for the second engine"));

  destination.Add(wxT("propulsion/yaw-deg"));
  destination_desc.Add(_("throttle attitude"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("use propulsion/yaw-deg[1] for the second engine"));

  destination.Add(wxT("gear/gear-pos-norm"));
  destination_desc.Add(_("gear position"));
  destination_unit.Add(wxT("norm"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/flap-pos-norm"));
  destination_desc.Add(_("flap position norm"));
  destination_unit.Add(wxT("norm"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/flap-pos-deg"));
  destination_desc.Add(_("flap position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/speedbrake-pos-norm"));
  destination_desc.Add(_("speedbrake position"));
  destination_unit.Add(wxT("norm"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/speedbrake-pos-deg"));
  destination_desc.Add(_("speedbrake position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/speedbrake-pos-rad"));
  destination_desc.Add(_("speedbrake position"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/spoiler-pos-norm"));
  destination_desc.Add(_("spoiler position"));
  destination_unit.Add(wxT("norm"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/spoiler-pos-deg"));
  destination_desc.Add(_("spoiler position"));
  destination_unit.Add(wxT("DEG"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  destination.Add(wxT("fcs/spoiler-pos-rad"));
  destination_desc.Add(_("spoiler position"));
  destination_unit.Add(wxT("RAD"));
  destination_access.Add(wxT("R/W"));
  destination_comment.Add(_("do not use the other one together"));

  /*****************************************************
  destination.Add(wxT("fcs/left-aileron-pos-norm"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-left-aileron-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/right-aileron-pos-norm"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-right-aileron-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/elevator-pos-norm"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-elevator-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/rudder-pos-norm"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-rudder-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));
  
  destination.Add(wxT("fcs/flap-pos-deg"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-speedbrake-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  destination.Add(wxT("fcs/mag-spoiler-pos-rad"));
  destination_desc.Add(wxT(""));
  destination_unit.Add(wxT(""));
  destination_access.Add(wxT(""));
  destination_comment.Add(wxT(""));

  *************************************************/


  for (int i = 0; i < source.Count(); ++i)
  {
    all.Add(source[i]);
    all_desc.Add(source_desc[i]);
    all_unit.Add(source_unit[i]);
    all_access.Add(source_access[i]);
    all_comment.Add(source_comment[i]);
  }
  for (int i = 0; i < destination.Count(); ++i)
  {
    all.Add(destination[i]);
    all_desc.Add(destination_desc[i]);
    all_unit.Add(destination_unit[i]);
    all_access.Add(destination_access[i]);
    all_comment.Add(destination_comment[i]);
  }
}

#endif //INITPROPS_H

