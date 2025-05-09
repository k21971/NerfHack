/* NetHack 3.7	monflag.h	$NHDT-Date: 1596498549 2020/08/03 23:49:09 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.21 $ */
/* Copyright (c) 1989 Mike Threepoint                             */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONFLAG_H
#define MONFLAG_H
/* clang-format off */
/* *INDENT-OFF* */

enum ms_sounds {
    MS_SILENT   =  0,   /* makes no sound */
    MS_BARK     =  1,   /* if full moon, may howl */
    MS_MEW      =  2,   /* mews or hisses */
    MS_ROAR     =  3,   /* roars */
    MS_BELLOW   =  4,   /* adult male crocodiles; hatchlings 'chirp' */
    MS_GROWL    =  5,   /* growls */
    MS_SQEEK    =  6,   /* squeaks, as a rodent */
    MS_SQAWK    =  7,   /* squawks, as a bird */
    MS_CHIRP    =  8,   /* baby crocodile */
    MS_HISS     =  9,   /* hisses */
    MS_BUZZ     = 10,   /* buzzes (killer bee) */
    MS_GRUNT    = 11,   /* grunts (or speaks own language) */
    MS_NEIGH    = 12,   /* neighs, as an equine */
    MS_MOO      = 13,   /* minotaurs, rothes */
    MS_WAIL     = 14,   /* wails, as a tortured soul */
    MS_GURGLE   = 15,   /* gurgles, as liquid or through saliva */
    MS_BURBLE   = 16,   /* burbles (jabberwock) */
    MS_TRUMPET  = 17,   /* trumpets (elephant) */
    MS_ANIMAL   = 17,   /* up to here are animal noises */
    /* FIXME? the grunt "speaks own language" case above
       shouldn't be classified as animal */
    MS_SHRIEK   = 18,   /* wakes up others */
    MS_BONES    = 19,   /* rattles bones (skeleton) */
    MS_LAUGH    = 20,   /* grins, smiles, giggles, and laughs */
    MS_MUMBLE   = 21,   /* says something or other */
    MS_IMITATE  = 22,   /* imitates others (leocrotta) */
    MS_WERE     = 23,   /* lycanthrope in human form */
    MS_ORC      = 24,   /* intelligent brutes */
    MS_GROAN    = 25,   /* zombies groan */
    MS_ATHOL    = 26,   /* athols; howls like a shrieker */
    /* from here onward, speech can be comprehended */
    MS_HUMANOID = 27,   /* generic traveling companion */
    MS_ARREST   = 28,   /* "Stop in the name of the law!" (Kops) */
    MS_SOLDIER  = 29,   /* army and watchmen expressions */
    MS_GUARD    = 30,   /* "Please drop that gold and follow me." */
    MS_DJINNI   = 31,   /* "Thank you for freeing me!" */
    MS_FAMILIAR = 32,   /* Vampire pets */
    MS_NURSE    = 33,   /* "Take off your shirt, please." */
    MS_SEDUCE   = 34,   /* "Hello, sailor." (Nymphs) */
    MS_VAMPIRE  = 35,   /* vampiric seduction, Vlad's exclamations */
    MS_BRIBE    = 36,   /* asks for money, or berates you */
    MS_CUSS     = 37,   /* berates (demons) or intimidates (Wiz) */
    MS_RIDER    = 38,   /* astral level special monsters */
    MS_LEADER   = 39,   /* your class leader */
    MS_NEMESIS  = 40,   /* your nemesis */
    MS_GUARDIAN = 41,   /* your leader's guards */
    MS_SELL     = 42,   /* demand payment, complain about shoplifters */
    MS_ORACLE   = 43,   /* do a consultation */
    MS_PRIEST   = 44,   /* ask for contribution; do cleansing */
    MS_SPELL    = 45,   /* spellcaster not matching any of the above */
    MS_BOAST    = 46,   /* giants */
    MS_GNOLL    = 47,   /* gnolls */
};

#define MR_FIRE         0x00000001L /* resists fire */
#define MR_COLD         0x00000002L /* resists cold */
#define MR_SLEEP        0x00000004L /* resists sleep */
#define MR_DISINT       0x00000008L /* resists disintegration */
#define MR_ELEC         0x00000010L /* resists electricity */
#define MR_POISON       0x00000020L /* resists poison */
#define MR_ACID         0x00000040L /* resists acid */
#define MR_STONE        0x00000080L /* resists petrification */
/* NB: the above resistances correspond to the first 8 hero properties in
   prop_types (FIRE_RES through STONE_RES), which can be converted to their
   MR_foo equivalents with the macro res_to_mr() defined in prop.h */
/* other resistances: magic, sickness */
/* other conveyances: teleport, teleport control, telepathy */

/* individual resistances */
#define MR2_LEVITATE    0x00000100L /* levitation */
#define MR2_FLYING      0x00000200L /* flying */
#define MR2_WATERWALK   0x00000400L /* water walking */
#define MR2_JUMPING     0x00000800L /* can jump */
#define MR2_SEE_INVIS   0x00001000L /* see invisible */
#define MR2_MAGBREATH   0x00002000L /* magical breathing */
#define MR2_DISPLACED   0x00004000L /* displaced */
#define MR2_STRENGTH    0x00008000L /* gauntlets of power */
#define MR2_FUMBLING    0x00010000L /* clumsy */
#define MR2_TELEPATHY   0x00020000L /* telepathic */
#define MR2_REFLECTION  0x00040000L /* has reflection */
#define MR2_FREE_ACTION 0x00080000L /* has free action */
#define MR2_PHASING     0x00100000L /* has phasing */

#define M1_FLY          0x00000001L /* can fly or float */
#define M1_SWIM         0x00000002L /* can traverse water */
#define M1_AMORPHOUS    0x00000004L /* can flow under doors */
#define M1_WALLWALK     0x00000008L /* can phase thru rock */
#define M1_CLING        0x00000010L /* can cling to ceiling */
#define M1_TUNNEL       0x00000020L /* can tunnel thru rock */
#define M1_NEEDPICK     0x00000040L /* needs pick to tunnel */
#define M1_CONCEAL      0x00000080L /* hides under objects */
#define M1_HIDE         0x00000100L /* mimics, blends in with ceiling */
#define M1_AMPHIBIOUS   0x00000200L /* can survive underwater */
#define M1_BREATHLESS   0x00000400L /* doesn't need to breathe */
#define M1_NOTAKE       0x00000800L /* cannot pick up objects */
#define M1_NOEYES       0x00001000L /* no eyes to gaze into or blind */
#define M1_NOHANDS      0x00002000L /* no hands to handle things */
#define M1_NOLIMBS      0x00006000L /* no arms/legs to kick/wear on */
#define M1_NOHEAD       0x00008000L /* no head to behead */
#define M1_MINDLESS     0x00010000L /* has no mind--golem, zombie, mold */
#define M1_HUMANOID     0x00020000L /* has humanoid head/arms/torso */
#define M1_ANIMAL       0x00040000L /* has animal body */
#define M1_SLITHY       0x00080000L /* has serpent body */
#define M1_UNSOLID      0x00100000L /* has no solid or liquid body */
#define M1_THICK_HIDE   0x00200000L /* has thick hide or scales */
#define M1_OVIPAROUS    0x00400000L /* can lay eggs */
#define M1_REGEN        0x00800000L /* regenerates hit points */
#define M1_SEE_INVIS    0x01000000L /* can see invisible creatures */
#define M1_TPORT        0x02000000L /* can teleport */
#define M1_TPORT_CNTRL  0x04000000L /* controls where it teleports to */
#define M1_ACID         0x08000000L /* acidic to eat */
#define M1_POIS         0x10000000L /* poisonous to eat */
#define M1_CARNIVORE    0x20000000L /* eats corpses */
#define M1_HERBIVORE    0x40000000L /* eats fruits */
#define M1_OMNIVORE     0x60000000L /* eats both */
#ifdef NHSTDC
#define M1_METALLIVORE  0x80000000UL /* eats metal */
#else
#define M1_METALLIVORE  0x80000000L /* eats metal */
#endif

#define M2_NOPOLY       0x00000001L /* players mayn't poly into one */
#define M2_COLLECT      0x00000002L /* picks up weapons and food */
#define M2_WANDER       0x00000004L /* wanders randomly */
#define M2_STALK        0x00000008L /* follows you to other levels */
#define M2_NASTY        0x00000010L /* extra-nasty monster (more xp) */
#define M2_STRONG       0x00000020L /* strong (or big) monster */
#define M2_ROCKTHROW    0x00000040L /* throws boulders */
#define M2_GREEDY       0x00000080L /* likes gold */
#define M2_JEWELS       0x00000100L /* likes gems */
#define M2_MERC         0x00000200L /* is a guard or soldier */
#define M2_LORD         0x00000400L /* is a lord to its kind */
#define M2_PRINCE       0x00000800L /* is an overlord to its kind */
#define M2_MINION       0x00001000L /* is a minion of a deity */
#define M2_DOMESTIC     0x00002000L /* can be tamed by feeding */
#define M2_SHAPESHIFTER 0x00004000L /* is a shapeshifting species */
#define M2_FLANK        0x00008000L /* will attempt to outflank the player */
#define M2_MALE         0x00010000L /* always male */
#define M2_FEMALE       0x00020000L /* always female */
#define M2_NEUTER       0x00040000L /* neither male nor female */
#define M2_PNAME        0x00080000L /* monster name is a proper name */
#define M2_HOSTILE      0x00100000L /* always starts hostile */
#define M2_PEACEFUL     0x00200000L /* always starts peaceful */


#ifdef NHSTDC
#define M2_MAGIC        0x80000000UL /* picks up magic items */
#else
#define M2_MAGIC        0x80000000L /* picks up magic items */
#endif

#define M3_WANTSAMUL    0x0001 /* would like to steal the amulet */
#define M3_WANTSBELL    0x0002 /* wants the bell */
#define M3_WANTSBOOK    0x0004 /* wants the book */
#define M3_WANTSCAND    0x0008 /* wants the candelabrum */
#define M3_WANTSARTI    0x0010 /* wants the quest artifact */
#define M3_WANTSALL     0x001f /* wants any major artifact */
#define M3_WAITFORU     0x0040 /* waits to see you or get attacked */
#define M3_CLOSE        0x0080 /* lets you close unless attacked */

#define M3_COVETOUS     0x001f /* wants something */
#define M3_WAITMASK     0x00c0 /* waiting... */

/* Infravision is currently implemented for players only */
#define M3_INFRAVISION  0x0100 /* has infravision */
#define M3_INFRAVISIBLE 0x0200 /* visible by infravision */
#define M3_DISPLACES    0x0400 /* moves monsters out of its way */
#define M3_JUMPER       0x0800 /* can jump at the player */
#define M3_NOTAME       0x1000 /* Cannot be tamed */
#define M3_BERSERK      0x2000 /* extremely indiscriminate in its attacks */
#define M3_ACCURATE     0x4000 /* gets an accuracy bonus to hit */

#define MZ_TINY         0 /* < 2' */
#define MZ_SMALL        1 /* 2-4' */
#define MZ_MEDIUM       2 /* 4-7' */
#define MZ_HUMAN        MZ_MEDIUM /* human-sized */
#define MZ_LARGE        3 /* 7-12' */
#define MZ_HUGE         4 /* 12-25' */
#define MZ_GIGANTIC     7 /* off the scale */

/* Monster species -- must stay within ROLE_RACEMASK */
/* Position is important for MH_HUMAN through MH_ORC - do not disturb!  */
#define MH_HUMAN        0x00000008L
#define MH_ELF          0x00000010L
#define MH_DWARF        0x00000020L
#define MH_GNOME        0x00000040L
#define MH_ORC          0x00000080L
#define MH_VAMPIRE      0x00000100L
#define MH_GRUNG        0x00000200L
/* End of playable races */
#define MH_WERE         0x00000400L
#define MH_GIANT        0x00000800L
#define MH_UNDEAD       0x00001000L
#define MH_DRAGON       0x00002000L
#define MH_TROLL        0x00004000L
#define MH_OGRE         0x00008000L
#define MH_JABBERWOCK   0x00010000L
#define MH_ANGEL        0x00020000L
#define MH_DEMON        0x00040000L

#define MH_ANY          0x80000000L

/* for mons[].geno (constant during game) */
#define G_VLGROUP	    0x2000 /* appear in very large groups normally */
#define G_UNIQ          0x1000 /* generated only once */
#define G_NOHELL        0x0800 /* not generated in "hell" */
#define G_HELL          0x0400 /* generated only in "hell" */
#define G_NOGEN         0x0200 /* generated only specially */
#define G_SGROUP        0x0080 /* appear in small groups normally */
#define G_LGROUP        0x0040 /* appear in large groups normally */
#define G_GENO          0x0020 /* can be exiled */
#define G_NOCORPSE      0x0010 /* no corpse left ever */
#define G_FREQ          0x0007 /* creation frequency mask */
/* note: G_IGNORE controls handling of mvitals[].mvflags bits but is
   passed to mkclass() as if it dealt with mons[].geno bits */
#define G_IGNORE        0x8000 /* for mkclass(), ignore G_GENOD|G_EXTINCT */

/* for svm.mvitals[].mvflags (variant during game), along with G_NOCORPSE */
#define G_KNOWN         0x04 /* have been encountered */
#define G_GENOD         0x02 /* have been exiled */
#define G_EXTINCT       0x01 /* population control; create no more */
#define G_GONE          (G_GENOD | G_EXTINCT)
#define MV_KNOWS_EGG    0x08 /* player recognizes egg of this monster type */

enum mgender { MALE, FEMALE, NEUTRAL,
               NUM_MGENDERS };

/* *INDENT-ON* */
/* clang-format on */
#endif /* MONFLAG_H */
