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
-- $Revision: 161 $
-- $Date: 2012-01-30 14:11:26 +0100 (lun, 30 gen 2012) $
--------------------------------------------------------------------------


function evaluate(individuals)
  local _, count = string.gsub(individuals[1], "1", "1");
  return count
end

