--[[
 tune.lua

 SIMION user program for tune.pa0.

 D.Manura 2011-11,2006-08
 (c) 2006-2011 Scientific Instrument Services, Inc. (Licensed under SIMION 8.1)
--]]
 
simion.workbench_program()
simion.import 'contourlib.lua'

-- voltage for central electrode
-- Note: the default Value here was found using the _tune example.
adjustable test_voltage = 0

-- number of grid points to step electrode x location per run.
adjustable step_x = 5

adjustable V1_begin = -2000 -- Source
adjustable V2_begin = -2000   -- elettrodo
adjustable V1_step = 50   -- V1 voltage step size
adjustable V2_step = 50   -- V2 voltage step size
adjustable V1_count = 41  -- V1 voltage number of steps
adjustable V2_count = 41  -- V2 voltage number of steps
adjustable excel_enable = 1  -- Use Excel? (1=yes, 0=no)






local V1_voltage  -- current Value of voltage V1
local V2_voltage  -- current Value of voltage V2
local nhits      -- number of hits detected in current run
local esterni      -- number of hits detected in current run
local angoloInternoMax=0
local angoloI=0
local angoloEsternoMax=0
local angoloEsternoMax1=0
local angoloE=0
local aperturaMax=0
local apertura=0
-- Excel objects.
local excel
local function excel_begin()



  excel = simion.import 'C:/Program Files/SIMION-8.1/examples/excel/excellib.lua' . get_excel()
  excel.Visible = true
  excel_wb = excel.Workbooks:Add()
  excel_ws = excel_wb.Worksheets(1)
  excel_ws.Cells(1, 1).Value2 = "Number of particles transmitted for each combination of voltages V2,V1."
  excel_ws.Cells(2, 1).Value2 = "V2 - V1"
end


local b = 0.465  -- gauss
function segment.mfield_adjust()
  ion_bfieldx_gu = 0
  ion_bfieldy_gu = 0
  ion_bfieldz_gu = b
end


-- Utility for HTML file writing.
local Html = function(filename)
  local fh = assert(io.open(filename, "w"))
  fh:write('<html><body>\n')

  local self = {}
  function self:add_image(imgfilename)
    fh:write(('<img src="%s" />\n'):format(imgfilename))
  end
  function self:add_line(text)
    fh:write(('<div>%s</div>\n'):format(text))
  end
  function self:close()
    fh:write('</body></html>\n')
    fh:close()
  end
  return self
end

local result_y

-- Performs a simulation on a single parameterization
-- of the geometry.
-- dx is the axial x-offset in grid units.
-- returns calculated y-offset of outermost splat.
local function test(dx)
  
  -- convert GEM file to PA# file.
  _G.dx = dx  -- pass V1riable to GEM file.
  local pa = simion.open_gem('tune.gem'):to_pa()
      -- TODO: this function not yet documented
  pa:save('tune.pa#')
  pa:close()

  -- reload in workbench
  simion.wb.instances[1].pa:load('tune.pa#')
  
  -- refine PA# file.
  simion.wb.instances[1].pa:refine()
 angoloInternoMax=0
 angoloI=0
 angoloEsternoMax=0
 angoloEsternoMax1=0
 angoloE=0  
  -- Fly ions and collect results.
  run()

--  print("RESULT: dx=" .. dx .. ", ion_py_gu=" .. 0)
 
  return 0
end


-- called on Fly'm and expected to initiate runs by calling `run()`.
function segment.flym()
  -- Initialize Excel speadsheet (if enabled).
  if excel_enable ~= 0 then
    excel_begin()
  end


  -- Simulate all parameterizations.
  -- Results are summarized in results.csv.
  
  -- enable 'output' directory exists
  os.execute("mkdir output")

  -- cleanup any old files.
  os.remove("output/results.csv")

  -- Open output files.
  local results_file = assert(io.open("output/results.csv", "w")) -- write mode
  results_file:write("dx, dy\n")
  local html = Html("output/results.html")

  -- run each test, collecting results to results.csv
    -- Run simulation.
  for V2_pos = 1,V2_count do

  for V1_pos = 1,V1_count do
  
      -- Prepare for this run.
      V1_voltage = V1_begin + (V1_pos - 1) * V1_step
      V2_voltage = V2_begin + (V2_pos - 1) * V2_step



 angoloInternoMax=0
 angoloI=0
 angoloEsternoMax=0
 angoloEsternoMax1=0
 angoloE=0 
aperturaMax=0
apertura=0



      -- Perform trajectory calculation run.
      run()

      -- Record results.
--      print('result: V1='..V1_voltage..',V2='..V2_voltage..',nhits='..nhits..',AngoloI='..angoloInternoMax..'°,esterni='..esterni..',AngoloE='..angoloEsternoMax..'°')
      if excel_enable ~= 0 then
        excel_ws.Cells(2, V1_pos+1).Value2 = V1_voltage
        excel_ws.Cells(V2_pos+2, 1).Value2 = V2_voltage
        excel_ws.Cells(V2_pos+2, V1_pos+1).Value2 = nhits
		
        excel_ws.Cells(2+V2_count*2, V1_pos+1).Value2 = V1_voltage
        excel_ws.Cells(V2_pos+2+V2_count*2, 1).Value2 = V2_voltage
        excel_ws.Cells(V2_pos+2+V2_count*2, V1_pos+1).Value2 = esterni
		
        excel_ws.Cells(2+V2_count*4, V1_pos+1).Value2 = V1_voltage
        excel_ws.Cells(V2_pos+2+V2_count*4, 1).Value2 = V2_voltage
        excel_ws.Cells(V2_pos+2+V2_count*4, V1_pos+1).Value2 = angoloInternoMax

        excel_ws.Cells(2+V2_count*6, V1_pos+1).Value2 = V1_voltage
        excel_ws.Cells(V2_pos+2+V2_count*6, 1).Value2 = V2_voltage
        excel_ws.Cells(V2_pos+2+V2_count*6, V1_pos+1).Value2 = angoloEsternoMax

        excel_ws.Cells(2+V2_count*8, V1_pos+1).Value2 = V1_voltage
        excel_ws.Cells(V2_pos+2+V2_count*8, 1).Value2 = V2_voltage
        excel_ws.Cells(V2_pos+2+V2_count*8, V1_pos+1).Value2 = aperturaMax
		
      end
	


    -- Print image of screen to file.
    simion.printer.type = 'png'  -- 'bmp', 'png', 'jpg'
    simion.printer.filename = 'output/result_' .. V1_voltage .. ", " .. V2_voltage .. '.png'
    simion.printer.scale = 1
    simion.print_screen()
    -- caution: print_screen redraws the screen.  If ion trajectories
    -- are unsaved (i.e. e.g. sim_rerun_flym == 1), they will be lost.

    -- Write data to file.
    results_file:write(V1_voltage .. ", " .. V2_voltage .. "\n")
    results_file:flush()  -- immediately output to disk
    if html then
      html:add_image('result_' .. V1_voltage .. ", " .. V2_voltage .. '.png')
      html:add_line('result: V1='..V1_voltage..',V2='..V2_voltage..',nhits='..nhits..',AngoloI='..angoloInternoMax..'°,esterni='..esterni..',AngoloE='..angoloEsternoMax..'°'..',aperturaMax='..aperturaMax..'°')
    end	
	  
	  
    
  end

end



  

  -- Close output files.
  results_file:close()
  if html then html:close() end

  -- Show results.
  os.execute([[start notepad output\results.csv]])
  if html then
 --   print "See also output/results.html."
    os.execute([[start output\results.html]]) -- or "firefox"
      -- note: need for two "starts" may be a bug in SIMION (TO REVIEW)
  end

end

-- called exactly once on run initialization.
function segment.initialize_run()
  -- Ensure keeping ion trajectories (required for `simion.print_screen`).
  sim_rerun_flym = 0
  sim_trajectory_image_control = 0 -- keep trajectories (when rerun 0)
    first = true
  nhits = 0  -- reset for next run
  esterni=0	
 angoloInternoMax=0
 angoloI=0
 angoloEsternoMax=0
 angoloEsternoMax1=0
 angoloE=0  
 apertura=0
 aperturaMax=0
end

-- called on PA initialization to set voltages.
function segment.init_p_values()
    -- sets central electrode voltage
    adj_elect01 = 0
  adj_elect03 = V1_voltage
  adj_elect02 = 0
;    adj_elect04 = 0
  adj_elect05 = V1_voltage
  adj_elect06 = -1000
  adj_elect07 = V1_voltage
  adj_elect08 = V2_voltage



end

-- called on each particle splat in PA instance
function segment.terminate()
  -- tune only on ion #6
  if ion_number == 6 then
    -- store results
    result_y = ion_py_gu
  end
  -- Acceptance criteria
	if ion_px_gu>149.99 and ion_px_gu <150.01 then 
    nhits = nhits + 1
	AngoloI=57.2958*atan((sqrt (ion_py_gu*ion_py_gu+ion_pz_gu*ion_pz_gu))/(ion_px_gu-50))
	if aperturaMax<abs(apertura) then
		aperturaMax=abs(apertura)
	end  
	end
	if ion_px_gu>141 and ion_px_gu <314 and ion_py_gu>-81 and ion_py_gu<-79 then 
    nhits = nhits + 1
	AngoloI=57.2958*atan((sqrt (ion_py_gu*ion_py_gu+ion_pz_gu*ion_pz_gu))/(ion_px_gu-50))
	if aperturaMax<abs(apertura) then
		aperturaMax=abs(apertura)
	end  
	end  
	if ion_px_gu>303 and ion_px_gu <305 and ion_py_gu>-81 and ion_py_gu<79 then
    nhits = nhits + 1
	AngoloI=57.2958*atan((sqrt (ion_py_gu*ion_py_gu+ion_pz_gu*ion_pz_gu))/(ion_px_gu-50))
	if aperturaMax<abs(apertura) then
		aperturaMax=abs(apertura)
	end  
	end  
  
	if ion_px_gu>130 and ion_px_gu <132 and ion_py_gu>19 and ion_py_gu<91 then 
    esterni = esterni + 1
	AngoloE=57.2958*atan((sqrt (ion_py_gu*ion_py_gu+ion_pz_gu*ion_pz_gu))/(ion_px_gu-50))
 	end
	if ion_px_gu>130 and ion_px_gu <132 and ion_py_gu>-91 and ion_py_gu<-19 then 
    esterni = esterni + 1
	AngoloE1=57.2958*atan((sqrt (ion_py_gu*ion_py_gu+ion_pz_gu*ion_pz_gu))/(ion_px_gu-50))
 	end	
	if AngoloE == nil then
	AngoloE=0
	end
	if AngoloE1 == nil then
	AngoloE1=0
	end	
	if AngoloI == nil then
	AngoloI=0
	end	
	
	if angoloInternoMax<AngoloI then
	angoloInternoMax=AngoloI
	end
	if angoloEsternoMax<AngoloE then
	angoloEsternoMax=AngoloE
	end
	if angoloEsternoMax1<AngoloE1 then
	angoloEsternoMax1=AngoloE1
	end		
end
-- called exactly once on run termination.
function segment.terminate_run()
  -- simion.sleep(1) -- optional pause for easier visualization.

  sim_rerun_flym = 1 -- clears trajectories on rerun
end

function segment.other_actions()
  -- Update the PE surface display on first time-step of run.
  if first then first = false; sim_update_pe_surface = 1 end
end


function segment.initialize()
  local volts = simion.wb:epotential(ion_px_mm, ion_py_mm, ion_pz_mm)
         -- note: ion_volts not accessible from initialize segment; why?
  --local eV = V2_voltage
  --assert(eV >= 0)
  --local new_speed = ke_to_speed(eV, ion_mass)
  --local old_speed = rect3d_to_polar3d(ion_vx_mm, ion_vy_mm, ion_vz_mm)
  --assert(old_speed ~= 0)
  
  --apertura=0
  
  
  
  --ion_vx_mm = ion_vx_mm * new_speed / old_speed
  --ion_vy_mm = ion_vy_mm * new_speed / old_speed
  --ion_vz_mm = ion_vz_mm * new_speed / old_speed
end



-- optionally draw solenoid wires too
