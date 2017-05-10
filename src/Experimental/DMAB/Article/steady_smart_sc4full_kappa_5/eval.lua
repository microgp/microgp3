-- PERL evaluator for the string coverage problem
-- Alberto Tonda, 2012 <alberto.tonda@gmail.com>
--
-- Translated to Lua in september 2014 by Jany Belluz

-- The original paper "A Hierarchical Cooperative Evolutionary Algorithm"
-- by Wu and Banzhaf feature a multiplicative factor for individual fitness
-- that depends on the test set.
local file, msg = io.open("params.txt", "r")
if not file then
  error("Error while opening params.txt: " .. msg)
end
local individual_factor = string.match(file:read("*all"), "individual_factor=(%d+)")
assert(individual_factor, "params.txt does not contain the line 'individual_factor=...'")

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
local pattern_length = #(test_set[1])

function evaluate(individuals)
  local global_fitness = 0
  local global_covered = {}
  for j = 1, #test_set do
    global_covered[j] = 0
  end
  local global_nb_stars = 0
  for i, template in pairs(individuals) do
    -- Turn the template into a Lua pattern and count the stars
    template = string.gsub(template, "%s", "")
    local pattern, number_of_stars = string.gsub(template, "%*", ".")
    local fitness_ratio_ones = (#template - number_of_stars) / #template
    global_nb_stars = global_nb_stars + number_of_stars
    -- Matching
    local match_count = 0
    for j, test_string in pairs(test_set) do
      if test_string:match(pattern) then
        match_count = match_count + 1
        global_covered[j] = global_covered[j] + 1
      end
    end
    local fitness_matching = match_count / #test_set
    local fitness_individual = individual_factor * fitness_matching * fitness_ratio_ones
    global_fitness = global_fitness + fitness_individual
  end
  -- Count covered test strings and overlap
  local nb_covered = 0
  local nb_overlap = 0
  for _, count in pairs(global_covered) do
    if count > 0 then
      nb_covered = nb_covered + 1
    end
    if count > 1 then
      nb_overlap = nb_overlap + 1
    end
  end
  -- Return a compromise between coverage and number of ones
  local avg_nb_stars = global_nb_stars / #individuals
  local fitness = ((#test_set - nb_overlap) / #test_set) * (nb_covered / #test_set) * (global_fitness / #individuals)
  return fitness, ("cov. %d/%d, ov. %d/%d, #ind %d, avg. #* %d"):format(nb_covered, #test_set, nb_overlap, #test_set, #individuals, avg_nb_stars)
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
  print('Scale test set: evaluate({"1111"}) = ', evaluate({"1111"}))
  print('Scale test set: evaluate({"111*"}) = ', evaluate({"111*"}))
  print('Scale test set: evaluate({"11**"}) = ', evaluate({"11**"}))
  print('Scale test set: evaluate({"1***"}) = ', evaluate({"1***"}))
  print('Scale test set: evaluate({"****"}) = ', evaluate({"****"}))
  test_set = {
    "1101",
    "1100",
    "1110",
    "0011",
    "0111",
    "1011",
  }
  print('Split test set: evaluate({"11**", "**11"}) = ', evaluate({"11**", "**11"}))
  test_set = _test_set
end

