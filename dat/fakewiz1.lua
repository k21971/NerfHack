-- NetHack yendor fakewiz1.lua	$NHDT-Date: 1652196025 2022/05/10 15:20:25 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.2 $
--	Copyright (c) 1989 by Jean-Christophe Collet
--	Copyright (c) 1992 by M. Stephenson and Izchak Miller
-- NetHack may be freely redistributed.  See license for details.
--
des.level_init({ style="mazegrid", bg ="-" });

des.level_flags("mazelevel", "noflip");

local tmpbounds = selection.match("-");
local bnds = tmpbounds:bounds();
local bounds2 = selection.fillrect(bnds.lx, bnds.ly + 1, bnds.hx - 2, bnds.hy - 1);

local fakewiz1 = des.map({ halign = "center", valign = "center", map = [[
.........
.}}}}}}}.
.}}---}}.
.}--.--}.
.}|...|}.
.}--.--}.
.}}---}}.
.}}}}}}}.
.........
]], contents = function(rm)
   des.levregion({ region={01,00,79,20}, region_islev=1, exclude={0,0,8,8}, type="stair-up" })
   des.levregion({ region={01,00,79,20}, region_islev=1, exclude={0,0,8,8}, type="stair-down" })
   des.levregion({ region={01,00,79,20}, region_islev=1, exclude={0,0,8,8}, type="branch" });
   des.teleport_region({ region={01,00,79,20}, region_islev=1,exclude={2,2,6,6} })
   des.levregion({ region={4,4,4,4}, type="portal", name="wizard7" })
   des.mazewalk(08,05,"east")
   des.region({ region={04,03,06,06},lit=0,type="ordinary",irregular=1,arrival_room=true })
   des.monster("L",04,04)
   des.monster("vampire lord",03,04)
   des.monster("kraken",06,06)
   -- And to make things a little harder.
   des.trap("board",04,03)
   des.trap("board",04,05)
   des.trap("board",03,04)
   des.trap("board",05,04)
end
});

local protected = bounds2:negate() | fakewiz1;
hell_tweaks(protected);
