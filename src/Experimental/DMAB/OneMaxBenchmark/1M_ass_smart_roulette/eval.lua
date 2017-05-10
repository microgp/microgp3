--------------------------------------------------------------------------
--                                                                       -
-- This file is part of MicroGP v3 (ugp3)                                -
-- http://ugp3.sourceforge.net/                                          -
--                                                                       -
--------------------------------------------------------------------------
--                                                                       -
-- This program is free software; you can redistribute it and/or modify  -
-- it under the terms of the GNU General Public License as published by  -
-- the Free Software Foundation, either version 3 of the License, or (at -
-- your option) any later version.                                       -
--                                                                       -
-- This program is distributed in the hope that it will be useful, but   -
-- WITHOUT ANY WARRANTY; without even the implied warranty of            -
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      -
-- General Public License for more details                               -
--                                                                       -
--------------------------------------------------------------------------
-- $Revision: 159 $
-- $Date: 2012-01-30 13:09:37 +0100 (lun, 30 gen 2012) $
--------------------------------------------------------------------------

-- We should get a temporary directory from MicroGP
tempdir = assert(ugp_environment.tempdir, "Lua evaluator: no temporary directory.")

-- Compile main.c
local fmt = string.format;
local ret = os.execute(fmt("gcc -O99 -c main.c -o %s/main.o", tempdir))
if ret ~= 0 then
    error("Lua evaluator: compilation error in main.c.")
end

function evaluate(individuals)
  -- Clean up directory
  os.remove(tempdir .. "/tprog")
  os.remove(tempdir .. "/error.log")
  -- Compile
  local file = io.open(tempdir .. "/code.S", "w")
  assert(file, "Lua evaluator: could not write assembly code.")
  file:write(individuals[1])
  file:close()
  local ret = os.execute(fmt("gcc -o %s/tprog %s/main.o %s/code.S -lm 2>%s/error.log", tempdir, tempdir, tempdir, tempdir))
  if ret ~= 0 then
    local error_log = "No error log."
    local file = io.open(tempdir .. "/error.log")
    if file then
      error_log = file:read("*all")
      file:close()
    end
    error("Lua evaluator: compilation error.\n" .. error_log)
  end
  -- Execute
  local out = io.popen(tempdir .. "/tprog")
  assert(out, "Lua evaluator: unable to execute program")
  local fitness, output = string.match(out:read("*all"), "(%d+) (%x*)")
  out:close()
  -- Get number of lines
  local _, num_lines = string.gsub(individuals[1], "\n", "\n")
  fitness_lines = math.max(0, 10000 - num_lines)

  return fitness, fitness_lines, fmt("0x%s/%d", output, num_lines)
end

