-- NetHack sokoban soko1-2.lua	$NHDT-Date: 1652196034 2022/05/10 15:20:34 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.6 $
--	Copyright (c) 1998-1999 by Kevin Hugo
-- NetHack may be freely redistributed.  See license for details.
--
-- "J Franklin Mentzer <wryter@aol.com>"
-- https://nethackwiki.com/wiki/Sokoban_Level_4c
-- Ported from SLASH'EM
-- Converted to lua by hackemslashem
-- NerfHack enlarged the zoo slightly
--
des.level_init({ style = "solidfill", fg = " " });
des.level_flags("mazelevel", "noteleport", "premapped", "sokoban", "solidify", "cold");
des.map([[
            --------- 
            |.......| 
 -------    |..----+--
 |.....-------.|.....|
-----........F.|.....|
|...|-....--.|.|.....|
|...|..--....F.|.....|
|...|..-----.|.|.....|
--.........|.F.|...|-|
 |.....---.F.|.|...+.|
 |.|.....|.|.F.|...|-|
 |.|.|.|...F.|.|...+.|
--.---...|.|.F.|...|-|
|.....----.F.|.|...+.|
|..............|------
----....|......|      
   |....|..-----      
   ---------          
]]);

des.stair("down", 02,03);
des.region(selection.area(00,00,21,17),"lit");
des.non_diggable(selection.area(00,00,21,17));
des.non_passwall(selection.area(00,00,21,17));

-- Doors
des.door("locked",19,02)
des.door("locked",19,09)
des.door("locked",19,11)
des.door("locked",19,13)

-- Boulders
des.object("boulder",06,04);
des.object("boulder",02,06);
des.object("boulder",02,07);
des.object("boulder",03,08);
des.object("boulder",06,08);
des.object("boulder",03,09);
des.object("boulder",05,09);
des.object("boulder",06,10);
des.object("boulder",06,11);
des.object("boulder",03,13);
des.object("boulder",05,14);
des.object("boulder",10,14);
des.object("boulder",04,15);
des.object("boulder",07,15);
des.object("boulder",10,15);
des.object("boulder",12,05);

-- Traps
-- prevent monster generation over the (filled) holes
des.exclusion({ type = "monster-generation", region = { 15,01, 18,01 } });
des.trap("hole",15,01)
des.trap("hole",16,01)
des.trap("hole",17,01)
des.trap("hole",18,01)
des.exclusion({ type = "monster-generation", region = { 14,03, 14,13 } });
des.trap("hole",14,03)
des.trap("hole",14,04)
des.trap("hole",14,05)
des.trap("hole",14,06)
des.trap("hole",14,07)
des.trap("hole",14,08)
des.trap("hole",14,09)
des.trap("hole",14,10)
des.trap("hole",14,11)
des.trap("hole",14,12)
des.trap("hole",14,13)

des.monster({ id = "giant mimic", appear_as = "obj:boulder" });
des.monster({ id = "giant mimic", appear_as = "obj:boulder" });

-- Random objects
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "%" });
des.object({ class = "=" });
des.object({ class = "/" });

des.region({ region={16,03, 13,07}, lit = 1, type = "zoo", filled = 1, irregular = 1 });
--des.region({ region={16,08, 15,07}, lit = 1, type = "zoo", filled = 1, irregular = 1 });

-- Ice must be created after the zoo, otherwise it interferes with monster creation.
des.replace_terrain({ region={0,0, 75,08}, fromterrain=".", toterrain="I", chance=15 })
des.replace_terrain({ region={0,9, 75,08}, fromterrain=".", toterrain="I", chance=15 })

-- Rewards

local place = selection.new();
place:set(20,09);
place:set(20,11);
place:set(20,13);

local pt = selection.rndcoord(place);
local prizes = { { id = "bag of holding" },
                 { id = "amulet of reflection"},
                 { id = "cloak of magic resistance"},
                 { id = "magic marker"} }
shuffle(prizes)
des.object({ id = prizes[1].id, coord=pt, buc="not-cursed", achievement=1 });
des.engraving({ coord = pt, type = "burn", text = "Elbereth" });
des.object({ id = "scroll of scare monster", coord = pt, buc = "cursed" });

-- Ruling steward of Sokoban
des.monster({ id = "Wintercloak", x = 19, y = 3, waiting = 1 });