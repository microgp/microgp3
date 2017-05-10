-- PERL evaluator for the string coverage problem
-- Alberto Tonda, 2012 <alberto.tonda@gmail.com>
--
-- Translated to Lua in september 2014 by Jany Belluz

-- Load the test set
local file, msg = io.open("testSet.txt", "r")
if not file then
  error("Error while opening testSet.txt: " .. msg)
end
local test_set = {}
for line in file:lines() do
  if line ~= "" then
    table.insert(test_set, line)
  end
end
file:close()

function evaluate(individuals, fitness)
  local global_fitness = 0
  local global_covered = {}
  for i, template in pairs(individuals) do
    -- Turn the template into a Lua pattern and count the stars
    template = string.gsub(template, "%s", "")
    local pattern, number_of_stars = string.gsub(template, "%*", "[01]")
    local fitness_ratio_ones = (#template - number_of_stars) / #template
    -- Matching
    local match_count = 0
    for j, test_string in pairs(test_set) do
      if string.match(test_string, pattern) == test_string then
        match_count = match_count + 1
        global_covered[test_string] = true
      end
    end
    local fitness_matching = match_count / #test_set
    local fitness_individual = fitness_matching * fitness_ratio_ones
    if (fitness) then
      fitness[i] = fitness_individual
    end
    global_fitness = global_fitness + fitness_individual
  end
  -- Count covered test strings
  local nb_covered = 0
  for _, _ in pairs(global_covered) do
    nb_covered = nb_covered + 1
  end
  if (fitness) then
    -- We return a custom measure of the group fitness
    return global_fitness / #individuals * nb_covered / #test_set,
      nb_covered .. "/" .. #test_set .." global_fitness=" .. global_fitness .. "/".. #individuals
  else
    -- We only return the coverage
    return nb_covered / #test_set, nb_covered .. "/" .. #test_set
  end
end

-- Test
if not ugp_environment then
  local _test_set = test_set
  test_set = {
    "0000",
    "1000",
    "1100",
    "1110",
    "1111",
  }
  assert(evaluate({"1111"}) == 0.2)
  assert(evaluate({"111*"}) == 0.4)
  assert(evaluate({"11**"}) == 0.6)
  assert(evaluate({"1***"}) == 0.8)
  assert(evaluate({"****"}) == 1)
  test_set = {
    "1101",
    "1100",
    "1110",
    "0011",
    "0111",
    "1011",
  }
  assert(evaluate({"11**", "**11"}) == 1)
  print("Lua evaluator tests: OK")
  test_set = _test_set
end

