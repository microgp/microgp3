-- Lua evaluation script for the `lamps' sample
------------------------------------------------
--
--
--
-- This version is meant to be fast and simple,
-- to be used with Valgrind. *GIVES BAD RESULTS*
-- (Manhattan distance lamps, subsampling).
--
--
--

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

local maxFitness = height * width;

function evaluate(individuals, fitness)
  local lamps = {}
  -- Parse the coordinates of the lamps
  for k,ind in pairs(individuals) do
    local x, y = ind:match("(%d+)%s+(%d+)")
    table.insert(lamps, {x=x, y=y})
  end
  if fitness then
    for k, lamp in pairs(lamps) do
      fitness[k] = coverage({lamp})
    end
  end
  -- Determine room illumination
  illuminated = coverage(lamps)
  -- Return the fitness value(s)
  return illuminated/maxFitness, "Coverage: " .. illuminated/maxFitness
end

function coverage(lamps)
  local illuminated = 0
  for i=1,height,5 do
    for j=1,width,5 do
      for k,l in pairs(lamps) do
        if math.abs(i - l.x) + math.abs(j - l.y) <= radius then
          illuminated = illuminated + 25
          break
        end
      end
    end
  end
  return illuminated;
end

-- Test to see if the funtion works
print("Lua evaluator test: coverage of a single lamp in the middle")
print(evaluate({math.floor(height/2) .. " " .. math.floor(width/2)}))
