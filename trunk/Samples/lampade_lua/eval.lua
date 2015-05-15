-- Lua evaluation script for the `lamps' sample
-- TODO copyright notice 

-- This code will be executed once when a Lua worker using this script is
-- launched. A global table named `ugp_environment' is available and contains
-- the following fields:
--   * generation: the number of the current generation
--   * tempdir: the name of a temporary directory created especially for this
--              script, in which it is possible to write temporary files.
--   * ugp_version: version number of MicroGP
--   * ugp_tagline: complete version string of MicroGP

-- This library, exclusive to LuaJIT, allows the use of C functions and types.
ffi = require("ffi")

-- We read the parameters of the problem.
local file, msg = io.open("parameters.txt", "r")
if not file then
  error("Error while opening parameters.txt: " .. msg)
end
local contents = file:read("*all")
file:close()
local radius = assert(contents:match("r=(%d+)"), "Error: radius not found")
local height = assert(contents:match("h=(%d+)"), "Error: heigth not found")
local width  = assert(contents:match("w=(%d+)"), "Error: width not found")
radius = tonumber(radius)
height = tonumber(height)
width = tonumber(width)

local max_fitness = height * width;

-- This function will be called repeatedly with the following argument:
-- individuals: a table containing all individuals of a group
function evaluate(individuals)
  local lamps = {}
  -- Parse the coordinates of the lamps
  for k,ind in pairs(individuals) do
    local x, y = ind:match("(%d+)%s+(%d+)")
    table.insert(lamps, {x=x, y=y})
  end
  -- MicroGP expects that this function evaluate the fitness of the given list
  -- of individuals, i.e. how they perform as a group.
  local total_room = new_room()
  for k, lamp in pairs(lamps) do
    light_room(total_room, lamp)
  end
  local group_coverage = coverage(total_room)
  local group_overlap = overlap(total_room)
  -- Return the fitness value(s)
  return group_coverage, 1 - group_overlap, "Cov: " .. group_coverage .. " - Ov: " .. group_overlap .. ", " .. #individuals .. " lamps"
end

-- Create a new room a C data structure
function new_room()
  return ffi.new("int32_t[?]", max_fitness, 0)
end

local min, max = math.min, math.max
function sqrt(x)
  return math.floor(math.sqrt(x) + 0.5)
end

-- Increment a circle centered on the lamp coordinates
function light_room(room, lamp)
  local x, y = lamp.x, lamp.y
  for i = max(0, x - radius), min(width-1, x + radius) do
    for j = max(0, y - sqrt(radius^2 - (i - x)^2)),min(height-1, y + sqrt(radius^2 - (i - x)^2)) do
      room[j * width + i] = room[j * width + i] + 1
    end
  end
end

function sum_rooms(rooms)
  result = rooms[1]
  if #rooms > 1 then
    for k=2,#rooms do
      for i=0,width-1 do
        for j=0,height-1 do
          local index = j*width + i
          result[index] = result[index] + rooms[k][index]
        end
      end
    end
  end
  return result
end

function count_brighter_than(room, level)
  -- Determine room illumination at or above the given level
  local illuminated = 0
  for i=0,width-1 do
    for j=0,height-1 do
      if room[j*width + i] >= level then
        illuminated = illuminated + 1
      end
    end
  end
  return illuminated
end

function coverage(room)
  return count_brighter_than(room, 1) / max_fitness
end

function overlap(room)
  return count_brighter_than(room, 2) / max_fitness
end

if not ugp_environment then
  -- Test to see if the funtion works, only when executed outside MicroGP
  print("Lua evaluator test: room lit by a lamp in the middle")
  local room = new_room()
  light_room(room, {x=math.floor(height/2), y=math.floor(width/2)})
  for i=0,width-1 do
    for j=0,height-1 do
      io.write(room[j*width + i], " ")
    end
    io.write("\n")
  end
  print("Lua evaluator test: coverage of a single lamp in the middle")
  print(evaluate({math.floor(height/2) .. " " .. math.floor(width/2)}))
  print("Lua evaluator test: coverage of a two overlapping lamps")
  print(evaluate({math.floor(height/2) .. " " .. (math.floor(width/2 - radius/2)),
                  math.floor(height/2) .. " " .. (math.floor(width/2 + radius/2))}))
end

