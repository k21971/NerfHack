/* NetHack 3.7	prop.h	$NHDT-Date: 1702274027 2023/12/11 05:53:47 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.24 $ */
/* Copyright (c) 1989 Mike Threepoint                             */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROP_H
#define PROP_H

/*** What the properties are ***
 *
 * note:  propertynames[] array in timeout.c has string values for these.
 *        Property #0 is not used.
 */
/* Resistances to troubles */
enum prop_types {
    /* Beginning of partial resistances: order is important.
     * See explode.c and monattk.h */
    FIRE_RES          =  1,
    COLD_RES          =  2,
    SLEEP_RES         =  3,
    DISINT_RES        =  4,
    SHOCK_RES         =  5,
    POISON_RES        =  6,
    /* End of partial resistances */
    ACID_RES          =  7,
    STONE_RES         =  8,
    /* note: the first eight properties above are equivalent to MR_xxx bits
     * MR_FIRE through MR_STONE, and can be directly converted to them: */
#define res_to_mr(r) \
    ((FIRE_RES <= (r) && (r) <= STONE_RES) ? (uchar) (1 << ((r) - 1)) : 0x00)
    DRAIN_RES         =  9,
    SICK_RES          = 10,
    INVULNERABLE      = 11,
    ANTIMAGIC         = 12,
    /* Troubles */
    STUNNED           = 13,
    CONFUSION         = 14,
    BLINDED           = 15,
    DEAF              = 16,
    SICK              = 17,
    STONED            = 18,
    STRANGLED         = 19,
    VOMITING          = 20,
    GLIB              = 21,
    SLIMED            = 22,
    HALLUC            = 23,
    HALLUC_RES        = 24,
    FUMBLING          = 25,
    WOUNDED_LEGS      = 26,
    SLEEPY            = 27,
    HUNGER            = 28,
    /* Vision and senses */
    SEE_INVIS         = 29,
    TELEPAT           = 30,
    WARNING           = 31,
    WARN_OF_MON       = 32,
    WARN_UNDEAD       = 33,
    SEARCHING         = 34,
    CLAIRVOYANT       = 35,
    INFRAVISION       = 36,
    DETECT_MONSTERS   = 37,
    BLND_RES          = 38,
    /* Appearance and behavior */
    ADORNED           = 39,
    INVIS             = 40,
    DISPLACED         = 41,
    STEALTH           = 42,
    AGGRAVATE_MONSTER = 43,
    CONFLICT          = 44,
    /* Transportation */
    JUMPING           = 45,
    TELEPORT          = 46,
    TELEPORT_CONTROL  = 47,
    LEVITATION        = 48,
    FLYING            = 49,
    WWALKING          = 50,
    SWIMMING          = 51,
    MAGICAL_BREATHING = 52,
    PASSES_WALLS      = 53,
    /* Physical attributes */
    SLOW_DIGESTION    = 54,
    HALF_SPDAM        = 55,
    HALF_PHDAM        = 56,
    REGENERATION      = 57,
    ENERGY_REGENERATION = 58,
    PROTECTION        = 59,
    PROT_FROM_SHAPE_CHANGERS = 60,
    POLYMORPH         = 61,
    POLYMORPH_CONTROL = 62,
    UNCHANGING        = 63,
    FAST              = 64,
    REFLECTING        = 65,
    FREE_ACTION       = 66,
    FIXED_ABIL        = 67,
    WITHERING         = 68,
    LIFESAVED         = 69,
    STOMPING          = 70,
    STUN_RES          = 71,
    BREATHLESS        = 72,
    RABID             = 73,
    WATERTIGHT        = 74,
    LAST_PROP = WATERTIGHT
};

/*** Where the properties come from ***/
/* Definitions were moved here from obj.h and you.h */
struct prop {
    /*** Properties conveyed by objects ***/
    long extrinsic;
/* Armor */
#define W_ARM 0x00000001L  /* Body armor */
#define W_ARMC 0x00000002L /* Cloak */
#define W_ARMH 0x00000004L /* Helmet/hat */
#define W_ARMS 0x00000008L /* Shield */
#define W_ARMG 0x00000010L /* Gloves/gauntlets */
#define W_ARMF 0x00000020L /* Footwear */
#define W_ARMU 0x00000040L /* Undershirt */
#define W_ARMOR (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF | W_ARMU)
/* Weapons and artifacts */
#define W_WEP 0x00000100L     /* Wielded weapon */
#define W_QUIVER 0x00000200L  /* Quiver for (f)iring ammo */
#define W_SWAPWEP 0x00000400L /* Secondary weapon */
#define W_WEAPONS (W_WEP | W_SWAPWEP | W_QUIVER)
#define W_ART 0x00001000L     /* Carrying artifact (not really worn) */
#define W_ARTI 0x00002000L    /* Invoked artifact  (not really worn) */
/* Amulets, rings, tools, and other items */
#define W_AMUL 0x00010000L    /* Amulet */
#define W_RINGL 0x00020000L   /* Left ring */
#define W_RINGR 0x00040000L   /* Right ring */
#define W_RING (W_RINGL | W_RINGR)
#define W_TOOL 0x00080000L   /* Eyewear */
#define W_ACCESSORY (W_RING | W_AMUL | W_TOOL)
    /* historical note: originally in slash'em, 'worn' saddle stayed in
       hero's inventory; in nethack, it's kept in the steed's inventory */
#define W_SADDLE 0x00100000L /* KMH -- For riding monsters */
#define W_BALL 0x00200000L   /* Punishment ball */
#define W_CHAIN 0x00400000L  /* Punishment chain */

    /*** Property is blocked by an object ***/
    long blocked; /* Same assignments as extrinsic */

    /*** Timeouts, permanent properties, and other flags ***/
    long intrinsic;
/* Timed properties */
#define TIMEOUT 0x00ffffffL     /* Up to 16 million turns */
/* Permanent properties */
#define FROMEXPER   0x01000000L /* Gain/lose with experience, for role */
#define FROMRACE    0x02000000L /* Gain/lose with experience, for race */
#define FROMOUTSIDE 0x04000000L /* By corpses, prayer, thrones, etc. */
#define HAVEPARTIAL 0x08000000L /* Partial resistance */
#define INTRINSIC (FROMOUTSIDE | FROMRACE | FROMEXPER | HAVEPARTIAL)
/* Control flags */
#define FROMFORM    0x10000000L /* Polyd; conferred by monster form */
#define I_SPECIAL   0x20000000L /* Property is controllable */
};

/*** Definitions for backwards compatibility ***/
#define LEFT_RING W_RINGL
#define RIGHT_RING W_RINGR
#define LEFT_SIDE LEFT_RING
#define RIGHT_SIDE RIGHT_RING
#define BOTH_SIDES (LEFT_SIDE | RIGHT_SIDE)
#define WORN_ARMOR W_ARM
#define WORN_CLOAK W_ARMC
#define WORN_HELMET W_ARMH
#define WORN_SHIELD W_ARMS
#define WORN_GLOVES W_ARMG
#define WORN_BOOTS W_ARMF
#define WORN_AMUL W_AMUL
#define WORN_BLINDF W_TOOL
#define WORN_SHIRT W_ARMU

#endif /* PROP_H */
