-- NetHack Caveman Cav-strt.lua	$NHDT-Date: 1652196002 2022/05/10 15:20:02 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.3 $
--	Copyright (c) 1989 by Jean-Christophe Collet
--	Copyright (c) 1991 by M. Stephenson
-- NetHack may be freely redistributed.  See license for details.
--
--
--    The "start" level for the quest.
--
--    Here you meet your (besieged) class leader, Shaman Karnov
--    and receive your quest assignment.
--
des.level_init({ style = "solidfill", fg = " " });

des.level_flags("mazelevel", "noteleport", "hardfloor", "noflip")

des.map([[
                                                                            
  ......     ..........................       ... }}     ....  ......       
 ......       ..........................     ........     } .... }} .....} 
  ..BB      .............................   }.........}    }}}}}}}....  ..  
     ..    ......................             }.......}  }} .. }}}}....} .. 
     ..    BB...................         }}          ..  .......}  }..} ... 
   ..    ...B      S   BB                ..... }   .......   ....  }}} .... 
    ..}} .    ...  .   ..               ........} ..     ..   ..   }   ...  
     ..  .}  .......    ..            }............       ..       }} ...   
  }}}  .      ....     } ..}}  }      } ........  }        ..  ...........  
  ...}} ..     ..      }}.............}  }}  }}  }} }   ..........}........ 
 .....  }.....          }}...............................      ...........  
} .}}.BB................ }}}}}}}}   ... }}} }}}}}}  } }}        }}}   ...   
  .....  }} .  ..........    }}} ....}.}} }}}...  .......... }}}}}    }...  
  }...}    ..          .............  ..    ...................  }}}}}}.... 
          BB       ..   .........}}    BB    ...  ..........} .. }}...  ... 
       ......    .....  B     }}}} ........     }}  .. }}}     .. .... .... 
     ..........  ..........    }}   ..... ..       .....        ........ }  
       ..  ...    .  .....         ....    ..       ...            ..       
                                                                            
]]);
-- Dungeon Description
des.region(selection.area(00,00,75,19), "unlit")
des.region({ region={13,01, 40,05}, lit=1, type="temple", filled=1, irregular=1 })
-- The occupied rooms.
des.region({ region={02,01, 08,03}, lit=1, type="ordinary", irregular=1 })
des.region({ region={01,11, 06,14}, lit=1, type="ordinary", irregular=1 })
des.region({ region={13,08, 18,10}, lit=1, type="ordinary", irregular=1 })
des.region({ region={05,17, 14,18}, lit=1, type="ordinary", irregular=1 })
des.region({ region={17,16, 23,18}, lit=1, type="ordinary", irregular=1 })
des.region({ region={35,16, 44,18}, lit=1, type="ordinary", irregular=1 })
-- Stairs
des.stair("down", 02,03)
-- Portal arrival point
des.levregion({ region = {71,09,71,09}, type="branch" })
-- Doors
des.door("locked",19,06)
-- The temple altar (this will force a priest(ess) to be created)
des.altar({ x=36,y=02, align="coaligned", type="shrine" })
-- Shaman Karnov
des.monster({ id = "Shaman Karnov", coord = {35, 02}, inventory = function()
   des.object({ id = "leather armor", spe = 5 });
   des.object({ id = "club", spe = 5 });
end })
-- The treasure of Shaman Karnov
des.object("chest", 34, 02)
-- neanderthal guards for the audience chamber
des.monster("neanderthal", 20, 03)
des.monster("neanderthal", 20, 02)
des.monster("neanderthal", 20, 01)
des.monster("neanderthal", 21, 03)
des.monster("neanderthal", 21, 02)
des.monster("neanderthal", 21, 01)
des.monster("neanderthal", 22, 01)
des.monster("neanderthal", 26, 09)
-- Non diggable walls
des.non_diggable(selection.area(00,00,75,19))

-- A couple of pits to make navigation a bit annoying
des.trap("pit",47,11)
des.trap("pit",57,10)
des.trap("bear",46,11)

-- Random traps

des.trap("pit")
des.trap("pit")
des.trap("pit")
des.trap("spiked pit")
des.trap("spiked pit")
des.trap("spiked pit")
des.trap("bear")
des.trap("bear")
des.trap("bear")
des.trap("spear")
des.trap("spear")
des.trap("spear")

-- Monsters on siege duty (in the outer caves).
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })
des.monster({ id = "giant eel", peaceful=0 })

des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })
des.monster({ id = "monkey", peaceful=0 })

des.monster({ id = "ape", peaceful=0 })
des.monster({ id = "ape", peaceful=0 })
des.monster({ id = "ape", peaceful=0 })
des.monster({ id = "ape", peaceful=0 })
des.monster({ id = "ape", peaceful=0 })

des.monster({ id = "python", peaceful=0 })
des.monster({ id = "python", peaceful=0 })
des.monster({ id = "python", peaceful=0 })
des.monster({ id = "python", peaceful=0 })

des.monster({ id = "panther", peaceful=0 })
des.monster({ id = "panther", peaceful=0 })
des.monster({ id = "panther", peaceful=0 })
des.monster({ id = "panther", peaceful=0 })
des.monster({ id = "tiger", peaceful=0 })
des.monster({ id = "tiger", peaceful=0 })
des.monster({ id = "weretiger", peaceful=0 })

des.wallify()
