-- === GENERATORE DI N FILE GEM ===

-- Quanti file generare?
local N = 3        -- cambia quanto vuoi
local step = 10     -- shift tra un file e l’altro

local function generate_gem(filename, x_shift)
  -- Calcolo coordinate
  local x1 = 325 + x_shift
  local x2 = 330 + x_shift
  local x3 = 430 + x_shift

  local f = assert(io.open(filename, "w"))

  f:write(string.format([[

pa_define(950,2100,1,cylindrical,y_mirror,E)

; --- ELETTRODI FISSI ---
e(1) { fill{ within{box(10,0,230,100)} } }
e(2) { fill{ within{box(10,100,210,300)} } }
e(3) { fill{ within{box(10,300,210,800)} } }
e(4) { fill{ within{box(10,800,110,2000)} } }

; --- ELETTRODI SHIFTATI ---
e(5) { fill{ within{box(%d,20,%d,300)} } }
e(6) { fill{ within{box(%d,200,%d,2000)} } }

; --- ALTRI ELETTRODI ---
e(7) { fill{ within{box(630,0,730,2000)} } }
e(8) {
  fill{
    within{box(0,0,950,2100)}
    notin{box(2,0,947,2097)}
  }
}
]], 
  x1, x2,     -- per e(5)
  x2, x3      -- per e(6)
))

  f:close()
end

-- Generatore ciclico
for i = 0, N-1 do
  local shift = i * step
  local filename = string.format("tune_shift_%d.gem", shift)
  generate_gem(filename, shift)
end

