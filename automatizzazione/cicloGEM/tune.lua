--[[ 
Compatibile con SIMION 8.1
Scansiona V1 e V2, esegue una simulazione per ciascuna coppia,
salva i dati iniziali/finali di ogni ione in un file separato. Cicla su diversi file gem
--]]

simion.workbench_program()

local GEM = simion.import 'gemlib.lua'
simion.import 'contourlib.lua'

-- === PARAMETRI DI CONTROLLO ===
adjustable threshold    = -100
adjustable V1_begin     = -100
adjustable V1_step      = 10
adjustable V1_count     = 10
adjustable energy       = 0.01

-- file GEM
local gem_list = {}
for i = 0, 20, 10 do -- partenza, fine inclusa, incremento
  table.insert(gem_list, string.format("tune_shift_%d.gem", i))
end

-- === VARIABILI LOCALI ===
local V1_voltage, V2_voltage
local data_file
local first
local data = {}

-- === ROUTINE PRINCIPALE DI SIMULAZIONE ===
function segment.flym()

  os.execute("mkdir output")
  os.remove("output/results.csv")

  local results_file = assert(io.open("output/results.csv", "w"))
  local inst = simion.wb.instances[1]

  -- === CICLO SULLE GEOMETRIE ===
  for _, gemfile in ipairs(gem_list) do

    simion.print_screen("=== Carico GEM: " .. gemfile .. " ===")

    -- aggiorna la PA con questo GEM
    GEM.update_painst_from_gem(inst, gemfile, "")

    -- refine richiesto da SIMION 8.1
    inst.pa:refine{convergence = 1e-5}

    -- === CICLO SUI VOLTAGGI ===
    for V1_pos = 1, V1_count do
      V1_voltage = V1_begin + (V1_pos - 1) * V1_step
      V2_voltage = V1_voltage - threshold

      -- crea file di output per questa simulazione
      local fname = string.format(
        "output/%s_V1_%d_V2_%d.txt",
        gemfile, V1_voltage, V2_voltage
      )

      data_file = assert(io.open(fname, "w"))
      data_file:write("ion, x, y, z, vx, vy, vz, energy, elev, azim\n")

      -- esegue simulazione
      run()

      data_file:close()

      -- screenshot
      local img_filename =
         string.format("output/%s_V1_%d_V2_%d.png",
                        gemfile, V1_voltage, V2_voltage)
      simion.printer.type = "png"
      simion.printer.filename = img_filename
      simion.printer.scale = 1 -- se metto 2 ho il doppio dei pixel ma è più lento nell'acquisizione
      simion.print_screen()

      -- stampa risultati nel file CSV/HTML
      results_file:write(
        string.format("%s, V1=%d, V2=%d\n",
                      gemfile, V1_voltage, V2_voltage)
      )
      results_file:flush()
    end
  end

  results_file:close()
  os.execute([[start notepad output\results.csv]])
end

-- === VOLTAGE ASSIGNMENT ===
function segment.init_p_values()
  adj_elect01 = V1_voltage
  adj_elect02 = V1_voltage
  adj_elect03 = V1_voltage
  adj_elect04 = V1_voltage
  adj_elect05 = V2_voltage
  adj_elect06 = V2_voltage
  adj_elect07 = 0
  adj_elect08 = 0
end

-- === RUN INIT ===
function segment.initialize_run()
  sim_rerun_flym = 0
  first = true
  data = {}
end

-- === FINE RUN ===
function segment.terminate_run()
  sim_rerun_flym = 1
end

function segment.other_actions()
  if first then
    first = false
    sim_update_pe_surface = 1
  end
end

-- === INIZIALIZZAZIONE SINGOLO IONE ===
function segment.initialize()
  local eV = energy
  local m = ion_mass or 1.0
  local new_speed = ke_to_speed(eV, m)
  local old_speed = rect3d_to_polar3d(ion_vx_mm, ion_vy_mm, ion_vz_mm)
  if old_speed == 0 then old_speed = 1 end

  ion_vx_mm = ion_vx_mm * new_speed / old_speed
  ion_vy_mm = ion_vy_mm * new_speed / old_speed
  ion_vz_mm = ion_vz_mm * new_speed / old_speed

  local vmag = math.sqrt(ion_vx_mm^2 + ion_vy_mm^2 + ion_vz_mm^2)
  local elev = vmag > 0 and math.deg(math.atan2(
      ion_vx_mm, math.sqrt(ion_vy_mm^2 + ion_vz_mm^2))) or 0
  local azim = math.deg(math.atan2(-ion_vy_mm, ion_vz_mm))

  data[ion_number] = {
    x0=ion_px_gu, y0=ion_py_gu, z0=ion_pz_gu,
    vx0=ion_vx_mm, vy0=ion_vy_mm, vz0=ion_vz_mm,
    ke0=eV, elev0=elev, azim0=azim
  }
end

-- === FINE TRAIETTORIA SINGOLO IONE ===
function segment.terminate()
  local init = data[ion_number]
  if not init then return end

  local vmag = math.sqrt(ion_vx_mm^2+ion_vy_mm^2+ion_vz_mm^2)
  local elev = math.deg(math.atan2(
      ion_vx_mm, math.sqrt(ion_vy_mm^2 + ion_vz_mm^2)))
  local azim = math.deg(math.atan2(-ion_vy_mm, ion_vz_mm))

  local ke1 =
    0.5 * (ion_mass*1.66054e-27) * (vmag*1000)^2 / 1.602e-19

 if data_file then
  data_file:write(string.format("%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n %d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f \n",
    ion_number, init.x0, init.y0, init.z0, init.vx0, init.vy0, init.vz0, init.ke0, init.elev0, init.azim0,
	ion_number, ion_px_gu, ion_py_gu, ion_pz_gu, ion_vx_mm, ion_vy_mm, ion_vz_mm, ke1, elev, azim))
end
  data[ion_number] = nil
end