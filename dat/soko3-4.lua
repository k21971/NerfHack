-- NetHack sokoban soko2-2.lua	$NHDT-Date: 1652196035 2022/05/10 15:20:35 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.1 $
--	Copyright (c) 1998-1999 by Kevin Hugo
-- NetHack may be freely redistributed.  See license for details.
--
-- SLASH'EM 3d
-- https://nethackwiki.com/wiki/Sokoban_Level_3d
-- "Joseph L Traub"
-- Ported from SLASH'EM
-- Converted to lua by hackemslashem
--
des.level_init({ style = "solidfill", fg = " " });
des.level_flags("mazelevel", "noteleport", "premapped", "sokoban", "solidify", "cold");
des.map([[
         ----------
    -----|........|
 ----...-|.-FFF-..|
 |.......---...|.--
 |.............|.| 
 |.|.--------.-|.| 
 |.|.....|.....|.| 
 |.|.....|.|...|.| 
--.|...|.|.-FFF|.| 
|..--....|.....|.| 
|......|.|.......| 
-----.--.........| 
   |.............| 
   |...---.......| 
   ----- --------- 
]]);

des.stair("down", 04,04)
des.stair("up", 10,02)
des.region(selection.area(00,00,18,14), "lit");
des.non_diggable(selection.area(00,00,18,14));
des.non_passwall(selection.area(00,00,18,14));

-- Ice
des.replace_terrain({ region={0,0, 75,19}, fromterrain=".", toterrain="I", chance=15 })

-- Boulders
des.object("boulder",04,03)
des.object("boulder",03,04)
des.object("boulder",05,04)
des.object("boulder",07,04)
des.object("boulder",12,04)
des.object("boulder",04,05)
des.object("boulder",06,06)
des.object("boulder",05,07)
des.object("boulder",07,07)
des.object("boulder",06,08)
des.object("boulder",05,10)
des.object("boulder",05,12)
des.object("boulder",13,05)

-- Traps
-- prevent monster generation over the (filled) holes
des.exclusion({ type = "monster-generation", region = { 11,01, 15,01 } });
des.trap("hole",11,01)
des.trap("hole",12,01)
des.trap("hole",13,01)
des.trap("hole",14,01)
des.trap("hole",15,01)
des.exclusion({ type = "monster-generation", region = { 16,03, 16,09 } });
des.trap("hole",16,03)
des.trap("hole",16,04)
des.trap("hole",16,05)
des.trap("hole",16,06)
des.trap("hole",16,07)
des.trap("hole",16,08)
des.trap("hole",16,09)

-- Random objects
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "=" });
des.object({ class = "/" });

-- Two random mimics!
des.monster("m")
des.monster("m")