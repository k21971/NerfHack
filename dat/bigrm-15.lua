-- NetHack bigroom bigrm-15.lua	$NHDT-Date: 1652196021 2022/05/10 15:20:21 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.3 $
--	Copyright (c) 1989 by Jean-Christophe Collet
--	Copyright (c) 1990 by M. Stephenson
-- NetHack may be freely redistributed.  See license for details.
--
-- The Mandelbrot
-- Ported from UnNetHack
-- Converted to lua by hackemslashem
--
des.level_init({ style = "solidfill", fg = " " });
des.level_flags("mazelevel", "noflip");

des.map([[
                      LLLLLLLLLLLLuuuuuLLuuuIIIIuuLuuuuLLLLLL              
                     LLLLLLLLLLLLuuuuLLLLuuI.,,..,..uLLuuuuuLLLL           
                  LLLLLLLLLLuuuuuuLuuuuuuuuI,,.,,.,IuuuLLLLLuuLLLL         
              LLLLLLLLLuuuuuuuuLLuu,,.,,.,.,,......,,..uuuI.uLuLLLL        
          LLLLLLLLuuuuuuuuuuLLLLuuuI,,...............,,,,,..uLuuLLLL       
      LLLLLLLLLuuLuLLLLLLLLLuuuuu,,,.....................,,IuuLuuLLLL      
   LLLLLLLLLuuuuLuIIuuuu,Iuuuuuu,,.........................,,,.uuuLLLL     
 LLLLLLLLuuuuuLLuuuI,,,.,,.,.II.,..........................,,uuLuLLLLL     
LLLLLLuuuuuLLLuuuu,,........,,,,...........................,,uLuuLLLLL     
LuuuuLLLLLuuuuu,,,.......................................,,,uuLuuLLLLL     
u,,,,,.......,,,.........................................,IuuLLuuLLLLL     
LuuuuLLLLLuuuuu,,,.......................................,,,uuLuuLLLLL     
LLLLLLuuuuuLLLuuuu,,,.......,,,............................,,uLuuLLLLL     
 LLLLLLLLuuuuuLLuuuI.,,,.,,,.II,...........................,,uuLuLLLLL     
   LLLLLLLLLuuuuLuIIuuuu,Iuuuuuu,.........................,,,,uuuLLLL      
      LLLLLLLLLuuLuLLLLLLLLLuuuuu,,,......................,IuuLuuLLLL      
          LLLLLLLLuuuuuuuuuuLLLLuuuI,.................,,,,,.uLuuLLLL       
              LLLLLLLLLuuuuuuuuLLuu.,,,,,,,.,,....,,,,.uuuI.uLuLLLL        
                  LLLLLLLLLLuuuuuuLuuuuuuuuI,...,,.IuuuLLLLLuuLLLL         
                     LLLLLLLLLLLLuuuuLLLLuuI,,,,,,,,uLLuuuuuLLLL           
                        LLLLLLLLLLLLuuuuuLLuuuIIIIuuLuuuuLLLLLL            
]]);

des.region(selection.area(01,01, 74, 21), "lit");

des.stair("up");
des.stair("down");

des.non_diggable();

for i = 1,10 do
   des.object();
end

des.object("wax candle")
des.object("wax candle")

for i = 1,6 do
   des.trap();
end

for i = 1,28 do
  des.monster();
end
