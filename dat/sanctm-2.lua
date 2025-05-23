-- NetHack gehennom sanctum.lua	$NHDT-Date: 1652196034 2022/05/10 15:20:34 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.5 $
--	Copyright (c) 1989 by Jean-Christophe Collet
--	Copyright (c) 1992 by M. Stephenson and Izchak Miller
-- NetHack may be freely redistributed.  See license for details.
--
-- Ported from UnNetHack and EvilHack
-- Converted to lua by hackemslashem
--
des.level_init({ style="mines", fg=".", bg="L", smoothed=true, joined=true, walled=true })

des.level_flags("mazelevel", "noteleport", "hardfloor", "nommap")
-- This is outside the main map, below, so we must do it before adding
-- that map and anchoring coordinates to it. This extends the invisible
-- barrier up to the top row, which falls outside the drawn map.
des.non_passwall(selection.area(39,00,41,00))

--0       1         2         3         4         5         6         7     
--23456789012345678901234567890123456789012345678901234567890123456789012345
des.map([[
LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
LLLLLLLLLLLLLL----F----F----LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
LLLLLLLLLLLLLL|............|LLLLLLLLLLLLL-------LLLLLLLLLLLLLLLLLLLLLLLLLLLL
LLLLLLLL---F---............---F-LLLLLLLLLF.....FLLLLLLLLLLLLLLLLLLLLLLLLLLLL
LLLLLLLL|......................|LLLLLLLL--.....|LLLLLLLLLLLL--F---F--LLLLLLL
LLLLL-F--......................-F-----LL|......----LLLLLLLLL|.......|LLLLLLL
LLLLL|........---------..........|...|LL|.........|--F----F---+---..FLLLLLLL
LLL---........|.......|..........--S--LL|.........|.....S........|..|LLLLLLL
LLLF..........|.......|.............|LLLF.........---S---..-----S----LLLLLLL
LLL|..........|.......|..........----LLL|..........|....|..|......|LLLLLLLLL
LLL|..........|.......|..........FLLLLLL--.......----+---S---S--..FLLLLLLLLL
LLLF..........---------..........|LLLLLLLF.......|.............|..|LLLLLLLLL
LLL---...........................|LLLLLLL-----+-------S---------S---LLLLLLLL
LLLLL|...........................FLLLLLLLLLL|...F.F......|....|....|--LLLLLL
LLLLL-F--.....................----LLLLLLLLLLF...-F-....------S-......FLLLLLL
LLLLLLLL|.....................|LLLLLLLLLLLLL|..........|LLLL|.....----LLLLLL
LLLLLLLL---F---...........---F-LLLLLLLLLLLLL--...----F--LLLLF.....|LLLLLLLLL
LLLLLLLLLLLLLL|...........|LLLLLLLLLLLLLLLLLLF...FLLLLLLLLLL|.....FLLLLLLLLL
LLLLLLLLLLLLLL----F----F---LLLLLLLLLLLLLLLLLL-----LLLLLLLLLL-------LLLLLLLLL
LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
]]);
des.region({ region={15,07, 21,10}, lit=1, type="temple", filled=2, contents = function()
                des.door({ wall = "random", state = "secret" });
end })
des.altar({ x=18, y=08, align="noalign", type="sanctum" })
des.region({ region={41,06, 48,11}, lit=0, type="morgue", filled=1, irregular=1 })
-- Non diggable walls
des.non_diggable(selection.area(00,00,75,19))
-- Invisible barrier separating the left & right halves of the level
des.non_passwall(selection.area(37,00,39,19))

-- Doors
--des.door("closed",40,06)
des.door("locked",62,06)
des.door("closed",46,12)
des.door("closed",53,10)

-- Drawbridges
des.drawbridge({ dir="west", state="closed", x=38,y=06})
des.drawbridge({ dir="east", state="closed", x=39,y=06})

-- Surround the temple with fire
des.trap("fire",13,05)
des.trap("fire",14,05)
des.trap("fire",15,05)
des.trap("fire",16,05)
des.trap("fire",17,05)
des.trap("fire",18,05)
des.trap("fire",19,05)
des.trap("fire",20,05)
des.trap("fire",21,05)
des.trap("fire",22,05)
des.trap("fire",23,05)
des.trap("fire",13,12)
des.trap("fire",14,12)
des.trap("fire",15,12)
des.trap("fire",16,12)
des.trap("fire",17,12)
des.trap("fire",18,12)
des.trap("fire",19,12)
des.trap("fire",20,12)
des.trap("fire",21,12)
des.trap("fire",22,12)
des.trap("fire",23,12)
des.trap("fire",13,06)
des.trap("fire",13,07)
des.trap("fire",13,08)
des.trap("fire",13,09)
des.trap("fire",13,10)
des.trap("fire",13,11)
des.trap("fire",23,06)
des.trap("fire",23,07)
des.trap("fire",23,08)
des.trap("fire",23,09)
des.trap("fire",23,10)
des.trap("fire",23,11)
-- Some traps.
des.trap("spiked pit")
des.trap("fire")
des.trap("sleep gas")
des.trap("anti magic")
des.trap("fire")
des.trap("magic")
-- Some random objects
des.object("[")
des.object("[")
des.object("[")
des.object("[")
des.object(")")
des.object(")")
des.object("*")
des.object("!")
des.object("!")
des.object("!")
des.object("!")
des.object("?")
des.object("?")
des.object("?")
des.object("?")
des.object("?")

-- Chest by the altar
des.object({ id = "chest", trapped = 0, locked = 1, x = 17, y = 09 ,
             contents = function()
                -- This is converted into a zappable scroll of wishing
                des.object("wishing");
             end
});

-- Some monsters.
des.monster({ id = "horned devil", x=14,y=12,peaceful=0 })
des.monster({ id = "barbed devil", x=18,y=08,peaceful=0 })
des.monster({ id = "erinys", x=10,y=04,peaceful=0 })
des.monster({ id = "marilith", x=07,y=09,peaceful=0 })
des.monster({ id = "nalfeshnee", x=27,y=08,peaceful=0 })

-- Moloch's horde
des.monster({ id = "aligned cleric", x=20,y=03,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=15,y=04,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=11,y=05,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=11,y=07,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=11,y=09,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=11,y=12,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=15,y=13,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=17,y=13,align="noalign",peaceful=0 })
des.monster({ id = "aligned cleric", x=21,y=13,align="noalign",peaceful=0 })

-- A few nasties
des.monster("L")
des.monster("L")
des.monster("V")
des.monster("V")
des.monster("V")
des.stair("up", 63,15)
-- Teleporting to this level is allowed after the invocation creates its
-- entrance.  Force arrival in that case to be on rightmost third of level.
des.teleport_region({ region = {54,1,79,18}, region_islev=1, dir="down" })

-- Cthulu is placed right in the middle
des.monster({ id = "Cthulhu", x=45,y=14,peaceful=0 })
