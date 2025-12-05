-- genera_ion.lua
-- Crea un file particles.ion con ordine:
-- tob, mass, charge, x, y, z, az, el, KE, cwf, color

local outfile = "particles.ion"

local N = 500

local R = 100
local x0 = 230
local y0 = 0
local z0 = 0

local KE_eV = 2
local mass_amu = 0.00054857990946
local charge = -1
local color = 0
local cwf = 1
local tob = 0

local eV_to_J  = 1.602176634e-19
local amu_to_kg = 1.66054e-27

local function ke_to_speed_mm_us(ke_eV, mass_amu)
  local ke_J = ke_eV * eV_to_J
  local m = mass_amu * amu_to_kg
  local v_m_s = math.sqrt(2 * ke_J / m)
  return v_m_s * 0.001   -- mm/us
end

local function sample_cone(alpha)
  local cmin = math.cos(alpha)
  local u = math.random()
  local cos_t = 1 - u * (1 - cmin)
  local sin_t = math.sqrt(1 - cos_t*cos_t)
  local phi = 2 * math.pi * math.random()
  return cos_t, sin_t*math.cos(phi), sin_t*math.sin(phi)
end

local function sample_disk(R)
  local u = math.random()
  local r = R * math.sqrt(u)
  local a = 2*math.pi * math.random()
  return r*math.cos(a), r*math.sin(a)
end

math.randomseed(os.time())

local vmag = ke_to_speed_mm_us(KE_eV, mass_amu)

local f = assert(io.open(outfile, "w"))

for i = 1, N do
  local yrel, zrel = sample_disk(R)

  local x = x0
  local y = y0 + yrel
  local z = z0 + zrel

  local dx, dy, dz = sample_cone(math.rad(90))

  local vx = dx * vmag
  local vy = dy * vmag
  local vz = dz * vmag

  -- conversione in formato SIMION
  local Vt   = math.sqrt(vx*vx + vy*vy + vz*vz)
  local elev = math.deg(math.atan2(vx, math.sqrt(vy*vy + vz*vz)))
  local azm  = math.deg(math.atan2(-vy, vz))

  -- SCRITTURA NELL'ORDINE RICHIESTO
  f:write(string.format(
    "%d, %.14g, %d, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %d, %d\n",
    tob, mass_amu, charge,
    x, y, z,
    azm, elev, KE_eV,
    cwf, color
  ))
end

f:close()

print("Creato correttamente particles.ion con", N, "particelle")
