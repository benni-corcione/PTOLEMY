--generazione particelle da file .ion prodotto all'interno del programma

simion.workbench_program()

local ION_FILENAME = "particles.ion"

adjustable threshold = -100
adjustable V1_begin  = -100
adjustable V1_step   = 10
adjustable V1_count  = 2
adjustable energy    = 0.01     -- se >0 forza energia, se 0 usa quella del file

--=== VARIABILE GLOBALI ===
local ion_defs = {}
local data = {}
local data_file
local V1_voltage, V2_voltage
local first = true

--=== FUNZIONI UTILI ===
local function deg2rad(d) return d * math.pi / 180 end

local function tokenize(line)
  line = line:gsub("#.*","")
  line = line:gsub("^%s+",""):gsub("%s+$","")
  if line == "" then return {} end
  local t = {}
  for w in line:gmatch("[^,]+") do
    t[#t+1] = w:gsub("^%s+",""):gsub("%s+$","")
  end
  return t
end

local function read_ion_file(path)
  local f = io.open(path, "r")
  if not f then print("Errore apertura "..path) return false end

  ion_defs = {}
  local i=0
  for line in f:lines() do
    local tok = tokenize(line)
    if #tok >= 11 then
      i=i+1
      ion_defs[i] = {
        tob    = tonumber(tok[1]),
        mass   = tonumber(tok[2]),
        charge = tonumber(tok[3]),
        x      = tonumber(tok[4]),
        y      = tonumber(tok[5]),
        z      = tonumber(tok[6]),
        az     = tonumber(tok[7]),
        el     = tonumber(tok[8]),
        ke     = tonumber(tok[9]),
        cwf    = tonumber(tok[10]),
        color  = tonumber(tok[11])
      }
    end
  end
  f:close()
  print("Letti "..#ion_defs.." ioni")
  return #ion_defs > 0
end

local function ke_to_speed_mm_per_usec(ke_eV, mass_amu)
  local e = 1.602176634e-19
  local amu = 1.66054e-27
  local v = math.sqrt(2*(ke_eV*e)/(mass_amu*amu))
  return v*0.001
end

local function compute_velocity(Vt, az, el)
  local azr = deg2rad(az)
  local elr = deg2rad(el)
  local vx = Vt * math.sin(elr)
  local rperp = Vt * math.cos(elr)
  local vy = - rperp * math.sin(azr)
  local vz =   rperp * math.cos(elr)
  return vx,vy,vz
end

--=== GENERAZIONE FILE .ION PARTICELLE ===
local function generate_particles()
  local outfile = ION_FILENAME
  local N = 300        -- NUMERO DI PARTICELLE DA GENERARE
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
    local v = math.sqrt(2 * (ke_eV*eV_to_J) / (mass_amu*amu_to_kg))
    return v * 0.001
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

  math.randomseed(os.time()) -- ogni volta generate diversamente
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

    local Vt   = math.sqrt(vx*vx + vy*vy + vz*vz)
    local elev = math.deg(math.atan2(vx, math.sqrt(vy*vy + vz*vz)))
    local azm  = math.deg(math.atan2(-vy, vz))

    f:write(string.format("%d, %.14g, %d, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %d, %d\n",tob, mass_amu, charge, x, y, z, azm, elev, KE_eV, cwf, color))
  end

  f:close()

  --=== IMPOSTAZIONE NUMERO DI PARTICELLE ===
  particles = N   -- <<<  QUESTA È LA RIGA MAGICA  >>>

  print("Generato '"..outfile.."' con "..N.." particelle")
end

--=== CICLO SUI VOLTAGGI (ora sospeso perché ho un solo file .ion) ===
function segment.flym()
  os.execute("mkdir output")
  local results_file = assert(io.open("output/results.csv","w"))

  for i=1,V1_count do
    V1_voltage = V1_begin + (i-1)*V1_step
    V2_voltage = V1_voltage - threshold

    local fname = string.format("output/simulazione_V1_%d_V2_%d.txt",V1_voltage,V2_voltage)
    data_file = assert(io.open(fname,"w"))
    data_file:write("ion,x0,y0,z0,vx0,vy0,vz0,ke0,elev0,azim0,x1,y1,z1,vx1,vy1,vz1,ke1,elev1,azim1\n")

    run()
    data_file:close()

    simion.printer.type = "png"
    simion.printer.filename = string.format("output/results_V1_%d_V2_%d.png",V1_voltage,V2_voltage)
    simion.printer.scale = 1
    simion.print_screen()

    results_file:write("V1="..V1_voltage.."  V2="..V2_voltage.."\n")
  end

  results_file:close()
  os.execute([[start notepad output\results.csv]])
end

--=== RUN INIT ===
function segment.initialize_run()
  sim_rerun_flym = 0
  sim_trajectory_image_control = 0

  generate_particles()               -- genera particelle
  read_ion_file(ION_FILENAME)        -- le rilegge

  particles = #ion_defs              -- <<< FONDAMENTALE PER 8.1

  first = true
end

--=== ASSEGNAZIONE VOLTAGGI ===
function segment.init_p_values()
  adj_elect01 = V1_voltage
  adj_elect02 = V1_voltage
  adj_elect03 = V1_voltage
  adj_elect04 = V1_voltage
  adj_elect05 = V2_voltage
  adj_elect06 = V2_voltage
end

function segment.other_actions()
   if first then first=false sim_update_pe_surface=1 end
end

--=== INIZIALIZZAZIONE SINGOLI IONI DA FILE .ION ===
function segment.initialize()
  local def = ion_defs[ion_number]
  if not def then return end

  ion_mass   = def.mass
  ion_charge = def.charge

  ion_px_mm = def.x
  ion_py_mm = def.y
  ion_pz_mm = def.z

  local KE = (energy>0) and energy or def.ke
  local Vt = ke_to_speed_mm_per_usec(KE, ion_mass)
  local vx,vy,vz = compute_velocity(Vt, def.az, def.el)

  ion_vx_mm = vx
  ion_vy_mm = vy
  ion_vz_mm = vz

  local elev = math.deg(math.atan2(vx, math.sqrt(vy*vy+vz*vz)))
  local azim = math.deg(math.atan2(-vy, vz))

  data[ion_number] = {
    x0=def.x, y0=def.y, z0=def.z,
    vx0=vx, vy0=vy, vz0=vz,
    ke0=KE, elev0=elev, azim0=azim
  }
end

--=== TERMINATE RUN ===
function segment.terminate()
  local init = data[ion_number]
  if not init then return end

  local vx,vy,vz = ion_vx_mm, ion_vy_mm, ion_vz_mm
  local vmag = math.sqrt(vx*vx+vy*vy+vz*vz)
  local elev = math.deg(math.atan2(vx, math.sqrt(vy*vy+vz*vz)))
  local azim = math.deg(math.atan2(-vy, vz))
  local ke1 = 0.5*(ion_mass*1.66054e-27)*(vmag*1000)^2/1.602e-19

  if data_file then
    data_file:write(string.format(
      "%d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n%d, %.3f,%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n",
      ion_number,init.x0,init.y0,init.z0,init.vx0,init.vy0,init.vz0,init.ke0,init.elev0,init.azim0, 
      ion_number,ion_px_mm,ion_py_mm,ion_pz_mm,vx,vy,vz,ke1,elev,azim))
  end

  data[ion_number] = nil
end