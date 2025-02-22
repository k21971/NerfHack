/* NetHack 3.7	spell.h	$NHDT-Date: 1646838388 2022/03/09 15:06:28 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.14 $ */
/* Copyright 1986, M. Stephenson                                  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SPELL_H
#define SPELL_H

#define NO_SPELL 0
#define UNKNOWN_SPELL (-1)

/* spellbook re-use control; used when reading and when polymorphing */
#define MAX_SPELL_STUDY 3
#define CARD_COST 5
#define COUNTER_COST 10


struct spell {
    short sp_id;  /* spell id (== object.otyp) */
    xint16 sp_lev; /* power level */
    int sp_know;  /* knowledge of spell */
};

enum spellknowledge {
    spe_Forgotten  = -1, /* known but no longer castable */
    spe_Unknown    =  0, /* not yet known */
    spe_Fresh      =  1, /* castable if various casting criteria are met */
    spe_GoingStale =  2  /* still castable but nearly forgotten */
};

/* levels of memory destruction with a scroll of amnesia */
#define ALL_MAP 0x1
#define ALL_SPELLS 0x2

#define decrnknow(spell) svs.spl_book[spell].sp_know--
#define spellid(spell) svs.spl_book[spell].sp_id
#define spellknow(spell) svs.spl_book[spell].sp_know

/* how much Pw a spell of level lvl costs to cast? */
#define SPELL_LEV_PW(lvl) ((lvl) * 5)

/* These are the roles that specialize in spellcasting, they start with
 * spellbooks */
#define primary_spellcaster()   \
    (Role_if(PM_ARCHEOLOGIST)   \
    || Role_if(PM_HEALER)       \
    || Role_if(PM_CLERIC)       \
    || Role_if(PM_MONK)         \
    || Role_if(PM_WIZARD))

#endif /* SPELL_H */
