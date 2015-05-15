#!/usr/bin/env luajit
-- 
-- Wrapper for Lua evaluators that makes Lua evaluation available to old
-- versions of MicroGP (group evaluation only). Thus all versions of MicroGP in
-- the benchmark will get the same fitness values.
--

if arg and arg[1] then
  -- The script is run by an old version of MicroGP.
  --
  -- We emulate newer MicroGP Lua evaluation, by reading the individual files
  -- from and calling the Lua evaluation function and writing the results to
  -- the disk.
  --
  _G['ugp_environment'] = {
    ugp_version = os.getenv("UGP3_VERSION"),
    ugp_tagline = os.getenv("UGP3_TAGLINE"),
    tempdir = '.',
    generation = os.getenv("UGP3_GENERATION"),
    stop = false
  }
end

-- Write here the name of the script that contains the evaluate() function
require "eval"

-- The rest of this file should be independent from the configuration of the
-- sample, do not modify it.
assert(evaluate, "Lua compatibility evaluator: error: evaluate() function not found.")

if arg and arg[1] then
  local fitness_file, msg = io.open(os.getenv("UGP3_FITNESS_FILE"), "w")
  if not fitness_file then
    error("Error while opening fitness file for writing: " .. msg)
  end

  function write_fitness(fitness)
    -- Write the fitness to the output file
    for i, val in ipairs(fitness) do
      if i == #fitness and type(val) == "string" then
        -- If the last fitness component is a string, we consider it is the
        -- description. We ditch it.
      else
        fitness_file:write(val, " ")
      end
    end
  end

  for group in string.gmatch(os.getenv("UGP3_OFFSPRING"), "%S+") do
    local groupfile, msg = io.open(group, "r")
    if not groupfile then
      error("Error while opening group file: " .. msg)
    end
    local individuals = {}
    for indfilename in groupfile:lines() do
      local indfile, msg = io.open(indfilename, "r")
      if not indfile then
        error("Error while opening individual file: " .. msg)
      end
      table.insert(individuals, indfile:read("*all"))
      indfile:close()
    end
    groupfile:close()
    local group_fitness = {evaluate(individuals)}
    write_fitness(group_fitness)
    for k, individual in ipairs(individuals) do
      local val = {evaluate({individual})}
      write_fitness(val)
    end
    fitness_file:write("\n")
  end
  if ugp_environment.stop then
    fitness_file:write("#stop\n")
  end
  fitness_file:close()
end
