--generazione particelle da file esterno .ion

simion.workbench_program()

local ION_FILENAME = "particles.ion"

adjustable threshold = -100
adjustable V1_begin  = -100
adjustable V1_step   = 10
adjustable V1_count  = 2
adjustable energy    = 0.01   -- se >0 forza energia, se 0 usa quella del file

--=== DEFINIZIONE VARIABILI GLOBALI ===
local ion_defs = {}
local data = {}
local data_file
local V1_voltage, V2_voltage
local first = true

--=== DEFINIZIONE FUNZIONI UTILI ===
local function deg2rad(d) return d * math.pi / 180 end

-- tokenizza riga file .ion
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

-- lettura file .ion
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
  print("Letti "..#ion_defs.." ioni.")
  return #ion_defs > 0
end

-- KE ? velocità (mm/usec)
local function ke_to_speed_mm_per_usec(ke_eV, mass_amu)
  if type(ke_to_speed) == "function" then
    return ke_to_speed(ke_eV, mass_amu)
  end
  local e = 1.602176634e-19
  local amu = 1.66054e-27
  local v = math.sqrt(2*(ke_eV*e)/(mass_amu*amu))
  return v*0.001    -- m/s ? mm/usec
end

-- calcolo vx,vy,vz da Vt + angoli
local function compute_velocity(Vt, az, el)
  local azr = deg2rad(az)
  local elr = deg2rad(el)
  local vx = Vt * math.sin(elr)
  local rperp = Vt * math.cos(elr)
  local vy = - rperp * math.sin(azr)
  local vz =   rperp * math.cos(azr)
  return vx,vy,vz
end


--=== CICLO V (non attivo perché ho un solo file .ion) ===
function segment.flym()
  os.execute("mkdir output")

  local results_file = assert(io.open("output/results.csv","w"))

  for i=1,V1_count do
    V1_voltage = V1_begin + (i-1)*V1_step
    V2_voltage = V1_voltage - threshold

    local fname = string.format("output/simulazione_V1%d_V2%d.txt",V1_voltage,V2_voltage)
    data_file = assert(io.open(fname,"w"))
    data_file:write("ion, x0, y0, z0, vx0, vy0, vz0, ke0, elev0, azim0, x1, y1, z1, vx1, vy1, vz1, ke1, elev1, azim1\n")

    run()

    data_file:close()

    -- screenshot
    simion.printer.type = "png"
    simion.printer.filename = "output/results_V1_" .. V1_voltage .. "V2_" .. V2_voltage .. ".png"
    simion.printer.scale = 1
    simion.print_screen()

    --stampa risultati file CSV
    results_file:write("V1="..V1_voltage.."  V2="..V2_voltage.."\n")
    results_file:flush()
  end

  results_file:close()
  os.execute([[start notepad output\results.csv]])
end

--=== RUN INIT ===
function segment.initialize_run()
  sim_rerun_flym = 0
  sim_trajectory_image_control = 0
  first = true

  read_ion_file(ION_FILENAME)

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

--=== INIZIALIZZAZIONE SINGOLO IONE DA FILE .ION ===
function segment.initialize()
  local def = ion_defs[ion_number]
  if not def then return end

  -- MASSA + CARICA
  ion_mass   = def.mass
  ion_charge = def.charge

  -- POSIZIONI — in mm (SIMION 8.1 fa conversione interna)
  ion_px_mm = def.x
  ion_py_mm = def.y
  ion_pz_mm = def.z

  -- VELOCITA'
  local KE = (energy>0) and energy or def.ke
  local Vt = ke_to_speed_mm_per_usec(KE, ion_mass)
  local vx,vy,vz = compute_velocity(Vt, def.az, def.el)

  ion_vx_mm = vx
  ion_vy_mm = vy
  ion_vz_mm = vz

  -- salva stato iniziale
  local vmag = math.sqrt(vx*vx + vy*vy + vz*vz)
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
      ion_number,init.x0,init.y0,init.z0,init.vx0,init.vy0,init.vz0,init.ke0,init.elev0,init.azim0,             ion_number,ion_px_mm,ion_py_mm,ion_pz_mm,vx,vy,vz,ke1,elev,azim))
  end

  data[ion_number] = nil

end