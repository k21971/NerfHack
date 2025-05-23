/* NetHack 3.7	weapon.c	$NHDT-Date: 1725227810 2024/09/01 21:56:50 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.128 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2011. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *      This module contains code for calculation of "to hit" and damage
 *      bonuses for any given weapon used, as well as weapons selection
 *      code for monsters.
 */
#include "hack.h"

staticfn void give_may_advance_msg(int);
staticfn void finish_towel_change(struct obj *obj, int) NONNULLARG1;
staticfn boolean could_advance(int);
staticfn boolean peaked_skill(int);
staticfn int slots_required(int);
staticfn void skill_advance(int);
staticfn int dmgval_core(struct obj*, struct monst*, struct damage_info_t*);

/* Categories whose names don't come from OBJ_NAME(objects[type])
 */
#define PN_BARE_HANDED          (-1) /* includes martial arts */
#define PN_TWO_WEAPONS          (-2)
#define PN_SHIELD               (-3)
#define PN_RIDING               (-4)
#define PN_POLEARMS             (-5)
#define PN_SABER                (-6)
#define PN_HAMMER               (-7)
#define PN_WHIP                 (-8)
#define PN_ATTACK_SPELL         (-9)
#define PN_HEALING_SPELL        (-10)
#define PN_DIVINATION_SPELL     (-11)
#define PN_ENCHANTMENT_SPELL    (-12)
#define PN_CLERIC_SPELL         (-13)
#define PN_ESCAPE_SPELL         (-14)
#define PN_MATTER_SPELL         (-15)

static NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
    /* Weapon */
    0,
    DAGGER,
    KNIFE,
    AXE,
    PICK_AXE,
    SHORT_SWORD,
    BROADSWORD,
    LONG_SWORD,
    TWO_HANDED_SWORD,
    PN_SABER,
    CLUB,
    MACE,
    MORNING_STAR,
    FLAIL,
    PN_HAMMER,
    QUARTERSTAFF,
    PN_POLEARMS,
    SPEAR,
    TRIDENT,
    LANCE,
    BOW,
    SLING,
    CROSSBOW,
    DART,
    SHURIKEN,
    BOOMERANG,
    PN_WHIP,
    UNICORN_HORN,
    /* Spell */
    PN_ATTACK_SPELL,
    PN_HEALING_SPELL,
    PN_DIVINATION_SPELL,
    PN_ENCHANTMENT_SPELL,
    PN_CLERIC_SPELL,
    PN_ESCAPE_SPELL,
    PN_MATTER_SPELL,
    /* Other */
    PN_BARE_HANDED,
    PN_TWO_WEAPONS,
    PN_SHIELD,
    PN_RIDING
};

/* note: entry [0] isn't used */
static NEARDATA const char *const odd_skill_names[] = {
    "no skill",
    "bare hands", /* use barehands_or_martial[] instead */
    "two weapon combat",
    "shield",
    "riding",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "clerical spells",
    "escape spells",
    "matter spells",
};
/* indexed via is_martial() */
static NEARDATA const char *const barehands_or_martial[] = {
    "bare handed combat",
    "martial arts"
};

#define P_NAME(type)                                    \
    ((skill_names_indices[type] > 0)                    \
         ? OBJ_NAME(objects[skill_names_indices[type]]) \
         : (type == P_BARE_HANDED_COMBAT)               \
               ? barehands_or_martial[martial_bonus()]  \
               : odd_skill_names[-skill_names_indices[type]])

/* targets that provide attacker with small to-hit bonus when using a spear */
const char kebabable[] = {
    S_XORN, S_DRAGON, S_JABBERWOCK, S_NAGA, S_GIANT,  '\0'
};

staticfn void
give_may_advance_msg(int skill)
{
    You_feel("more confident in your %sskills.",
             (skill == P_NONE) ? ""
                 : (skill <= P_LAST_WEAPON) ? "weapon "
                     : (skill <= P_LAST_SPELL) ? "spell casting "
                        : (skill == P_SHIELD) ? "defensive "
                            : "fighting ");
    (void) handle_tip(TIP_ENHANCE);
}

/* weapon's skill category name for use as generalized description of weapon;
   mostly used to shorten "you drop your <weapon>" messages when slippery
   fingers or polymorph causes hero to involuntarily drop wielded weapon(s) */
const char *
weapon_descr(struct obj *obj)
{
    int skill = weapon_type(obj);
    const char *descr = P_NAME(skill);

    /* assorted special cases */
    switch (skill) {
    case P_NONE:
        /* not a weapon or weptool: use item class name;
           override class name for things where it sounds strange and
           for things that aren't unexpected to find being wielded:
           corpses, tins, eggs, and globs avoid "food",
           statues and boulders avoid "large rock",
           and towels and tin openers avoid "tool" */
        descr = (obj->otyp == CORPSE || obj->otyp == TIN || obj->otyp == EGG
                 || obj->otyp == STATUE || obj->otyp == BOULDER
                 || obj->otyp == TOWEL || obj->otyp == TIN_OPENER)
                ? OBJ_NAME(objects[obj->otyp])
                : obj->globby ? "glob"
                  : def_oc_syms[(int) obj->oclass].name;
        break;
    case P_SLING:
        if (is_ammo(obj))
            descr = (obj->otyp == ROCK || is_graystone(obj->otyp))
                        ? "stone"
                        /* avoid "rock"; what about known glass? */
                        : (obj->oclass == GEM_CLASS)
                            ? "gem"
                            /* in case somebody adds odd sling ammo */
                            : def_oc_syms[(int) obj->oclass].name;
        break;
    case P_BOW:
        if (is_ammo(obj))
            descr = "arrow";
        break;
    case P_CROSSBOW:
        if (is_ammo(obj))
            descr = "bolt";
        break;
    case P_FLAIL:
        if (obj->otyp == GRAPPLING_HOOK)
            descr = "hook";
        break;
    case P_PICK_AXE:
        /* even if "dwarvish mattock" hasn't been discovered yet */
        if (obj->otyp == DWARVISH_MATTOCK)
            descr = "mattock";
        break;
    default:
        break;
    }
    return makesingular(descr);
}

/*
 *      hitval returns an integer representing the "to hit" bonuses
 *      of "otmp" against the monster.
 */
int
hitval(struct obj *otmp, struct monst *mon)
{
    int tmp = 0;
    struct permonst *ptr = mon->data;
    boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || is_weptool(otmp));

    if (Is_weapon)
        tmp += (otmp->spe > 0) ? rnd(otmp->spe) : otmp->spe;

    /* Put weapon specific "to hit" bonuses in below: */
    tmp += objects[otmp->otyp].oc_hitbon;

    /* Put weapon vs. monster type "to hit" bonuses in below: */

    /* Blessed weapons used against undead or demons */
    if (Is_weapon && otmp->blessed && mon_hates_blessings(mon))
        tmp += 2;

    /* Undead Slayers are naturally gifted at dispatching undead. */
    if (Role_if(PM_UNDEAD_SLAYER) && mon_hates_blessings(mon))
        tmp += 2;

    if (is_spear(otmp) && strchr(kebabable, ptr->mlet))
        tmp += 2;

    if (otmp->otyp == SCIMITAR && u.usteed)
        tmp += 4;

    /* trident is highly effective against swimmers */
    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
        if (is_damp_terrain(mon->mx, mon->my))
            tmp += 4;
        else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE)
            tmp += 2;
    }

    /* Picks used against xorns and earth elementals */
    if (is_pick(otmp) && (passes_walls(ptr) && thick_skinned(ptr)))
        tmp += 2;

    if (is_pole(otmp) && (ptr->mlet == S_CENTAUR || ptr->mlet == S_UNICORN))
        tmp += 4;

    /* Check specially named weapon "to hit" bonuses */
    if (otmp->oartifact)
        tmp += spec_abon(otmp, mon);

    return tmp;
}

/* Historical note: The original versions of Hack used a range of damage
 * which was similar to, but not identical to, the damage used in Advanced
 * Dungeons and Dragons.  I figured that since it was so close, I may as well
 * make it exactly the same as AD&D, adding some more weapons in the process.
 * This has the advantage that it is at least possible that the player would
 * already know the damage of at least some of the weapons.  This was circa
 * 1987 and I used the table from Unearthed Arcana until I got tired of typing
 * them in (leading to something of an imbalance towards weapons early in
 * alphabetical order).  The data structure still doesn't include fields that
 * fully allow the appropriate damage to be described (there's no way to say
 * 3d6 or 1d6+1) so we add on the extra damage in dmgval() if the weapon
 * doesn't do an exact die of damage.
 *
 * Of course new weapons were added later in the development of Nethack.  No
 * AD&D consistency was kept, but most of these don't exist in AD&D anyway.
 *
 * Second edition AD&D came out a few years later; luckily it used the same
 * table.  As of this writing (1999), third edition is in progress but not
 * released.  Let's see if the weapon table stays the same.  --KAA
 * October 2000: It didn't.  Oh, well.
 */
int
dmgval(struct obj *otmp, struct monst *mon)
{
    struct damage_info_t ignored = {0};
    return dmgval_core(otmp, mon, &ignored);
}

struct damage_info_t
dmgval_info(struct obj *otmp)
{
    struct damage_info_t damage_info = {0};
    damage_info.bonus_small = "";
    damage_info.bonus_large = "";
    (void) dmgval_core(otmp, NULL, &damage_info);
    return damage_info;
}

/*
 *      dmgval returns an integer representing the damage bonuses
 *      of "otmp" against the monster.
 */
staticfn int
dmgval_core(
    struct obj *otmp,
    struct monst *mon,
    struct damage_info_t *damage_info)
{
    int tmp = 0, otyp = otmp->otyp;
    struct permonst *ptr = mon ? mon->data : NULL;
    boolean Is_weapon = (otmp->oclass == WEAPON_CLASS || is_weptool(otmp));

    if (otyp == CREAM_PIE || otyp == PINCH_OF_CATNIP)
        return 0;

    /* Damage vs large monsters */

    if (ptr == NULL || bigmonst(ptr)) {
        if (objects[otyp].oc_wldam) {
            tmp = rnd(objects[otyp].oc_wldam);
            damage_info->damage_large = objects[otyp].oc_wldam;
        }
        switch (otyp) {
        case IRON_CHAIN:
        case CROSSBOW_BOLT:
        case MORNING_STAR:
        case PARTISAN:
        case RUNESWORD:
        case ELVEN_BROADSWORD:
        case BROADSWORD:
        case SLING_BULLET:
            tmp++;
            damage_info->bonus_large = "+1";
            break;

        case FLAIL:
        case RANSEUR:
        case SCYTHE:
        case VOULGE:
            tmp += rnd(4);
            damage_info->bonus_large = "+1d4";
            break;

        case ACID_VENOM:
        case HALBERD:
        case SPETUM:
            tmp += rnd(6);
            damage_info->bonus_large = "+1d6";
            break;

        case WAR_HAMMER:
            tmp += rnd(8);
            damage_info->bonus_large = "+1d8";
            break;

        case BATTLE_AXE:
        case BARDICHE:
        case TRIDENT:
            tmp += d(2, 4);
            damage_info->bonus_large = "+2d4";
            break;

        case TSURUGI:
        case DWARVISH_MATTOCK:
        case TWO_HANDED_SWORD:
            tmp += d(2, 6);
            damage_info->bonus_large = "+2d6";
            break;
        case HEAVY_SWORD:
            tmp += d(2, 8);
            damage_info->bonus_large = "+2d8";
            break;
        case BOULDER:
            tmp += d(5, 4);
            damage_info->bonus_large = "+5d4";
            break;
        }
    }

    /* Damage vs small monsters */

    if (ptr == NULL || !bigmonst(ptr)) {
        if (objects[otyp].oc_wsdam) {
            tmp = rnd(objects[otyp].oc_wsdam);
            damage_info->damage_small = objects[otyp].oc_wsdam;
        }
        switch (otyp) {
        case IRON_CHAIN:
        case CROSSBOW_BOLT:
        case MACE:
        case SILVER_MACE:
        case FLAIL:
        case SPETUM:
        case TRIDENT:
        case SLING_BULLET:
            tmp++;
            damage_info->bonus_small = "+1";
            break;

        case BATTLE_AXE:
        case BARDICHE:
        case BILL_GUISARME:
        case GUISARME:
        case LUCERN_HAMMER:
        case MORNING_STAR:
        case RANSEUR:
        case BROADSWORD:
        case ELVEN_BROADSWORD:
        case RUNESWORD:
        case SCYTHE:
        case VOULGE:
            tmp += rnd(4);
            damage_info->bonus_small = "+1d4";
            break;

        case WAR_HAMMER:
        case ACID_VENOM:
            tmp += rnd(6);
            damage_info->bonus_small = "+1d6";
            break;
        case HEAVY_SWORD:
            tmp += d(2, 66);
            damage_info->bonus_small = "+2d6";
            break;
        case BOULDER:
            tmp += d(5, 4);
            damage_info->bonus_small = "+5d4";
            break;
        }
    }
    if (Is_weapon) {
        tmp += otmp->spe;
        /* negative enchantment mustn't produce negative damage */
        if (tmp < 0)
            tmp = 0;
    }

    if (objects[otyp].oc_material <= LEATHER && ptr && thick_skinned(ptr))
        /* thick skinned/scaled creatures don't feel it */
        tmp = 0;

    if (otmp->oclass == GEM_CLASS && ptr && thick_skinned(ptr))
        tmp = 0; /* pebbles don't penetrate */

    if (ptr && shadelike(ptr) && !shade_glare(otmp))
        tmp = 0;

    /* "very heavy iron ball"; weight increase is in increments */
    if (otyp == HEAVY_IRON_BALL && tmp > 0) {
        int wt = (int) objects[HEAVY_IRON_BALL].oc_weight;

        if ((int) otmp->owt > wt) {
            wt = ((int) otmp->owt - wt) / WT_IRON_BALL_INCR;
            tmp += rnd(4 * wt);
            if (tmp > 25)
                tmp = 25; /* objects[].oc_wldam */
        }
    }

    /* Put weapon vs. monster type damage bonuses in below: */
    if (Is_weapon || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS
        || otmp->oclass == CHAIN_CLASS) {
        int bonus = 0;

        if (otmp->blessed && mon && mon_hates_blessings(mon))
            bonus += rnd(4);
        /* Undead Slayers are naturally gifted at dispatching undead. */
        if (mon && Role_if(PM_UNDEAD_SLAYER) && mon_hates_blessings(mon))
            bonus += 1 + rnd(u.ulevel / 5 + 1);

        if (is_axe(otmp)) {
            if (ptr && is_wooden(ptr))
                bonus += rnd(4);
            damage_info->axe_damage = "\t+1d4 against wood golems.";
        }
        if (is_silver(otmp) && mon && mon_hates_silver(mon)) {
            bonus += rnd(20);
            damage_info->hate_damage = "\t+1d20 against silver hating monsters.";
        }
        if (artifact_light(otmp) && otmp->lamplit && ptr && hates_light(ptr)) {
            bonus += rnd(8);
            damage_info->light_damage =
                "\tAdditional 1d8 against light hating monsters.";
        }

        /* if the weapon is going to get a double damage bonus, adjust
           this bonus so that effectively it's added after the doubling */
        if (bonus > 1 && otmp->oartifact && spec_dbon(otmp, mon, 25) >= 25)
            bonus = (bonus + 1) / 2;

        tmp += bonus;
    }

    if (tmp > 0) {
        int mac = (mon && mon != &cg.zeromonst) ? find_mac(mon) : 10;
        /* It's debatable whether a rusted blunt instrument
           should do less damage than a pristine one, since
           it will hit with essentially the same impact, but
           there ought to some penalty for using damaged gear
           so always subtract erosion even for blunt weapons. */
        tmp -= greatest_erosion(otmp);

        /* Low AC subtracts damage, just as it does with players */
        if (mac < 0)
            tmp -= rnd(-mac);

        if (tmp < 1)
            tmp = 1;
    }

    return tmp;
}

/* check whether blessed and/or silver damage applies for *non-weapon* hit;
   return value is the amount of the extra damage */
int
special_dmgval(
    struct monst *magr, /* attacker */
    struct monst *mdef, /* defender */
    long armask,        /* armor mask, multiple bits accepted for
                         * W_ARMC|W_ARM|W_ARMU or
                         * W_ARMG|W_RINGL|W_RINGR only */
    long *silverhit_p)  /* output flag mask for silver bonus */
{
    struct obj *obj;
    boolean left_ring = (armask & W_RINGL) ? TRUE : FALSE,
            right_ring = (armask & W_RINGR) ? TRUE : FALSE;
    long silverhit = 0L;
    int bonus = 0;

    obj = 0;
    if (armask & (W_ARMC | W_ARM | W_ARMU)) {
        if ((armask & W_ARMC) != 0L
            && (obj = which_armor(magr, W_ARMC)) != 0)
            armask = W_ARMC;
        else if ((armask & W_ARM) != 0L
                 && (obj = which_armor(magr, W_ARM)) != 0)
            armask = W_ARM;
        else if ((armask & W_ARMU) != 0L
                 && (obj = which_armor(magr, W_ARMU)) != 0)
            armask = W_ARMU;
        else
            armask = 0L;
    } else if (armask & (W_ARMG | W_RINGL | W_RINGR)) {
        armask = ((obj = which_armor(magr, W_ARMG)) != 0) ?  W_ARMG : 0L;
    } else {
        obj = which_armor(magr, armask);
    }

    if (obj) {
        if (obj->blessed && mon_hates_blessings(mdef))
            bonus += rnd(4);
        /* the only silver armor is shield of reflection (silver dragon
           scales refer to color, not material) and the only way to hit
           with one--aside from throwing--is to wield it and perform a
           weapon hit, but we include a general check here */
        if (is_silver(obj) && mon_hates_silver(mdef)) {
            bonus += rnd(20);
            silverhit |= armask;
        }

    /* when no gloves we check for silver rings (blessed rings ignored) */
    } else if ((left_ring || right_ring) && magr == &gy.youmonst) {
        if (left_ring && uleft) {
            if (is_silver(uleft) && mon_hates_silver(mdef)) {
                bonus += rnd(20);
                silverhit |= W_RINGL;
            }
        }
        if (right_ring && uright) {
            if (is_silver(uright) && mon_hates_silver(mdef)) {
                /* two silver rings don't give double silver damage
                   but 'silverhit' messages might be adjusted for them */
                if (!(silverhit & W_RINGL))
                    bonus += rnd(20);
                silverhit |= W_RINGR;
            }
        }
    }

    if (silverhit_p)
        *silverhit_p = silverhit;
    return bonus;
}

/* give a "silver <item> sears <target>" message;
   not used for weapon hit, so we only handle rings */
void
silver_sears(struct monst *magr UNUSED, struct monst *mdef,
             long silverhit)
{
    char rings[20]; /* plenty of room for "rings" */
    int ltyp = ((uleft && (silverhit & W_RINGL) != 0L)
                ? uleft->otyp : STRANGE_OBJECT),
        rtyp = ((uright && (silverhit & W_RINGR) != 0L)
                ? uright->otyp : STRANGE_OBJECT);
    boolean both,
        l_dknown = (uleft && uleft->dknown),
        r_dknown = (uright && uright->dknown),
        l_ag = (objects[ltyp].oc_material == SILVER && l_dknown),
        r_ag = (objects[rtyp].oc_material == SILVER && r_dknown);

    if ((silverhit & (W_RINGL | W_RINGR)) != 0L) {
        /* plural if both the same type (so not multi_claw and both rings
           are non-Null) and either both known or neither known, or both
           silver (in case there is ever more than one type of silver ring)
           and both known; singular if multi_claw (where one of ltyp or
           rtyp will always be STRANGE_OBJECT) even if both rings are known
           silver [see hmonas(uhitm.c) for explanation of 'multi_claw'] */
        both = ((ltyp == rtyp && l_dknown == r_dknown) || (l_ag && r_ag));
        Sprintf(rings, "ring%s", both ? "s" : "");
        Your("%s%s %s %s!",
             (l_ag || r_ag) ? "silver "
             : both ? ""
               : ((silverhit & W_RINGL) != 0L) ? "left "
                 : "right ",
             rings, vtense(rings, "sear"), mon_nam(mdef));
    } else if ((silverhit & W_ARMS) != 0L) {
        Your("silver shield sears %s!", mon_nam(mdef));
    }
}

staticfn struct obj *oselect(struct monst *, int);
#define Oselect(x) \
    do {                                        \
        if ((otmp = oselect(mtmp, x)) != 0)     \
            return otmp;                        \
    } while (0)

staticfn struct obj *
oselect(struct monst *mtmp, int type)
{
    struct obj *otmp;

    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
        if (otmp->otyp != type)
            continue;

        /* never select non-cockatrice corpses */
        if ((type == CORPSE || type == EGG)
            && (otmp->corpsenm == NON_PM
                || !touch_petrifies(&mons[otmp->corpsenm])))
            continue;

        if (!can_touch_safely(mtmp, otmp))
            continue;

        /* Don't select items our race isn't compatible with */
        if (hates_item(mtmp, otmp->otyp))
            continue;

        return otmp;
    }
    return (struct obj *) 0;
}

static NEARDATA const int rwep[] = {
    DWARVISH_SPEAR,
    SILVER_SPEAR,
    ELVEN_SPEAR,
    SPEAR,
    ORCISH_SPEAR,
    JAVELIN,
    SHURIKEN,
    YA,
    PINEAPPLE,
    SILVER_ARROW,
    ELVEN_ARROW,
    ARROW,
    ORCISH_ARROW,
    CROSSBOW_BOLT,
    SILVER_DAGGER,
    ELVEN_DAGGER,
    DAGGER,
    ORCISH_DAGGER,
    KNIFE,
    SLING_BULLET,
    FLINT,
    ROCK,
    LOADSTONE,
    LUCKSTONE,
    DART,
    /* BOOMERANG, */
    CREAM_PIE
};

static NEARDATA const int pwep[] = {
    SCYTHE,
    BARDICHE,
    HALBERD,
    SPETUM,
    BILL_GUISARME,
    VOULGE,
    RANSEUR,
    GUISARME,
    GLAIVE,
    LUCERN_HAMMER,
    BEC_DE_CORBIN,
    FAUCHARD,
    PARTISAN,
    LANCE };

boolean
would_prefer_rwep(struct monst *mtmp, struct obj *otmp)
{
    struct obj *wep = select_rwep(mtmp);
    int i = 0;

    if (wep) {
        if (wep == otmp)
            return TRUE;
        if (wep->oartifact)
            return FALSE;
        if (mtmp->data->mlet == S_KOP) {
            if (wep->otyp == CREAM_PIE)
                return FALSE;
            if (otmp->otyp == CREAM_PIE)
                return TRUE;
        }
        if (throws_rocks(mtmp->data)) {
            if (wep->otyp == BOULDER)
                return FALSE;
            if (otmp->otyp == BOULDER)
                return TRUE;
        }
    }

    if (strongmonst(mtmp->data) && (mtmp->misc_worn_check & W_ARMS) == 0) {
        for (i = 0; i < SIZE(pwep); i++) {
            if (wep && wep->otyp == pwep[i]
                  && !(otmp->otyp == pwep[i]
                  && dmgval(otmp, &gy.youmonst) > dmgval(wep, &gy.youmonst)))
                return FALSE;
            if (otmp->otyp == pwep[i])
                return TRUE;
        }
    }

    if (is_pole(otmp))
        return FALSE; /* If we get this far,
                         we failed the polearm strength check */

    for (i = 0; i < SIZE(rwep); i++) {
        if (wep && wep->otyp == rwep[i]
              && !(otmp->otyp == rwep[i]
	          && dmgval(otmp, &gy.youmonst) > dmgval(wep, &gy.youmonst)))
	        return FALSE;
        if (otmp->otyp == rwep[i])
            return TRUE;
    }
    return FALSE;
}


#define AKLYS_LIM (BOLT_LIM / 2)
/* throw-and-return weapons */
static NEARDATA const struct throw_and_return_weapon arwep[] = {
    /* { BOOMERANG, 5, 0 }, */
    { AKLYS, AKLYS_LIM * AKLYS_LIM, 1 },
};

const struct throw_and_return_weapon *
autoreturn_weapon(struct obj *otmp)
{
    int i;

    for (i = 0; i < SIZE(arwep); i++) {
        if (otmp->otyp == arwep[i].otyp)
            return &arwep[i];
    }
    return (struct throw_and_return_weapon *) 0;
}

/* select a ranged weapon for the monster */
struct obj *
select_rwep(struct monst *mtmp)
{
    struct obj *otmp;
    struct obj *mwep;
    boolean mweponly;
    int i;

    char mlet = mtmp->data->mlet;

    gp.propellor = &hands_obj;
    Oselect(EGG);      /* cockatrice egg */
    if (mlet == S_KOP) /* pies are first choice for Kops */
        Oselect(CREAM_PIE);
    if (throws_rocks(mtmp->data)) /* ...boulders for giants */
        Oselect(BOULDER);

    /* Select polearms first; they do more damage and aren't expendable.
       But don't pick one if monster's weapon is welded, because then
       we'd never have a chance to throw non-wielding missiles. */
    /* The limit of 13 here is based on the monster polearm range limit
     * (defined as 5 in mthrowu.c).  5 corresponds to a distance of 2 in
     * one direction and 1 in another; one space beyond that would be 3 in
     * one direction and 2 in another; 3^2+2^2=13.
     */
    mwep = MON_WEP(mtmp);
    /* NO_WEAPON_WANTED means we already tried to wield and failed */
    mweponly = (mwelded(mwep) && mtmp->weapon_check == NO_WEAPON_WANTED);
    if (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 13
        && couldsee(mtmp->mx, mtmp->my)) {
        for (i = 0; i < SIZE(pwep); i++) {
            /* Only strong monsters can wield big (esp. long) weapons.
             * Big weapon is basically the same as bimanual.
             * All monsters can wield the remaining weapons.
             */
            if (((strongmonst(mtmp->data)
                  && (mtmp->misc_worn_check & W_ARMS) == 0)
                 || !objects[pwep[i]].oc_bimanual)
                && (objects[pwep[i]].oc_material != SILVER
                    || !mon_hates_silver(mtmp))) {
                if ((otmp = oselect(mtmp, pwep[i])) != 0
                    && (otmp == mwep || !mweponly)) {
                    gp.propellor = otmp; /* force the monster to wield it */
                    return otmp;
                }
            }
        }
    }
    /* Next, try to select a throw-and-return weapon, since they are
     * also not as expendable. Again, don't pick one if monster's
     * weapon is welded.
     */
    for (i = 0; i < SIZE(arwep); i++) {
        const struct throw_and_return_weapon *arw = &arwep[i];

        if (!mindless(mtmp->data) && !is_animal(mtmp->data) && !mweponly
            && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= arw->range
            && couldsee(mtmp->mx, mtmp->my)) {
            if ((((mtmp->misc_worn_check & W_ARMS) == 0)
                 || !objects[arw->otyp].oc_bimanual)
                && (objects[arw->otyp].oc_material != SILVER
                    || !mon_hates_silver(mtmp))) {
                if ((otmp = oselect(mtmp, arw->otyp)) != 0
                    && (otmp == mwep || !mweponly)) {
                    gp.propellor = otmp; /* force the monster to wield it */
                    return otmp;
                }
            }
        }
    }

    /*
     * other than the specific cases above, always select the
     * most potent ranged weapon to hand.
     */
    for (i = 0; i < SIZE(rwep); i++) {
        int prop;

        /* shooting gems from slings; this goes just before the darts */
        /* (shooting rocks is already handled via the rwep[] ordering) */
        if (rwep[i] == DART && !likes_gems(mtmp->data)
            && m_carrying(mtmp, SLING)) { /* propellor */
            for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
                if (otmp->oclass == GEM_CLASS
                    && (otmp->otyp != LOADSTONE || !otmp->cursed)) {
                    gp.propellor = m_carrying(mtmp, SLING);
                    return otmp;
                }
        }

        /* KMH -- This belongs here so darts will work */
        gp.propellor = &hands_obj;

        prop = objects[rwep[i]].oc_skill;
        if (prop < 0) {
            switch (-prop) {
            case P_BOW:
                gp.propellor = oselect(mtmp, YUMI);
                if (!gp.propellor)
                    gp.propellor = oselect(mtmp, ELVEN_BOW);
                if (!gp.propellor)
                    gp.propellor = oselect(mtmp, BOW);
                if (!gp.propellor)
                    gp.propellor = oselect(mtmp, ORCISH_BOW);
                break;
            case P_SLING:
                gp.propellor = oselect(mtmp, SLING);
                break;
            case P_CROSSBOW:
                gp.propellor = oselect(mtmp, CROSSBOW);
            }
            if ((otmp = MON_WEP(mtmp)) && mwelded(otmp) && otmp != gp.propellor
                && mtmp->weapon_check == NO_WEAPON_WANTED)
                gp.propellor = 0;
        }
        /* propellor = obj, propellor to use
         * propellor = &hands_obj, doesn't need a propellor
         * propellor = 0, needed one and didn't have one
         */
        if (gp.propellor != 0) {
            /* Note: cannot use m_carrying for loadstones, since it will
             * always select the first object of a type, and maybe the
             * monster is carrying two but only the first is unthrowable.
             */
            if (rwep[i] != LOADSTONE) {
                /* Don't throw a cursed weapon-in-hand or an artifact */
                if ((otmp = oselect(mtmp, rwep[i])) && !otmp->oartifact
                    && !(otmp == MON_WEP(mtmp) && mwelded(otmp)))
                    return otmp;
            } else
                for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
                    if (otmp->otyp == LOADSTONE && !otmp->cursed)
                        return otmp;
                }
        }
    }

    /* failure */
    return (struct obj *) 0;
}

/* is 'obj' a type of weapon that any monster knows how to throw? */
boolean
monmightthrowwep(struct obj *obj)
{
    short idx;

    for (idx = 0; idx < SIZE(rwep); ++idx)
        if (obj->otyp == rwep[idx])
            return TRUE;
    return FALSE;
}

/* Weapons in order of preference */
static const NEARDATA short hwep[] = {
    CORPSE, /* cockatrice corpse */
    SCYTHE,
    TSURUGI,
    RUNESWORD,
    DWARVISH_MATTOCK,
    TWO_HANDED_SWORD,
    BATTLE_AXE,
    WAR_HAMMER,
    KATANA,
    CRYSKNIFE,
    TRIDENT,
    LONG_SWORD,
    ELVEN_BROADSWORD,
    BROADSWORD,
    SCIMITAR,
    SILVER_SABER,
    MORNING_STAR,
    SILVER_SHORT_SWORD,
    ELVEN_SHORT_SWORD,
    DWARVISH_SHORT_SWORD,
    SHORT_SWORD,
    ORCISH_SHORT_SWORD,
    SILVER_MACE,
    MACE,
    AXE,
    DWARVISH_SPEAR,
    SILVER_SPEAR,
    ELVEN_SPEAR,
    SPEAR,
    ORCISH_SPEAR,
    FLAIL,
    BULLWHIP,
    UNICORN_HORN,
    QUARTERSTAFF,
    WOODEN_STAKE,
    JAVELIN,
    AKLYS,
    CLUB,
    PICK_AXE,
    RUBBER_HOSE,
    SILVER_DAGGER,
    ELVEN_DAGGER,
    DAGGER,
    ORCISH_DAGGER,
    ATHAME,
    SCALPEL,
    KNIFE,
    WORM_TOOTH
};

boolean
would_prefer_hwep(struct monst *mtmp, struct obj *otmp)
{
    struct obj *wep = select_hwep(mtmp);

    int i = 0;

    if (wep) {
        if (wep == otmp)
            return TRUE;
        if (wep->oartifact)
            return FALSE;
        if (is_giant(mtmp->data)) {
            if (wep->otyp == CLUB)
                return FALSE;
            if (otmp->otyp == CLUB)
                return TRUE;
        }
    }

    for (i = 0; i < SIZE(hwep); i++) {
      	if (hwep[i] == CORPSE && !(mtmp->misc_worn_check & W_ARMG))
      	    continue;

        if (wep && wep->otyp == hwep[i]
              && !(otmp->otyp == hwep[i]
              && dmgval(otmp, &gy.youmonst) > dmgval(wep, &gy.youmonst)))
  	        return FALSE;
        if (otmp->otyp == hwep[i])
            return TRUE;
    }
    return FALSE;
}

/* select a hand to hand weapon for the monster */
struct obj *
select_hwep(struct monst *mtmp)
{
    struct obj *otmp;
    int i;
    boolean strong = strongmonst(mtmp->data);
    boolean wearing_shield = (mtmp->misc_worn_check & W_ARMS) != 0;

    /* bracers don't really count as shields */
    if (wearing_shield) {
        for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
            if (otmp->owornmask && is_bracer(otmp))
                wearing_shield = 0;
        }
    }

    /* prefer artifacts to everything else */
    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
        if (otmp->oclass == WEAPON_CLASS && otmp->oartifact
            && touch_artifact(otmp, mtmp)
            && ((strong && !wearing_shield)
                || !objects[otmp->otyp].oc_bimanual))
            return otmp;
    }

    if (is_giant(mtmp->data)) /* giants just love to use clubs */
        Oselect(CLUB);
    else if (mtmp->data == &mons[PM_BALROG] && uwep)
        Oselect(BULLWHIP);

    /* only strong monsters can wield big (esp. long) weapons */
    /* big weapon is basically the same as bimanual */
    /* all monsters can wield the remaining weapons */
    for (i = 0; i < SIZE(hwep); i++) {
        if (hwep[i] == CORPSE && !(mtmp->misc_worn_check & W_ARMG)
            && !(resists_ston(mtmp) || defended(mtmp, AD_STON)))
            continue;
        if (((strong && !wearing_shield) || !objects[hwep[i]].oc_bimanual)
            && (objects[hwep[i]].oc_material != SILVER
                || !mon_hates_silver(mtmp)))
            Oselect(hwep[i]);
    }

    /* failure */
    return (struct obj *) 0;
}

/* Called after polymorphing a monster, robbing it, etc....  Monsters
 * otherwise never unwield stuff on their own.  Might print message.
 */
void
possibly_unwield(struct monst *mon, boolean polyspot)
{
    struct obj *obj, *mw_tmp;

    if (!(mw_tmp = MON_WEP(mon)))
        return;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if (obj == mw_tmp)
            break;
    if (!obj) { /* The weapon was stolen or destroyed */
        MON_NOWEP(mon);
        mon->weapon_check = NEED_WEAPON;
        return;
    }
    if (!attacktype(mon->data, AT_WEAP)) {
        setmnotwielded(mon, mw_tmp);
        mon->weapon_check = NO_WEAPON_WANTED;
        /* if we're going to call distant_name(), do so before extract_self */
        if (cansee(mon->mx, mon->my)) {
            pline_mon(mon, "%s drops %s.", Monnam(mon), distant_name(obj, doname));
            newsym(mon->mx, mon->my);
        }
        obj_extract_self(obj);
        /* might be dropping object into water or lava */
        if (!flooreffects(obj, mon->mx, mon->my, "drop")) {
            if (polyspot)
                bypass_obj(obj);
            place_object(obj, mon->mx, mon->my);
            stackobj(obj);
        }
        return;
    }
    /* The remaining case where there is a change is where a monster
     * is polymorphed into a stronger/weaker monster with a different
     * choice of weapons.  This has no parallel for players.  It can
     * be handled by waiting until mon_wield_item is actually called.
     * Though the monster still wields the wrong weapon until then,
     * this is OK since the player can't see it.  (FIXME: Not okay since
     * probing can reveal it.)
     * Note that if there is no change, setting the check to NEED_WEAPON
     * is harmless.
     * Possible problem: big monster with big cursed weapon gets
     * polymorphed into little monster.  But it's not quite clear how to
     * handle this anyway....
     */
    if (!(mwelded(mw_tmp) && mon->weapon_check == NO_WEAPON_WANTED))
        mon->weapon_check = NEED_WEAPON;
    return;
}

/* Let a monster try to wield a weapon, based on mon->weapon_check.
 * Returns 1 if the monster took time to do it, 0 if it did not.
 */
int
mon_wield_item(struct monst *mon)
{
    struct obj *obj;
    boolean exclaim = TRUE; /* assume mon is planning to attack */

    /* This case actually should never happen */
    if (mon->weapon_check == NO_WEAPON_WANTED)
        return 0;
    switch (mon->weapon_check) {
    case NEED_HTH_WEAPON:
        obj = select_hwep(mon);
        break;
    case NEED_RANGED_WEAPON:
        (void) select_rwep(mon);
        obj = gp.propellor;
        break;
    case NEED_PICK_AXE:
        obj = m_carrying(mon, PICK_AXE);
        /* KMH -- allow other picks */
        if (!obj && !which_armor(mon, W_ARMS))
            obj = m_carrying(mon, DWARVISH_MATTOCK);
        exclaim = FALSE; /* mon is just planning to dig */
        break;
    case NEED_AXE:
        /* currently, only 2 types of axe */
        obj = m_carrying(mon, BATTLE_AXE);
        if (!obj || which_armor(mon, W_ARMS))
            obj = m_carrying(mon, AXE);
        exclaim = FALSE;
        break;
    case NEED_PICK_OR_AXE:
        /* prefer pick for fewer switches on most levels */
        obj = m_carrying(mon, DWARVISH_MATTOCK);
        if (!obj)
            obj = m_carrying(mon, BATTLE_AXE);
        if (!obj || which_armor(mon, W_ARMS)) {
            obj = m_carrying(mon, PICK_AXE);
            if (!obj)
                obj = m_carrying(mon, AXE);
        }
        exclaim = FALSE;
        break;
    default:
        impossible("weapon_check %d for %s?", mon->weapon_check,
                   mon_nam(mon));
        return 0;
    }
    if (obj && obj != &hands_obj) {
        struct obj *mw_tmp = MON_WEP(mon);

        if (mw_tmp && mw_tmp->otyp == obj->otyp) {
            /* already wielding it */
            mon->weapon_check = NEED_WEAPON;
            return 0;
        }
        /* Actually, this isn't necessary--as soon as the monster
         * wields the weapon, the weapon welds itself, so the monster
         * can know it's cursed and needn't even bother trying.
         * Still....
         */
        if (mw_tmp && mwelded(mw_tmp)) {
            if (canseemon(mon)) {
                char welded_buf[BUFSZ];
                const char *mon_hand = mbodypart(mon, HAND);

                if (bimanual(mw_tmp))
                    mon_hand = makeplural(mon_hand);
                Sprintf(welded_buf, "%s welded to %s %s",
                        otense(mw_tmp, "are"), mhis(mon), mon_hand);

                if (obj->otyp == PICK_AXE) {
                    pline("Since %s weapon%s %s,", s_suffix(mon_nam(mon)),
                          plur(mw_tmp->quan), welded_buf);
                    pline("%s cannot wield that %s.", mon_nam(mon),
                          xname(obj));
                } else {
                    pline_mon(mon, "%s tries to wield %s.", Monnam(mon), doname(obj));
                    pline("%s %s!", Yname2(mw_tmp), welded_buf);
                }
                mw_tmp->bknown = 1;
            }
            mon->weapon_check = NO_WEAPON_WANTED;
            return 1;
        }
        mon->mw = obj; /* wield obj */
        setmnotwielded(mon, mw_tmp);
        mon->weapon_check = NEED_WEAPON;
        if (canseemon(mon)) {
            boolean newly_welded;
            const struct throw_and_return_weapon *arw;

            pline_mon(mon, "%s wields %s%c",
                      Monnam(mon), doname(obj),
                      exclaim ? '!' : '.');
            if ((arw = autoreturn_weapon(obj)) != 0 && arw->tethered != 0)
                pline_mon(mon, "%s secures the tether on %s.", Monnam(mon),
                          the(xname(obj)));

            /* 3.6.3: mwelded() predicate expects the object to have its
               W_WEP bit set in owormmask, but the pline here and for
               artifact_light don't want that because they'd have '(weapon
               in hand/claw)' appended; so we set it for the mwelded test
               and then clear it, until finally setting it for good below */
            obj->owornmask |= W_WEP;
            newly_welded = mwelded(obj);
            obj->owornmask &= ~W_WEP;
            if (newly_welded) {
                const char *mon_hand = mbodypart(mon, HAND);

                if (bimanual(obj))
                    mon_hand = makeplural(mon_hand);
                pline("%s %s to %s %s!", Tobjnam(obj, "weld"),
                      is_plural(obj) ? "themselves" : "itself",
                      s_suffix(mon_nam(mon)), mon_hand);
                obj->bknown = 1;
            }
        }
        if (artifact_light(obj) && !obj->lamplit) {
            begin_burn(obj, FALSE);
            if (canseemon(mon))
                pline("%s %s in %s %s!", Tobjnam(obj, "shine"),
                      arti_light_description(obj), s_suffix(mon_nam(mon)),
                      mbodypart(mon, HAND));
            /* 3.6.3: artifact might be getting wielded by invisible monst */
            else if (cansee(mon->mx, mon->my))
                pline("Light begins shining %s.",
                      (mdistu(mon) <= 5 * 5) ? "nearby" : "in the distance");
        }
        obj->owornmask = W_WEP;
        return 1;
    }
    mon->weapon_check = NEED_WEAPON;
    return 0;
}

/* force monster to stop wielding current weapon, if any */
void
mwepgone(struct monst *mon)
{
    struct obj *mwep = MON_WEP(mon);

    if (mwep) {
        setmnotwielded(mon, mwep);
        mon->weapon_check = NEED_WEAPON;
    }
}

/* attack bonus for strength & dexterity */
int
abon(void)
{
    int sbon = 0, dex = ACURR(A_DEX);
    if (Upolyd)
        return (adj_lev(&mons[u.umonnum]) - 3);

#if 0 /* Strength should not give a to-hit bonus;
       * the encumbrance system is already there to address that.*/
    int str = ACURR(A_STR), dex = ACURR(A_DEX);

    if (str < 6)
        sbon = -2;
    else if (str < 8)
        sbon = -1;
    else if (str < 17)
        sbon = 0;
    else if (str <= STR18(50))
        sbon = 1; /* up to 18/50 */
    else if (str < STR18(100))
        sbon = 2;
    else
        sbon = 3;
#endif

    /* Game tuning kludge: make it a bit easier for a low level character to
     * hit */
    sbon += (u.ulevel < 3) ? 1 : 0;
    /* Be a little more generous for NerfHack early game too */
    sbon += (u.ulevel < 6) ? 1 : 0;

    if (dex < 4)
        return (sbon - 3);
    else if (dex < 6)
        return (sbon - 2);
    else if (dex < 8)
        return (sbon - 1);
    else if (dex < 14)
        return sbon;
    else
        return (sbon + dex - 14);
}

/* damage bonus for strength */
int
dbon(void)
{
    int str = ACURR(A_STR);

    if (Upolyd)
        return 0;

    if (str < 6)
        return -1;
    else if (str < 16)
        return 0;
    else if (str < 18)
        return 1;
    else if (str == 18)
        return 2; /* up to 18 */
    else if (str <= STR18(75))
        return 3; /* up to 18/75 */
    else if (str <= STR18(90))
        return 4; /* up to 18/90 */
    else if (str < STR18(100))
        return 5; /* up to 18/99 */
    else
        return 6;
}

/* called when wet_a_towel() or dry_a_towel() is changing a towel's wetness */
staticfn void
finish_towel_change(struct obj *obj, int newspe)
{
    /* towel wetness is always between 0 (dry) and 7, inclusive */
    newspe = min(newspe, 7);
    obj->spe = max(newspe, 0);

    /* if hero is wielding this towel, don't give "you begin bashing with
       your [wet] towel" message if it's wet, do give one if it's dry */
    if (obj == uwep)
        gu.unweapon = !is_wet_towel(obj);

    /* description might change: "towel" vs "moist towel" vs "wet towel" */
    if (carried(obj))
        update_inventory();
}

/* increase a towel's wetness */
void
wet_a_towel(
    struct obj *obj,
    int amt, /* positive: new val; negative: increment by -amt; zero: no-op */
    boolean verbose)
{
    int newspe = (amt <= 0) ? obj->spe - amt : amt;

    /* new state is only reported if it's an increase */
    if (newspe > obj->spe) {
        if (verbose) {
            const char *wetness = (newspe < 3)
                                     ? (!obj->spe ? "damp" : "damper")
                                     : (!obj->spe ? "wet" : "wetter");

            if (carried(obj))
                pline("%s gets %s.", Yobjnam2(obj, (const char *) 0),
                      wetness);
            else if (mcarried(obj) && canseemon(obj->ocarry))
                pline("%s %s gets %s.", s_suffix(Monnam(obj->ocarry)),
                      xname(obj), wetness);
        }
    }

    if (newspe != obj->spe)
        finish_towel_change(obj, newspe);
}

/* decrease a towel's wetness; unlike when wetting, 0 is not a no-op */
void
dry_a_towel(
    struct obj *obj,
    int amt, /* positive or zero: new value; negative: decrement by abs(amt) */
    boolean verbose)
{
    int newspe = (amt < 0) ? obj->spe + amt : amt;

    /* new state is only reported if it's a decrease */
    if (newspe < obj->spe) {
        if (verbose) {
            if (carried(obj))
                pline("%s dries%s.", Yobjnam2(obj, (const char *) 0),
                      !newspe ? " out" : "");
            else if (mcarried(obj) && canseemon(obj->ocarry))
                pline("%s %s dries%s.", s_suffix(Monnam(obj->ocarry)),
                      xname(obj), !newspe ? " out" : "");
        }
    }

    if (newspe != obj->spe)
        finish_towel_change(obj, newspe);
}

/* Express progress of training of a skill as a percentage, where every 100%
 * represents a full level of possible enhancement, e.g. a basic skill that
 * returns 150% for this means it can be advanced to skilled and is 50% of the
 * way to expert. */
static int
skill_training_percent(int skill)
{
    int percent = 0;
    int i;

    if (P_RESTRICTED(skill))
       return 0;

    for (i = P_SKILL(skill); i < P_MAX_SKILL(skill); i++) {
       if (P_ADVANCE(skill) >= practice_needed_to_advance(i)) {
           percent += 100;
       } else {
           int mintrain = (i == P_UNSKILLED) ? 0 :
                          practice_needed_to_advance(i - 1);
           int partial = (P_ADVANCE(skill) - mintrain) * 100 /
                         (practice_needed_to_advance(i) - mintrain);
           percent += min(partial, 100);
           break;
       }
    }

    return percent;
}

/* copy the skill level name into the given buffer */
char *
skill_level_name(int level, char *buf)
{
    const char *ptr;

    switch (level) {
    case P_UNSKILLED:
        ptr = "Unskilled";
        break;
    case P_BASIC:
        ptr = "Basic";
        break;
    case P_SKILLED:
        ptr = "Skilled";
        break;
    case P_EXPERT:
        ptr = "Expert";
        break;
    /* these are for unarmed combat/martial arts only */
    case P_MASTER:
        ptr = "Master";
        break;
    case P_GRAND_MASTER:
        ptr = "Grand Master";
        break;
    default:
        ptr = "Unknown";
        break;
    }
    Strcpy(buf, ptr);
    return buf;
}

const char *
skill_name(int skill)
{
    return P_NAME(skill);
}

/* return the # of slots required to advance the skill */
staticfn int
slots_required(int skill)
{
    int tmp = P_SKILL(skill);

    /* The more difficult the training, the more slots it takes.
     *  unskilled -> basic      1
     *  basic -> skilled        2
     *  skilled -> expert       3
     */
    if (skill <= P_LAST_WEAPON || skill == P_TWO_WEAPON_COMBAT)
        return tmp;

    /* Fewer slots used up for unarmed or martial.
     *  unskilled -> basic      1
     *  basic -> skilled        1
     *  skilled -> expert       2
     *  expert -> master        2
     *  master -> grand master  3
     */
    return (tmp + 1) / 2;
}

/* return true if this skill can be advanced */
boolean
can_advance(int skill, boolean speedy)
{
    if (P_RESTRICTED(skill)
        || P_SKILL(skill) >= P_MAX_SKILL(skill)
        || u.skills_advanced >= P_SKILL_LIMIT)
        return FALSE;

    if (wizard && speedy)
        return TRUE;

    return (boolean) ((int) P_ADVANCE(skill)
                      >= practice_needed_to_advance(P_SKILL(skill))
                      && u.weapon_slots >= slots_required(skill));
}

/* return true if this skill could be advanced if more slots were available */
staticfn boolean
could_advance(int skill)
{
    if (P_RESTRICTED(skill)
        || P_SKILL(skill) >= P_MAX_SKILL(skill)
        || u.skills_advanced >= P_SKILL_LIMIT)
        return FALSE;

    return (boolean) ((int) P_ADVANCE(skill)
                      >= practice_needed_to_advance(P_SKILL(skill)));
}

/* return true if this skill has reached its maximum and there's been enough
   practice to become eligible for the next step if that had been possible */
staticfn boolean
peaked_skill(int skill)
{
    if (P_RESTRICTED(skill))
        return FALSE;

    return (boolean) (P_SKILL(skill) >= P_MAX_SKILL(skill)
                      && ((int) P_ADVANCE(skill)
                          >= practice_needed_to_advance(P_SKILL(skill))));
}

staticfn void
skill_advance(int skill)
{
    u.weapon_slots -= slots_required(skill);
    P_SKILL(skill)++;
    u.skill_record[u.skills_advanced++] = skill;
    if (P_ADVANCE(skill) < practice_needed_to_advance(P_SKILL(skill) - 1)) {
        /* if enhanced through wizard mode, award the points as if you had
         * practiced */
        P_ADVANCE(skill) = practice_needed_to_advance(P_SKILL(skill) - 1);
    }
    /* subtly change the advance message to indicate no more advancement */
    You("are now %s skilled in %s.",
        P_SKILL(skill) >= P_MAX_SKILL(skill) ? "most" : "more",
        P_NAME(skill));

    if (skill == P_SHIELD) /* May affect AC shown for worn shield/bracers */
        update_inventory();

    /* wizards discover spellbook IDs depending on spell 'school' skill limits;
       this allows them to successfully write books for unknown spells without
       the Luck bias they used to have over other roles */
    if (skill >= P_FIRST_SPELL && skill <= P_LAST_SPELL)
        skill_based_spellbook_id();
}

static const struct skill_range {
    short first, last;
    const char *name;
} skill_ranges[] = {
    { P_FIRST_H_TO_H, P_LAST_H_TO_H, "Fighting Skills" },
    { P_FIRST_WEAPON, P_LAST_WEAPON, "Weapon Skills" },
    { P_FIRST_SPELL, P_LAST_SPELL, "Spellcasting Skills" },
};

/*
 * The `#enhance' extended command.  What we _really_ would like is
 * to keep being able to pick things to advance until we couldn't any
 * more.  This is currently not possible -- the menu code has no way
 * to call us back for instant action.  Even if it did, we would also need
 * to be able to update the menu since selecting one item could make
 * others unselectable.
 */
int
enhance_weapon_skill(void)
{
    int pass, i, n, len, longest, to_advance, eventually_advance, maxxed_cnt;
    char buf[BUFSZ], sklnambuf[BUFSZ], maxsklnambuf[BUFSZ], percentbuf[BUFSZ];
    const char *prefix;
    menu_item *selected;
    anything any;
    winid win;
    boolean speedy = FALSE;
    int clr = NO_COLOR;

    /* player knows about #enhance, don't show tip anymore */
    svc.context.tips[TIP_ENHANCE] = TRUE;

    if (wizard && y_n("Advance skills without practice?") == 'y')
        speedy = TRUE;

    do {
        /* find longest available skill name, count those that can advance */
        to_advance = eventually_advance = maxxed_cnt = 0;
        for (longest = 0, i = 0; i < P_NUM_SKILLS; i++) {
            if (P_RESTRICTED(i))
                continue;
            if ((len = Strlen(P_NAME(i))) > longest)
                longest = len;
            if (can_advance(i, speedy))
                to_advance++;
            else if (could_advance(i))
                eventually_advance++;
            else if (peaked_skill(i))
                maxxed_cnt++;
        }

        win = create_nhwindow(NHW_MENU);
        start_menu(win, MENU_BEHAVE_STANDARD);

        /* start with a legend if any entries will be annotated
           with "*" or "#" below */
        if (eventually_advance > 0 || maxxed_cnt > 0) {
            any = cg.zeroany;
            if (eventually_advance > 0) {
                Sprintf(buf, "(Skill%s flagged by \"*\" may be enhanced %s.)",
                        plur(eventually_advance),
                        (u.ulevel < MAXULEV)
                            ? "when you're more experienced"
                            : "if skill slots become available");
                add_menu(win, &nul_glyphinfo, &any, 0, 0,
                         ATR_NONE, clr, buf, MENU_ITEMFLAGS_NONE);
            }
            if (maxxed_cnt > 0) {
                Sprintf(buf,
                 "(Skill%s flagged by \"#\" cannot be enhanced any further.)",
                        plur(maxxed_cnt));
                add_menu(win, &nul_glyphinfo, &any, 0, 0,
                         ATR_NONE, clr, buf, MENU_ITEMFLAGS_NONE);
            }
            add_menu(win, &nul_glyphinfo, &any, 0, 0,
                     ATR_NONE, clr, "", MENU_ITEMFLAGS_NONE);
        }

        /* List the skills, making ones that could be advanced
           selectable.  List the miscellaneous skills first.
           Possible future enhancement:  list spell skills before
           weapon skills for spellcaster roles. */
        for (pass = 0; pass < SIZE(skill_ranges); pass++)
            for (i = skill_ranges[pass].first; i <= skill_ranges[pass].last;
                 i++) {
                /* Print headings for skill types */
                any = cg.zeroany;
                if (i == skill_ranges[pass].first)
                    add_menu_heading(win, skill_ranges[pass].name);

                if (P_RESTRICTED(i))
                    continue;
                /*
                 * Sigh, this assumes a monospaced font unless
                 * iflags.menu_tab_sep is set in which case it puts
                 * tabs between columns.
                 * The 12 is the longest skill level name.
                 * The "    " is room for a selection letter and dash, "a - ".
                 */
                if (can_advance(i, speedy))
                    prefix = ""; /* will be preceded by menu choice */
                else if (could_advance(i))
                    prefix = "  * ";
                else if (peaked_skill(i))
                    prefix = "  # ";
                else
                    prefix =
                        (to_advance + eventually_advance + maxxed_cnt > 0)
                            ? "    "
                            : "";

                (void) skill_level_name(P_SKILL(i), sklnambuf);
                (void) skill_level_name(P_MAX_SKILL(i), maxsklnambuf);

                int percent = skill_training_percent(i);
                Strcpy(percentbuf, "");
                if (percent > 0) {
                    Sprintf(percentbuf, "%d%%", percent);
                }
                if (P_SKILL(i) == P_MAX_SKILL(i)
                    || (P_SKILL(i) + (percent / 100)) == P_MAX_SKILL(i)) {
                    Strcpy(percentbuf, "MAX");
                }

                if (wizard) {
                    if (!iflags.menu_tab_sep)
                        Snprintf(buf, sizeof(buf), " %s%-*s %-12s %5d(%4d)", prefix,
                                 longest, P_NAME(i), sklnambuf, P_ADVANCE(i),
                                 practice_needed_to_advance(P_SKILL(i)));
                    else
                        Snprintf(buf, sizeof(buf), " %s%s\t%s\t%5d(%4d)", prefix, P_NAME(i),
                                 sklnambuf, P_ADVANCE(i),
                                 practice_needed_to_advance(P_SKILL(i)));
                } else {
                    if (!iflags.menu_tab_sep)
                        Snprintf(buf, sizeof(buf), " %s %-*s [%12s / %-12s] %4s",
                                 prefix, longest, P_NAME(i), sklnambuf,
                                 maxsklnambuf, percentbuf);
                    else
                        Snprintf(buf,  sizeof(buf), " %s%s\t[%s\t /%s] %4s",
                                 prefix, P_NAME(i), sklnambuf, maxsklnambuf,
                                 percentbuf);
                }
                any.a_int = can_advance(i, speedy) ? i + 1 : 0;
                add_menu(win, &nul_glyphinfo, &any, 0, 0,
                         ATR_NONE, clr, buf, MENU_ITEMFLAGS_NONE);
            }

        Strcpy(buf, (to_advance > 0) ? "Pick a skill to advance:"
                                     : "Current skills / skill caps:");
        Sprintf(eos(buf), "  (%d slot%s available)", u.weapon_slots,
                    plur(u.weapon_slots));
        end_menu(win, buf);
        n = select_menu(win, to_advance ? PICK_ONE : PICK_NONE, &selected);
        destroy_nhwindow(win);
        if (n > 0) {
            n = selected[0].item.a_int - 1; /* get item selected */
            free((genericptr_t) selected);
            skill_advance(n);
            /* check for more skills able to advance, if so then .. */
            for (n = i = 0; i < P_NUM_SKILLS; i++) {
                if (can_advance(i, speedy)) {
                    if (!speedy)
                        You_feel("you could be more dangerous!");
                    n++;
                    break;
                }
            }
        }
    } while (speedy && n > 0);
    return ECMD_OK;
}

/*
 * Change from restricted to unrestricted, allowing P_BASIC as max.  This
 * function may be called with with P_NONE.  Used in pray.c as well as below.
 */
void
unrestrict_weapon_skill(int skill)
{
    /* From SporkHack (modified):
     * Cavepersons are good at what they know how to use, but not much on advanced
     * fencing or combat tactics. So never unrestrict an edged weapon for them.
     * Same for priests, they shouldn't have edged weapons at all.
     */

    if (Role_if(PM_CAVE_DWELLER) || Role_if(PM_CLERIC)) {
        if (skill >= P_DAGGER && skill <= P_SABER)
            return;
        if (skill >= P_POLEARMS && skill <= P_UNICORN_HORN)
            return;
    }

    if (skill < P_NUM_SKILLS && P_RESTRICTED(skill)) {
        P_SKILL(skill) = P_UNSKILLED;
        P_MAX_SKILL(skill) = P_BASIC;
        P_ADVANCE(skill) = 0;
    }
}

void
use_skill(int skill, int degree)
{
    boolean advance_before;

    if (skill != P_NONE && !P_RESTRICTED(skill)) {
        advance_before = can_advance(skill, FALSE);
        P_ADVANCE(skill) += degree;
        if (!advance_before && can_advance(skill, FALSE))
            give_may_advance_msg(skill);
    }
}

void
add_weapon_skill(int n) /* number of slots to gain; normally one */
{
    int i, before, after;

    for (i = 0, before = 0; i < P_NUM_SKILLS; i++)
        if (can_advance(i, FALSE))
            before++;
    u.weapon_slots += n;
    for (i = 0, after = 0; i < P_NUM_SKILLS; i++)
        if (can_advance(i, FALSE))
            after++;
    if (before < after)
        give_may_advance_msg(P_NONE);
}

void
lose_weapon_skill(int n) /* number of slots to lose; normally one */
{
    int skill;

    while (--n >= 0) {
        /* deduct first from unused slots then from last placed one, if any */
        if (u.weapon_slots) {
            u.weapon_slots--;
        } else if (u.skills_advanced) {
            skill = u.skill_record[--u.skills_advanced];
            if (P_SKILL(skill) <= P_UNSKILLED)
                panic("lose_weapon_skill (%d)", skill);
            P_SKILL(skill)--; /* drop skill one level */
            /* Lost skill might have taken more than one slot; refund rest. */
            u.weapon_slots = slots_required(skill) - 1;
            /* It might now be possible to advance some other pending
               skill by using the refunded slots, but giving a message
               to that effect would seem pretty confusing.... */
        }
    }
}

void
drain_weapon_skill(int n) /* number of skills to drain */
{
    int skill;
    int i;
    int curradv;
    int prevadv;
    int tmpskills[P_NUM_SKILLS];

    (void) memset((genericptr_t) tmpskills, 0, sizeof(tmpskills));

    while (--n >= 0) {
        if (u.skills_advanced) {
            /* Pick a random skill, deleting it from the list. */
            i = rn2(u.skills_advanced);
            skill = u.skill_record[i];
            tmpskills[skill] = 1;
            for (; i < u.skills_advanced - 1; i++) {
                u.skill_record[i] = u.skill_record[i + 1];
            }
            u.skills_advanced--;
            if (P_SKILL(skill) <= P_UNSKILLED)
                panic("drain_weapon_skill (%d)", skill);
            P_SKILL(skill)--;   /* drop skill one level */
            /* refund slots used for skill */
            u.weapon_slots += slots_required(skill);
            /* drain skill training to a value appropriate for new level */
            curradv = practice_needed_to_advance(P_SKILL(skill));
            prevadv = practice_needed_to_advance(P_SKILL(skill) - 1);
            if (P_ADVANCE(skill) >= curradv)
                P_ADVANCE(skill) = prevadv + rn2(curradv - prevadv);
        }
    }

    for (skill = 0; skill < P_NUM_SKILLS; skill++)
        if (tmpskills[skill]) {
            You("forget %syour training in %s.",
                P_SKILL(skill) >= P_BASIC ? "some of " : "", P_NAME(skill));
        }
}

int
weapon_type(struct obj *obj)
{
    /* KMH -- now uses the object table */
    int type;

    if (!obj)
        return P_BARE_HANDED_COMBAT; /* Not using a weapon */
    if (obj->oclass != WEAPON_CLASS && obj->oclass != TOOL_CLASS
        && obj->oclass != GEM_CLASS)
        return P_NONE; /* Not a weapon, weapon-tool, or ammo */
    type = objects[obj->otyp].oc_skill;
    return (type < 0) ? -type : type;
}

int
uwep_skill_type(void)
{
    if (u.twoweap)
        return P_TWO_WEAPON_COMBAT;
    return weapon_type(uwep);
}

/*
 * Return hit bonus/penalty based on skill of weapon.
 * weapon can be null, meaning bare-handed combat.
 * Treat restricted weapons as unskilled.
 */
int
weapon_hit_bonus(struct obj *weapon)
{
    int type, wep_type, skill, bonus = 0;
    static const char bad_skill[] = "weapon_hit_bonus: bad skill %d";

    wep_type = weapon_type(weapon);
    /* use two weapon skill only if attacking with one of the wielded weapons
     */
    type = (u.twoweap && (weapon == uwep || weapon == uswapwep))
               ? P_TWO_WEAPON_COMBAT
               : wep_type;
    if (type == P_NONE) {
        bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
        switch (P_SKILL(type)) {
        default:
            impossible(bad_skill, P_SKILL(type));
            FALLTHROUGH;
            /*FALLTHRU*/
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -4;
            break;
        case P_BASIC:
            bonus = 0;
            break;
        case P_SKILLED:
            bonus = 2;
            break;
        case P_EXPERT:
            bonus = 3;
            break;
        case P_MASTER:
            bonus = 4;
            break;
        }
    } else if (type == P_TWO_WEAPON_COMBAT) {
        skill = P_SKILL(P_TWO_WEAPON_COMBAT);
        if (P_SKILL(wep_type) < skill)
            skill = P_SKILL(wep_type);
        switch (skill) {
        default:
            impossible(bad_skill, skill);
            FALLTHROUGH;
            /*FALLTHRU*/
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -9;
            break;
        case P_BASIC:
            bonus = -7;
            break;
        case P_SKILLED:
            bonus = -5;
            break;
        case P_EXPERT:
            bonus = -3;
            break;
        }
    } else if (type == P_BARE_HANDED_COMBAT) {
        /*
         *        b.h. m.a.
         * unskl:  +1  n/a
         * basic:  +1   +3
         * skild:  +2   +4
         * exprt:  +2   +5
         * mastr:  +3   +6
         * grand:  +3   +7
         */
        bonus = P_SKILL(type);
        bonus = max(bonus, P_UNSKILLED) - 1; /* unskilled => 0 */
        bonus = ((bonus + 2) * (martial_bonus() ? 2 : 1)) / 2;
    }

    /* KMH -- It's harder to hit while you are riding */
    if (u.usteed) {
        switch (P_SKILL(P_RIDING)) {
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus -= 2;
            break;
        case P_BASIC:
            bonus -= 1;
            break;
        case P_SKILLED:
            break;
        case P_EXPERT:
            break;
        }
        if (u.twoweap)
            bonus -= 2;
    }

    return bonus;
}

/*
 * Return damage bonus/penalty based on skill of weapon.
 * weapon can be null, meaning bare-handed combat.
 * Treat restricted weapons as unskilled.
 */
int
weapon_dam_bonus(struct obj *weapon)
{
    int type, wep_type, skill, bonus = 0;

    wep_type = weapon_type(weapon);
    /* use two weapon skill only if attacking with one of the wielded weapons
     */
    type = (u.twoweap && (weapon == uwep || weapon == uswapwep))
               ? P_TWO_WEAPON_COMBAT
               : wep_type;
    if (type == P_NONE) {
        bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
        switch (P_SKILL(type)) {
        default:
            impossible("weapon_dam_bonus: bad skill %d", P_SKILL(type));
            FALLTHROUGH;
            /*FALLTHRU*/
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -2;
            break;
        case P_BASIC:
            bonus = 0;
            break;
        case P_SKILLED:
            bonus = 1;
            break;
        case P_EXPERT:
            bonus = 2;
            break;
        case P_MASTER:
            bonus = 3;
            break;
        }
    } else if (type == P_TWO_WEAPON_COMBAT) {
        skill = P_SKILL(P_TWO_WEAPON_COMBAT);
        if (P_SKILL(wep_type) < skill)
            skill = P_SKILL(wep_type);
        switch (skill) {
        default:
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -3;
            break;
        case P_BASIC:
            bonus = -1;
            break;
        case P_SKILLED:
            bonus = 0;
            break;
        case P_EXPERT:
            bonus = 1;
            break;
        }
    } else if (type == P_BARE_HANDED_COMBAT) {
        /*
         *        b.h. m.a.
         * unskl:   0  n/a
         * basic:  +1   +3
         * skild:  +1   +4
         * exprt:  +2   +6
         * mastr:  +2   +7
         * grand:  +3   +9
         */
        bonus = P_SKILL(type);
        bonus = max(bonus, P_UNSKILLED) - 1; /* unskilled => 0 */
        bonus = ((bonus + 1) * (martial_bonus() ? 3 : 1)) / 2;
    }

    /* KMH -- Riding gives some thrusting damage */
    if (u.usteed && type != P_TWO_WEAPON_COMBAT) {
        switch (P_SKILL(P_RIDING)) {
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            break;
        case P_BASIC:
            break;
        case P_SKILLED:
            bonus += 1;
            break;
        case P_EXPERT:
            bonus += 2;
            break;
        }
    }

    /* add a little damage bonus for higher-level characters so
     * the stronger monsters aren't too overpowered --Amy */
	if (u.ulevel >= 10)
        bonus += 1;
    /* Post level 20 grants very juicy rewards: */
    if (u.ulevel >= 20)
        bonus += rn2(u.ulevel - 20 + 1);

    return bonus;
}

/*
 * Initialize weapon skill array for the game.  Start by setting all
 * skills to restricted, then set the skill for every weapon the
 * hero is holding, finally reading the given array that sets
 * maximums.
 */
void
skill_init(const struct def_skill *class_skill)
{
    struct obj *obj;
    int skmax, skill;

    /* initialize skill array; by default, everything is restricted */
    for (skill = 0; skill < P_NUM_SKILLS; skill++) {
        P_SKILL(skill) = P_ISRESTRICTED;
        P_MAX_SKILL(skill) = P_ISRESTRICTED;
        P_ADVANCE(skill) = 0;
    }

    /* Set skill for all weapons in inventory to be basic */
    for (obj = gi.invent; obj; obj = obj->nobj) {
        /* don't give skill just because of carried ammo, wait until
           we see the relevant launcher (prevents an archeologist's
           touchstone from inadvertently providing skill in sling) */
        if (is_ammo(obj))
            continue;

        skill = weapon_type(obj);
        if (skill != P_NONE)
            P_SKILL(skill) = P_BASIC;
    }

    /* set skills for magic */
    if (Role_if(PM_HEALER)) {
        P_SKILL(P_HEALING_SPELL) = P_BASIC;
    } else if (Role_if(PM_CLERIC)) {
        P_SKILL(P_CLERIC_SPELL) = P_BASIC;
    } else if (Role_if(PM_WIZARD)) {
        P_SKILL(P_ATTACK_SPELL) = P_BASIC;
        P_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
    } else if (Role_if(PM_MONK)) {
        if (carrying(SPE_HEALING))
            P_SKILL(P_HEALING_SPELL) = P_BASIC;
        else if (carrying(SPE_PROTECTION))
            P_SKILL(P_CLERIC_SPELL) = P_BASIC;
        else if (carrying(SPE_CONFUSE_MONSTER) || carrying(SPE_SLEEP))
            P_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
    }

    /* walk through array to set skill maximums */
    for (; class_skill->skill != P_NONE; class_skill++) {
        skmax = class_skill->skmax;
        skill = class_skill->skill;

        P_MAX_SKILL(skill) = skmax;
        if (P_SKILL(skill) == P_ISRESTRICTED) /* skill pre-set */
            P_SKILL(skill) = P_UNSKILLED;
    }

    /* High potential fighters already know how to use their hands. */
    if (P_MAX_SKILL(P_BARE_HANDED_COMBAT) > P_EXPERT)
        P_SKILL(P_BARE_HANDED_COMBAT) = P_BASIC;

    /* Roles that start with a horse know how to ride it */
    if (gu.urole.petnum == PM_PONY)
        P_SKILL(P_RIDING) = P_BASIC;

    /*
     * Make sure we haven't missed setting the max on a skill
     * & set advance
     */
    for (skill = 0; skill < P_NUM_SKILLS; skill++) {
        if (!P_RESTRICTED(skill)) {
            if (P_MAX_SKILL(skill) < P_SKILL(skill)) {
                impossible("skill_init: curr > max: %s", P_NAME(skill));
                P_MAX_SKILL(skill) = P_SKILL(skill);
            }
            P_ADVANCE(skill) = practice_needed_to_advance(P_SKILL(skill) - 1);
        }
    }

    /* Roles that can reach expert or master in shield skill
       already have a basic understanding of how to use them */
    if (Role_if(PM_KNIGHT) || Role_if(PM_VALKYRIE))
        P_SKILL(P_SHIELD) = P_BASIC;

    /* each role has a special spell; allow at least basic for its type
       (despite the function name, this works for spell skills too) */
    unrestrict_weapon_skill(spell_skilltype(gu.urole.spelspec));

    if (!u.uroleplay.pauper) /* paupers lack advanced access to books */
        skill_based_spellbook_id();
}

void
setmnotwielded(struct monst *mon, struct obj *obj)
{
    if (!obj)
        return;
    if (artifact_light(obj) && obj->lamplit) {
        end_burn(obj, FALSE);
        if (canseemon(mon))
            pline("%s in %s %s %s shining.", The(xname(obj)),
                  s_suffix(mon_nam(mon)), mbodypart(mon, HAND),
                  otense(obj, "stop"));
    }
    if (MON_WEP(mon) == obj)
        MON_NOWEP(mon);
    obj->owornmask &= ~W_WEP;
}

#undef PN_BARE_HANDED
#undef PN_RIDING
#undef PN_POLEARMS
#undef PN_SABER
#undef PN_HAMMER
#undef PN_WHIP
#undef PN_ATTACK_SPELL
#undef PN_HEALING_SPELL
#undef PN_DIVINATION_SPELL
#undef PN_ENCHANTMENT_SPELL
#undef PN_CLERIC_SPELL
#undef PN_ESCAPE_SPELL
#undef PN_MATTER_SPELL
#undef AKLYS_LIM

/*weapon.c*/
