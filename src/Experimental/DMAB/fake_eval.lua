-- Fake evaluator which considers the lineage rather than the actual DNA to evaluate individuals.
-- Used for testing operator selection strategies

-- Group evolution semantics
function evaluate(individuals, lineages)
  -- lineages.group is the lineage of the whole group

  --[[
  -- We can know when a group is in fact a temporary singleton for the
  -- evaluation of one individual because in that case its id begins with "TMP_"
  --]]
  if lineages.group.id:sub(1, 4) == "TMP_" then
    -- only care about the first individual (i.e. lineages[1])
    --
    for _, name in ipairs(lineages[1].parentNames) do
      if lineages[1].operatorName == "singleParameterAlterationMutation" and name == "A" then
        return 55, "mutated son of A"
      end
    end
  end

  -- For real groups:
  for i, lineage in ipairs(lineages) do
    -- lineage is that of the i-th individual
    if lineage.operatorName == "twoPointCrossover" then
      --[[
      -- if the group contains any individual made by twoPointCrossover,
      -- we return a fitness of 1 for the whole group.
      --]]
      return 1, "contains an individual from twoPointCrossover"
    end
    if #lineage.parents >= 1 and lineage.parents[1].operatorName == "twoPointCrossover" then
      --[[
      -- if the group contains any individual which first parent was
      -- made by twoPointCrossover, we return a fitness of 0.5 for the
      -- whole group.
      --]]
      return 0.5, "contains a son of an ind from 2PX"
    end
  end

  --[[
  -- we return a fitness of 12 if the group itself has been
  -- created by groupDreamTeam
  --]]
  if lineages.group.operatorName == "groupDreamTeam" then
    return 12, "produced by groupDreamTeam"
  end
end

-- Individual evolution semantics
function evaluate(individuals, lineages)
  -- the individual to evaluate is individuals[1] and has lineage lineages[1]
end


