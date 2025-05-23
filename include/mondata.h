/* NetHack 3.7	mondata.h	$NHDT-Date: 1703845738 2023/12/29 10:28:58 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.63 $ */
/* Copyright (c) 1989 Mike Threepoint                             */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONDATA_H
#define MONDATA_H

/* The macros in here take a permonst * as an argument */

#define monsndx(ptr) ((ptr)->pmidx)
#define verysmall(ptr) ((ptr)->msize < MZ_SMALL)
#define bigmonst(ptr) ((ptr)->msize >= MZ_LARGE)
#define tiny_groundedmon(ptr) \
    (verysmall(ptr) && !is_flyer(ptr) && !is_swimmer(ptr) && !amphibious(ptr))

#define pm_resistance(ptr, typ) (((ptr)->mresists & (typ)) != 0)

#define immune_poisongas(ptr) ((ptr) == &mons[PM_HEZROU]        \
                               || is_migo(ptr)                  \
                               || (ptr) == &mons[PM_VROCK])

#define can_levitate(mon) \
    ((mon_resistancebits(mon) & MR2_LEVITATE) != 0)
#define can_fly(mon) \
    ((mon_resistancebits(mon) & MR2_FLYING) != 0)
#define can_wwalk(mon) \
    ((mon_resistancebits(mon) & MR2_WATERWALK) != 0)
#define can_jump(mon) \
    ((mon_resistancebits(mon) & MR2_JUMPING) != 0)
#define has_seeinv(mon) \
    ((mon_resistancebits(mon) & MR2_SEE_INVIS) != 0)
#define has_magicbreath(mon) \
    ((mon_resistancebits(mon) & MR2_MAGBREATH) != 0)
#define has_displacement(mon) \
    ((mon_resistancebits(mon) & MR2_DISPLACED) != 0)
#define has_strength(mon) \
    ((mon_resistancebits(mon) & MR2_STRENGTH) != 0)
#define has_telepathy(mon) (telepathic(mon->data) \
     || (mon_resistancebits(mon) & MR2_TELEPATHY) != 0)
#define has_reflection(mon) \
    ((mon_resistancebits(mon) & MR2_REFLECTION) != 0)
#define has_free_action(mon) \
    ((mon_resistancebits(mon) & MR2_FREE_ACTION) != 0)
#define has_phasing(mon) \
    ((mon_resistancebits(mon) & MR2_PHASING) != 0)

#define resists_death(ptr) \
    (dmgtype((ptr), AD_DETH) \
     || nonliving(ptr) \
     || is_demon(ptr) \
     || is_angel(ptr) \
     || is_vampire(ptr) \
     || (ptr)->msound == MS_LEADER \
     || (ptr) == &mons[PM_MINOTAUR] \
     || (ptr) == &mons[PM_ELDER_MINOTAUR] \
     || (ptr) == &mons[PM_TITAN] \
     || ptr == &mons[PM_BARGHEST] \
     || (ptr) == &mons[PM_DEATH])

#define resists_sick(ptr)                      \
    ((ptr)->mlet == S_FUNGUS                   \
     || ((ptr)->mlet == S_ELEMENTAL && (ptr) != &mons[PM_STALKER]) \
     || nonliving(ptr)                         \
     || is_demon(ptr)                          \
     || is_angel(ptr)                          \
     || is_rider(ptr)                          \
     || is_gnoll(ptr)                          \
     || (ptr) == &mons[PM_GIANT_COCKROACH]     \
     || (ptr) == &mons[PM_BABY_GREEN_DRAGON]   \
     || (ptr) == &mons[PM_GREEN_DRAGON]        \
     || (ptr) == &mons[PM_LEPER]               \
     || (ptr) == &mons[PM_LOCUST])

#define resists_stun(ptr) \
    ((ptr) == &mons[PM_BABY_SHIMMERING_DRAGON] \
     || (ptr) == &mons[PM_SHIMMERING_DRAGON])

/* The general guideline is that a mammal-type monster can become rabid. */
#define can_become_rabid(ptr) \
    (((ptr)->mlet == S_DOG \
    || (ptr)->mlet == S_FELINE \
    || (ptr)->mlet == S_HUMANOID \
    || (ptr)->mlet == S_ORC \
    || (ptr)->mlet == S_QUADRUPED \
    || (ptr)->mlet == S_RODENT \
    || (ptr)->mlet == S_UNICORN \
    || (ptr)->mlet == S_ZRUTY \
    || (ptr)->mlet == S_BAT \
    || (ptr)->mlet == S_CENTAUR \
    || (ptr)->mlet == S_GNOME \
    || (ptr)->mlet == S_GIANT \
    || (ptr)->mlet == S_OGRE \
    || (ptr)->mlet == S_TROLL \
    || (ptr)->mlet == S_YETI \
    || (ptr)->mlet == S_HUMAN) \
    && !cannot_be_rabid(ptr))

#define invalid_spellbeing(ptr) \
    (is_rider(ptr) \
    || is_placeholder(ptr) \
    /* Watchmen can get angry from certain actions */ \
    || (ptr) == &mons[PM_WATCHMAN] \
    || (ptr) == &mons[PM_WATCH_CAPTAIN] \
    || (ptr) == &mons[PM_LONG_WORM_TAIL] \
    || (ptr) == &mons[PM_PHOENIX] /* Phoenixes ressurect */ \
    || (ptr)->mlet == S_KOP)  /* Kops can be farmed */

#define cannot_be_rabid(ptr) \
    (is_undead(ptr) \
    || nonliving(ptr) \
    || unique_corpstat(ptr) \
    || is_mind_flayer(ptr) \
    || is_unicorn(ptr) \
    || is_bird(ptr) \
    || is_grung(ptr) \
    || is_dino(ptr) \
    || ptr == &mons[PM_DEEP_ONE] \
    || ptr == &mons[PM_DEEPER_ONE] \
    || ptr == &mons[PM_DEEPEST_ONE] \
    || ptr == &mons[PM_BYAKHEE] \
    || ptr == &mons[PM_NIGHTGAUNT] \
    || ptr == &mons[PM_LANDSHARK] \
    /* These pose complex problems if they become rabid... */ \
    || ptr == &mons[PM_ALIGNED_CLERIC] \
    || ptr == &mons[PM_HIGH_CLERIC] \
    || ptr == &mons[PM_SHOPKEEPER] \
    || ptr == &mons[PM_DOPPELGANGER] \
    || ptr == &mons[PM_GUARD] \
    || ptr == &mons[PM_NURSE] \
    || (ptr)->msound == MS_LEADER)

/* as of 3.2.0:  gray dragons, Angels, Oracle, Yeenoghu */
#define resists_mgc(ptr) \
    (dmgtype(ptr, AD_MAGM) \
     || ptr == &mons[PM_BABY_GRAY_DRAGON] \
     || ptr == &mons[PM_DEEPEST_ONE] \
     || dmgtype(ptr, AD_RBRE)) /* Chromatic Dragon */

#define resists_drain(ptr) \
    (is_undead(ptr) || is_demon(ptr) || is_were(ptr)\
     || is_gnoll(ptr)                               \
     || ptr == &mons[PM_DEEP_ONE]                   \
     || ptr == &mons[PM_DEEPER_ONE]                 \
     || ptr == &mons[PM_DEEPEST_ONE]                \
     || ptr == &mons[PM_SHADOW_OGRE]                \
     || ptr == &mons[PM_BARGHEST]                   \
     || ptr == &mons[PM_DEATH])
/* is_were() doesn't handle hero in human form */

#define immune_mgc_scare(ptr) \
    ((ptr)->mlet == S_HUMAN \
    || (ptr)->mlet == S_ANGEL \
    || is_mplayer(ptr) \
    || is_rider(ptr) \
    || unique_corpstat(ptr) \
    || (ptr) == &mons[PM_GIANT_PRAYING_MANTIS] \
    || (ptr) == &mons[PM_CTHULHU] \
    || (ptr) == &mons[PM_WIZARD_OF_YENDOR])

#define disrespects_elbereth(ptr) \
    ((ptr)->mlet == S_HUMAN      \
    || unique_corpstat(ptr) \
    || is_zombie(ptr) \
    || (ptr) == &mons[PM_MINOTAUR] \
    || (ptr) == &mons[PM_ELDER_MINOTAUR]  \
    || (ptr) == &mons[PM_GIANT_PRAYING_MANTIS])

#define non_tameable(ptr) (unique_corpstat(ptr) \
     || ((ptr)->mflags3 & M3_WANTSARTI) \
     || (ptr->mflags3 & M3_NOTAME))

#define is_jumper(ptr) (((ptr)->mflags3 & M3_JUMPER) != 0L)
#define is_flyer(ptr) (((ptr)->mflags1 & M1_FLY) != 0L)
#define is_floater(ptr) ((ptr)->mlet == S_EYE || (ptr)->mlet == S_LIGHT)

/* clinger: piercers, mimics, wumpus -- generally don't fall down holes */
#define is_clinger(ptr) (((ptr)->mflags1 & M1_CLING) != 0L)
#define grounded(ptr) (!is_flyer(ptr) && !is_floater(ptr) \
                       && (!is_clinger(ptr) || !has_ceiling(&u.uz)))
#define is_swimmer(ptr) (((ptr)->mflags1 & M1_SWIM) != 0L)
#define breathless(ptr) (((ptr)->mflags1 & M1_BREATHLESS) != 0L)
#define amphibious(ptr) (((ptr)->mflags1 & M1_AMPHIBIOUS) != 0L)
#define mon_underwater(mon) \
    (is_swimmer((mon)->data) && is_pool((mon)->mx, (mon)->my))
#define cant_drown(ptr) (is_swimmer(ptr) || amphibious(ptr) || breathless(ptr))
#define passes_walls(ptr) (((ptr)->mflags1 & M1_WALLWALK) != 0L)
#define amorphous(ptr) (((ptr)->mflags1 & M1_AMORPHOUS) != 0L)
#define noncorporeal(ptr) ((ptr)->mlet == S_GHOST)
#define tunnels(ptr) (((ptr)->mflags1 & M1_TUNNEL) != 0L)
#define needspick(ptr) (((ptr)->mflags1 & M1_NEEDPICK) != 0L)
/* hides_under() requires an object at the location in order to hide */
#define hides_under(ptr) (((ptr)->mflags1 & M1_CONCEAL) != 0L)
/* is_hider() is True for mimics but when hiding they appear as something
   else rather than become mon->mundetected, so use is_hider() with care */
#define is_hider(ptr) (((ptr)->mflags1 & M1_HIDE) != 0L)
/* piercers cling to the ceiling; lurkers above are hiders but they fly
   so aren't classified as clingers; unfortunately mimics are classified
   as both hiders and clingers but have nothing to do with ceilings;
   wumpuses (not wumpi :-) cling but aren't hiders */
#define ceiling_hider(ptr) \
    (is_hider(ptr) && ((is_clinger(ptr) && (ptr)->mlet != S_MIMIC) \
                       || is_flyer(ptr))) /* lurker above */
#define haseyes(ptr) (((ptr)->mflags1 & M1_NOEYES) == 0L)
/* used to decide whether plural applies so no need for 'more than 2' */
#define eyecount(ptr) \
    (!haseyes(ptr) ? 0                                                     \
     : ((ptr) == &mons[PM_CYCLOPS] || (ptr) == &mons[PM_FLOATING_EYE]) ? 1 \
       : 2)
#define nohands(ptr) (((ptr)->mflags1 & M1_NOHANDS) != 0L)
#define nolimbs(ptr) (((ptr)->mflags1 & M1_NOLIMBS) == M1_NOLIMBS)
#define notake(ptr) (((ptr)->mflags1 & M1_NOTAKE) != 0L)
#define has_head(ptr) (((ptr)->mflags1 & M1_NOHEAD) == 0L)
#define has_horns(ptr) (num_horns(ptr) > 0)
#define is_whirly(ptr) \
    ((ptr)->mlet == S_VORTEX || (ptr) == &mons[PM_AIR_ELEMENTAL])
#define flaming(ptr)                       \
    ((ptr) == &mons[PM_FIRE_VORTEX]        \
     || (ptr) == &mons[PM_FLAMING_SPHERE]  \
     || (ptr) == &mons[PM_FIRE_ELEMENTAL]  \
     || (ptr) == &mons[PM_FIRE_VAMPIRE]    \
     || (ptr) == &mons[PM_WAX_GOLEM]       \
     || (ptr) == &mons[PM_SALAMANDER]      \
     || (ptr) == &mons[PM_LAVA_BLOB]       \
     || (ptr) == &mons[PM_VULPENFERNO]     \
     || (ptr) == &mons[PM_EYE_OF_FEAR_AND_FLAME] \
     || (ptr) == &mons[PM_LAVA_DEMON])
#define is_silent(ptr) ((ptr)->msound == MS_SILENT)
#define unsolid(ptr) (((ptr)->mflags1 & M1_UNSOLID) != 0L)
#define mindless(ptr) (((ptr)->mflags1 & M1_MINDLESS) != 0L)
#define humanoid(ptr) (((ptr)->mflags1 & M1_HUMANOID) != 0L)
#define is_animal(ptr) (((ptr)->mflags1 & M1_ANIMAL) != 0L)
#define slithy(ptr) (((ptr)->mflags1 & M1_SLITHY) != 0L)
#define is_wooden(ptr) ((ptr) == &mons[PM_WOOD_GOLEM])
#define thick_skinned(ptr) (((ptr)->mflags1 & M1_THICK_HIDE) != 0L)
#define hug_throttles(ptr) ((ptr) == &mons[PM_ROPE_GOLEM])
#define digests(ptr) \
    (dmgtype_fromattack((ptr), AD_DGST, AT_ENGL) != 0) /* purple w*/
#define enfolds(ptr) \
    (dmgtype_fromattack((ptr), AD_WRAP, AT_ENGL) != 0) /* 't' */
#define slimeproof(ptr) \
    ((ptr) == &mons[PM_GREEN_SLIME] || flaming(ptr) || noncorporeal(ptr))
#define lays_eggs(ptr) (((ptr)->mflags1 & M1_OVIPAROUS) != 0L)
#define eggs_in_water(ptr) \
    (lays_eggs(ptr) && (ptr)->mlet == S_EEL && is_swimmer(ptr))
#define regenerates(ptr) (((ptr)->mflags1 & M1_REGEN) != 0L)
#define innate_reflector(ptr) \
    ((ptr) == &mons[PM_SILVER_DRAGON]            \
    || (ptr) == &mons[PM_CHROMATIC_DRAGON])
#define perceives(ptr) (((ptr)->mflags1 & M1_SEE_INVIS) != 0L)
#define can_teleport(ptr) (((ptr)->mflags1 & M1_TPORT) != 0L)
#define control_teleport(ptr) (((ptr)->mflags1 & M1_TPORT_CNTRL) != 0L)
#define telepathic(ptr)                         \
    ((ptr) == &mons[PM_FLOATING_EYE]            \
     || (ptr) == &mons[PM_MIND_FLAYER]          \
     || (ptr) == &mons[PM_MASTER_MIND_FLAYER]   \
     || (ptr) == &mons[PM_SHAMAN_KARNOV]        \
     || (ptr) == &mons[PM_ORACLE]               \
     || (ptr) == &mons[PM_FIRE_VAMPIRE]         \
     || (ptr) == &mons[PM_STAR_VAMPIRE]         \
     || (ptr) == &mons[PM_WIZARD_OF_YENDOR]     \
     || (ptr) == &mons[PM_CTHULHU])
#define is_armed(ptr) attacktype(ptr, AT_WEAP)
#define acidic(ptr) (((ptr)->mflags1 & M1_ACID) != 0L)
#define poisonous(ptr) (((ptr)->mflags1 & M1_POIS) != 0L)
#define carnivorous(ptr) (((ptr)->mflags1 & M1_CARNIVORE) != 0L)
#define herbivorous(ptr) (((ptr)->mflags1 & M1_HERBIVORE) != 0L)
#define metallivorous(ptr) (((ptr)->mflags1 & M1_METALLIVORE) != 0L)
#define lithivorous(ptr) ((ptr) == &mons[PM_ROCK_MOLE])
#define inediate(ptr) (((ptr)->mflags1 & \
                        (M1_CARNIVORE | M1_HERBIVORE | M1_METALLIVORE)) == 0L)
#define polyok(ptr) (((ptr)->mflags2 & M2_NOPOLY) == 0L)
#define is_outflanker(ptr) (((ptr)->mflags2 & M2_FLANK) != 0)
#define follows_you(ptr) (((ptr)->mflags2 & M2_STALK) != 0)
#define is_shapeshifter(ptr) (((ptr)->mflags2 & M2_SHAPESHIFTER) != 0L)
#define is_undead(ptr) (((ptr)->mhflags & MH_UNDEAD) != 0L)
#define is_were(ptr) (((ptr)->mhflags & MH_WERE) != 0L)
#define is_elf(ptr) (((ptr)->mhflags & MH_ELF) != 0L)
#define is_dwarf(ptr) (((ptr)->mhflags & MH_DWARF) != 0L)
#define is_gnome(ptr) (((ptr)->mhflags & MH_GNOME) != 0L)
#define is_orc(ptr) (((ptr)->mhflags & MH_ORC) != 0L)
#define is_human(ptr) (((ptr)->mhflags & MH_HUMAN) != 0L)
#define is_troll(ptr) (((ptr)->mhflags & MH_TROLL) != 0L)
#define is_ogre(ptr) (((ptr)->mhflags & MH_OGRE) != 0L)
#define is_boomer(ptr)                          \
    (  (ptr) == PM_FREEZING_SPHERE              \
    || (ptr) == PM_GAS_SPORE                    \
    || (ptr) == PM_VOLATILE_MUSHROOM               \
    || (ptr) == PM_STINKING_SPHERE              \
    || (ptr) == PM_FLAMING_SPHERE               \
    || (ptr) == PM_SHOCKING_SPHERE              \
    || (ptr) == PM_ACID_SPHERE)
#define your_race(ptr) (((ptr)->mhflags & gu.urace.selfmask) != 0L)
#define is_bat(ptr)                                         \
    ((ptr) == &mons[PM_BAT] || (ptr) == &mons[PM_GIANT_BAT] \
     || (ptr) == &mons[PM_ZOO_BAT] || (ptr) == &mons[PM_VAMPIRE_BAT] \
     || (ptr) == &mons[PM_ATHOL])
#define is_bird(ptr) ((ptr)->mlet == S_BAT && !is_bat(ptr))
/* Jabberwocks not considered to have beaks because they have "jaws". */
#define has_beak(ptr) (is_bird(ptr) || (ptr) == &mons[PM_TENGU] \
                       || (ptr) == &mons[PM_PHOENIX] \
                       || (ptr) == &mons[PM_FELL_BEAST] \
                       || (ptr) == &mons[PM_VROCK])
#define is_migo(ptr) \
    ((ptr) == &mons[PM_MIGO_DRONE] \
     || (ptr) == &mons[PM_MIGO_WARRIOR] \
     || (ptr) == &mons[PM_MIGO_QUEEN])
#define is_dino(ptr) \
    ((ptr) == &mons[PM_VELOCIRAPTOR] \
    || (ptr) == &mons[PM_COMPSOGNATHUS] \
    || (ptr) == &mons[PM_T_REX])
#define is_giant(ptr) (((ptr)->mhflags & MH_GIANT) != 0L)
#define is_ettin(ptr) \
    ((ptr) == &mons[PM_ETTIN] || (ptr) == &mons[PM_ETTIN_MUMMY] \
     || (ptr) == &mons[PM_ETTIN_ZOMBIE])
#define is_golem(ptr) ((ptr)->mlet == S_GOLEM)
#define is_zombie(ptr) \
    ((ptr)->mlet == S_ZOMBIE && !is_not_zombie(ptr))
#define is_not_zombie(ptr) \
    (is_ghoul(ptr) \
    || (ptr) == &mons[PM_SKELETON] || (ptr) == &mons[PM_GUG])
#define is_ghoul(ptr) ((ptr) == &mons[PM_GHOUL] \
                        || (ptr)== &mons[PM_GHOUL_MAGE] \
                        || (ptr)== &mons[PM_GHOUL_QUEEN])
#define is_gnoll(ptr) ((ptr) == &mons[PM_GNOLL] \
    || (ptr)== &mons[PM_GNOLL_WARRIOR] \
    || (ptr)== &mons[PM_GNOLL_CHIEFTAIN] \
    || (ptr)== &mons[PM_GNOLL_SHAMAN])
#define is_domestic(ptr) (((ptr)->mflags2 & M2_DOMESTIC) != 0L)
#define is_demon(ptr) (((ptr)->mhflags & MH_DEMON) != 0L)
#define is_angel(ptr) (((ptr)->mhflags & MH_ANGEL) != 0L)
#define is_dragon(ptr) (((ptr)->mhflags & MH_DRAGON) != 0L)
#define is_feline(ptr) ((ptr)->mlet == S_FELINE)
#define is_mercenary(ptr) (((ptr)->mflags2 & M2_MERC) != 0L)
#define is_male(ptr) (((ptr)->mflags2 & M2_MALE) != 0L)
#define is_female(ptr) (((ptr)->mflags2 & M2_FEMALE) != 0L)
#define is_neuter(ptr) (((ptr)->mflags2 & M2_NEUTER) != 0L)
#define is_wanderer(ptr) (((ptr)->mflags2 & M2_WANDER) != 0L)
#define always_hostile(ptr) (((ptr)->mflags2 & M2_HOSTILE) != 0L)
#define always_peaceful(ptr) (((ptr)->mflags2 & M2_PEACEFUL) != 0L)
#define hostile_to_orcs(ptr) \
    ((ptr) == &mons[PM_HEDGEHOG] \
    || (ptr) == &mons[PM_FAMILIAR] \
    || (ptr) == &mons[PM_PRISONER])
#define race_hostile(ptr) (((ptr)->mhflags & gu.urace.hatemask) != 0L)
#define race_peaceful(ptr) (((ptr)->mhflags & gu.urace.lovemask) != 0L)
#define extra_nasty(ptr) (((ptr)->mflags2 & M2_NASTY) != 0L)
#define strongmonst(ptr) (((ptr)->mflags2 & M2_STRONG) != 0L)
#define can_breathe(ptr) attacktype(ptr, AT_BREA)
#define cantwield(ptr) (nohands(ptr) || verysmall(ptr))
/* Does this type of monster have multiple weapon attacks?  If so,
   hero poly'd into this form can use two-weapon combat.  It used
   to just check mattk[1] and assume mattk[0], which was suitable
   for mons[] at the time but somewhat fragile.  This is more robust
   without going to the extreme of checking all six slots. */
#define could_twoweap(ptr) \
    ((  ((ptr)->mattk[0].aatyp == AT_WEAP)              \
      + ((ptr)->mattk[1].aatyp == AT_WEAP)              \
      + ((ptr)->mattk[2].aatyp == AT_WEAP)  ) > 1)
#define cantweararm(ptr) (breakarm(ptr) || sliparm(ptr))
#define throws_rocks(ptr) (((ptr)->mflags2 & M2_ROCKTHROW) != 0L)
#define type_is_pname(ptr) (((ptr)->mflags2 & M2_PNAME) != 0L)
#define is_lord(ptr) (((ptr)->mflags2 & M2_LORD) != 0L)
#define is_prince(ptr) (((ptr)->mflags2 & M2_PRINCE) != 0L)
#define is_ndemon(ptr) \
    (is_demon(ptr) && (((ptr)->mflags2 & (M2_LORD | M2_PRINCE)) == 0L))
#define is_dlord(ptr) (is_demon(ptr) && is_lord(ptr))
#define is_dprince(ptr) (is_demon(ptr) && is_prince(ptr))
#define is_minion(ptr) (((ptr)->mflags2 & M2_MINION) != 0L)
#define likes_gold(ptr) (((ptr)->mflags2 & M2_GREEDY) != 0L)
#define likes_gems(ptr) (((ptr)->mflags2 & M2_JEWELS) != 0L)
#define likes_objs(ptr) (((ptr)->mflags2 & M2_COLLECT) != 0L || is_armed(ptr))
#define likes_magic(ptr) (((ptr)->mflags2 & M2_MAGIC) != 0L)
#define webmaker(ptr) \
    ((ptr) == &mons[PM_CAVE_SPIDER] \
     || (ptr) == &mons[PM_PHASE_SPIDER] \
     || (ptr) == &mons[PM_RECLUSE_SPIDER] \
     || (ptr) == &mons[PM_WERESPIDER] \
     || (ptr) == &mons[PM_MONSTROUS_SPIDER] \
     || (ptr) == &mons[PM_GIANT_SPIDER])
#define is_unicorn(ptr) ((ptr)->mlet == S_UNICORN && likes_gems(ptr))
#define is_longworm(ptr)                                                   \
    (((ptr) == &mons[PM_BABY_LONG_WORM]) || ((ptr) == &mons[PM_LONG_WORM]) \
     || ((ptr) == &mons[PM_LONG_WORM_TAIL]))
#define is_covetous(ptr) (((ptr)->mflags3 & M3_COVETOUS))
#define is_accurate(ptr) (((ptr)->mflags3 & M3_ACCURATE))
#define is_berserker(ptr) (((ptr)->mflags3 & M3_BERSERK))
#define infravision(ptr) (((ptr)->mflags3 & M3_INFRAVISION))
#define infravisible(ptr) (((ptr)->mflags3 & M3_INFRAVISIBLE))
#define is_displacer(ptr) (((ptr)->mflags3 & M3_DISPLACES) != 0L)
#define is_displaced(ptr) \
    ((ptr) == &mons[PM_DISPLACER_BEAST] \
    || (ptr) == &mons[PM_VULPENFERNO] \
    || (ptr) == &mons[PM_BABY_SHIMMERING_DRAGON] \
    || (ptr) == &mons[PM_SHIMMERING_DRAGON])
#define is_mplayer(ptr) \
    (((ptr) >= &mons[PM_ARCHEOLOGIST]) && ((ptr) <= &mons[PM_WIZARD]))
#define is_watch(ptr) \
    ((ptr) == &mons[PM_WATCHMAN] || (ptr) == &mons[PM_WATCH_CAPTAIN])
#define is_rider(ptr)                                      \
    ((ptr) == &mons[PM_DEATH] || (ptr) == &mons[PM_FAMINE] \
     || (ptr) == &mons[PM_PESTILENCE])
/* note: placeholder monsters are used for corpses of zombies and mummies;
   PM_DWARF and PM_GNOME are normal monsters, not placeholders */
#define is_placeholder(ptr)                             \
    ((ptr) == &mons[PM_ORC] || (ptr) == &mons[PM_GIANT] \
     || (ptr) == &mons[PM_GRUNG] \
     || (ptr) == &mons[PM_ELF] || (ptr) == &mons[PM_HUMAN])

#define can_eat_grass(ptr) \
    (((ptr)->mlet == S_UNICORN && !is_undead(ptr)) \
     || (ptr) == &mons[PM_ROTHE] \
     || (ptr) == &mons[PM_TITANOTHERE]             \
     || (ptr) == &mons[PM_BALUCHITHERIUM]          \
     || (ptr) == &mons[PM_MUMAK]                   \
     || (ptr) == &mons[PM_MASTODON]                \
     || (ptr) == &mons[PM_WOODCHUCK]               \
     || (ptr) == &mons[PM_GELATINOUS_CUBE])

/* Is an iron piercer really made of iron?? */
#define can_corrode(ptr) \
((ptr) == &mons[PM_IRON_GOLEM] || (ptr) == &mons[PM_IRON_PIERCER])

/* various monsters move faster underwater vs on land */
#define is_fast_underwater(ptr)         \
    ((ptr) == &mons[PM_WATER_TROLL]     \
    || (ptr) == &mons[PM_WATER_HULK]    \
    || (ptr) == &mons[PM_DEEP_ONE]    \
    || (ptr) == &mons[PM_DEEPER_ONE]    \
    || (ptr) == &mons[PM_DEEPEST_ONE]    \
    || (ptr) == &mons[PM_BABY_CROCODILE]\
    || (ptr) == &mons[PM_CROCODILE])

/* return TRUE if the monster tends to revive */
#define is_reviver(ptr) (is_rider(ptr) || is_zombie(ptr) \
                         || (ptr) == &mons[PM_CTHULHU] \
                         || (ptr)->mlet == S_TROLL)
/* monsters whose corpses and statues need special handling;
   note that high priests and the Wizard of Yendor are flagged
   as unique even though they really aren't; that's ok here */
#define unique_corpstat(ptr) (((ptr)->geno & G_UNIQ) != 0)

/* this returns the light's range, or 0 if none; if we add more light emitting
   monsters, we'll likely have to add a new light range field to mons[] */
#define emits_light(ptr)                                          \
    (((ptr)->mlet == S_LIGHT || (ptr) == &mons[PM_FLAMING_SPHERE] \
      || (ptr) == &mons[PM_SHOCKING_SPHERE]                       \
      || (ptr) == &mons[PM_BABY_GOLD_DRAGON]                      \
      || (ptr) == &mons[PM_GLOWING_EYE]                           \
      || (ptr) == &mons[PM_WAX_GOLEM]                             \
      || (ptr) == &mons[PM_FIRE_VAMPIRE]                          \
      || (ptr) == &mons[PM_FIRE_VORTEX]                           \
      || (ptr) == &mons[PM_VULPENFERNO])                          \
         ? 1                                                      \
         : ((ptr) == &mons[PM_FIRE_ELEMENTAL]                     \
            || (ptr) == &mons[PM_GOLD_DRAGON]) ? 1 : 0)
    /* [Note: the light ranges above were reduced to 1 for performance,
     *  otherwise screen updating on the plane of fire slowed to a crawl.
     *  Note too: that was with 1990s hardware and before fumarole smoke
     *  blocking line of sight was added, so might no longer be necessary.] */
#define likes_lava(ptr) \
    (ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_SALAMANDER] \
    || (ptr) == &mons[PM_LAVA_BLOB] || (ptr) == &mons[PM_LAVA_DEMON])
#define pm_invisible(ptr) \
    ((ptr) == &mons[PM_STALKER] || (ptr) == &mons[PM_BLACK_LIGHT] \
     || (ptr) == &mons[PM_STAR_VAMPIRE])

/* could probably add more */
#define likes_fire(ptr)                                                  \
    ((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FLAMING_SPHERE] \
     || (ptr) == &mons[PM_FIRE_VAMPIRE] || (ptr) == &mons[PM_PHOENIX]    \
     || (ptr) == &mons[PM_VULPENFERNO] || likes_lava(ptr)                \
      || (ptr) == &mons[PM_EYE_OF_FEAR_AND_FLAME])

#define touch_petrifies(ptr) \
    ((ptr) == &mons[PM_COCKATRICE] \
    || (ptr) == &mons[PM_CHICKATRICE] \
    || (ptr) == &mons[PM_ASPHYNX] \
    || (ptr) == &mons[PM_BASILISK])
/* Medusa doesn't pass touch_petrifies() but does petrify if eaten */
#define flesh_petrifies(pm) (touch_petrifies(pm) || (pm) == &mons[PM_MEDUSA])

/* missiles made of rocks don't harm these: xorns and earth elementals
   (but not ghosts and shades because that would impact all missile use
   and also require an exception for blessed rocks/gems/boulders) */
#define passes_rocks(ptr) (passes_walls(ptr) && !unsolid(ptr))

#define shadelike(ptr) \
    ((ptr) == &mons[PM_SHADE] || (ptr) == &mons[PM_SHADOW] \
    || (ptr) == &mons[PM_FIRST_EVIL])

#define is_mind_flayer(ptr) \
    ((ptr) == &mons[PM_MIND_FLAYER] || (ptr) == &mons[PM_MASTER_MIND_FLAYER])

#define is_vampire(ptr) ((ptr)->mlet == S_VAMPIRE  \
                     || ((ptr) == gy.youmonst.data &&       \
                         !Upolyd && Race_if(PM_DHAMPIR)))
#define is_grung(ptr) ((ptr)->mlet == S_GRUNG)
#define is_hulk(ptr) ((ptr)->mlet == S_UMBER                \
            && (ptr) != &mons[PM_FIRE_VAMPIRE]              \
            && (ptr) != &mons[PM_STAR_VAMPIRE])

/* For vampires */
#define has_blood(ptr)	(!vegetarian(ptr) && \
			((ptr)->mlet != S_GOLEM || (ptr) == &mons[PM_FLESH_GOLEM]) && \
				   (!is_undead(ptr) || is_vampire(ptr)))

#define hates_light(ptr) ((ptr) == &mons[PM_GREMLIN] \
                          || (ptr) == &mons[PM_SHADOW_OGRE])

/* used to vary a few messages */
#define weirdnonliving(ptr) (is_golem(ptr) || (ptr)->mlet == S_VORTEX)
#define nonliving(ptr) \
    (is_undead(ptr) || (ptr) == &mons[PM_MANES] || weirdnonliving(ptr))

/* no corpse (ie, blank scrolls) if killed by fire; special case instakill  */
#define completelyburns(ptr) \
    ((ptr) == &mons[PM_PAPER_GOLEM] || (ptr) == &mons[PM_STRAW_GOLEM] \
    || (ptr) == &mons[PM_WAX_GOLEM])
#define completelyrots(ptr) \
    ((ptr) == &mons[PM_WOOD_GOLEM] || (ptr) == &mons[PM_LEATHER_GOLEM])
#define completelyrusts(ptr) ((ptr) == &mons[PM_IRON_GOLEM])

/* Used for conduct with corpses, tins, and digestion attacks */
/* G_NOCORPSE monsters might still be swallowed as a purple worm */
/* Maybe someday this could be in mflags... */
#define vegan(ptr)                                                 \
    ((ptr)->mlet == S_BLOB || (ptr)->mlet == S_JELLY               \
     || (ptr)->mlet == S_FUNGUS || (ptr)->mlet == S_VORTEX         \
     || (ptr)->mlet == S_LIGHT                                     \
     || ((ptr)->mlet == S_ELEMENTAL && (ptr) != &mons[PM_STALKER]) \
     || ((ptr)->mlet == S_GOLEM && (ptr) != &mons[PM_FLESH_GOLEM]  \
         && (ptr) != &mons[PM_LEATHER_GOLEM]) || noncorporeal(ptr))
#define vegetarian(ptr) \
    (vegan(ptr)         \
     || ((ptr)->mlet == S_PUDDING \
        && ((ptr) != &mons[PM_BLACK_PUDDING] \
        && ((ptr) != &mons[PM_SHOGGOTH]) \
        && (ptr) != &mons[PM_LIKE_LIKE])))
/* jello-like creatures */
#define can_flollop(ptr) \
    ((ptr)->mlet == S_BLOB || (ptr)->mlet == S_JELLY \
     || (ptr)->mlet == S_PUDDING)

#define corpse_eater(ptr)                    \
    (ptr == &mons[PM_PURPLE_WORM]            \
     || ptr == &mons[PM_BABY_PURPLE_WORM]    \
     || is_ghoul(ptr)                        \
     || ptr == &mons[PM_PIRANHA])

/* monkeys are tamable via bananas but not pacifiable via food,
   otherwise their theft attack could be nullified too easily;
   dogs and cats can be tamed by anything they like to eat and are
   pacified by any other food;
   horses can be tamed by always-veggy food or lichen corpses but
   not tamed or pacified by other corpses or tins of veggy critters */
#define befriend_with_obj(ptr, obj) \
    (((ptr) == &mons[PM_MONKEY] || (ptr) == &mons[PM_APE])               \
     ? (obj)->otyp == BANANA                                             \
     : ((is_domestic(ptr)                                                \
            || (((ptr) == &mons[PM_WINTER_WOLF]                          \
                || (ptr) == &mons[PM_WINTER_WOLF_CUB])                   \
                && Role_if(PM_VALKYRIE))                                 \
            || (tameable_by_orc(ptr) && Race_if(PM_ORC)))                 \
        && (obj)->oclass == FOOD_CLASS                                   \
        && ((ptr)->mlet != S_UNICORN                                     \
            || objects[(obj)->otyp].oc_material == VEGGY                 \
            || ((obj)->otyp == CORPSE && (obj)->corpsenm == PM_LICHEN))))

#define is_bones_monster(ptr) \
    ((ptr) == &mons[PM_GHOST] || (ptr) == &mons[PM_GHOUL]        \
     || (ptr) == &mons[PM_VAMPIRE]  || (ptr) == &mons[PM_VAMPIRE_MAGE] \
     || (ptr) == &mons[PM_WRAITH] || (ptr) == &mons[PM_GREEN_SLIME]    \
     || (ptr)->mlet == S_MUMMY)

#ifdef PMNAME_MACROS
#define pmname(ptr,g) ((((g) == MALE || (g) == FEMALE) && (ptr)->pmnames[g]) \
                        ? (ptr)->pmnames[g] : (ptr)->pmnames[NEUTRAL])
#endif
#define monsym(ptr) (def_monsyms[(int) (ptr)->mlet].sym)

#define is_immobile(ptr) ((ptr)->mmove == 0)

/* The monster prefers to keep its distance rather than charging and engaging
 * you in combat. This was adapted from the M3_SKITTISH flag in SporkHack;
 * however, since only a couple monsters have this behavior and they're all in
 * the same class, it isn't necessary to add a M3 flag. */
#define keeps_distance(ptr) ((ptr) == &mons[PM_BLINKING_EYE] \
                             || (ptr) == &mons[PM_KOBOLD_SHAMAN] \
                             || (ptr) == &mons[PM_ORC_SHAMAN] \
                             || (ptr) == &mons[PM_ORB_WEAVER] \
                             || (ptr) == &mons[PM_ALCHEMIST])

#define DEAD_UNICORNS ((int) svm.mvitals[PM_WHITE_UNICORN].died \
                        + (int) svm.mvitals[PM_GRAY_UNICORN].died \
                        + (int) svm.mvitals[PM_BLACK_UNICORN].died)

/* instantly eats any organic object it comes into contact with */
#define is_bigeater(ptr) \
    ((ptr) == &mons[PM_GELATINOUS_CUBE] || (ptr) == &mons[PM_SHOGGOTH] \
    || (ptr) == &mons[PM_HUNGER_HULK])

/* Corpse intrinsic stats */
#define MAX_GAIN 25
#define MIN_GAIN 2
#define MAX_PARTIAL (Race_if(PM_DHAMPIR) ? 50 : 100)
#define percent_granted(ptr) ((ptr)->cwt / 90)

#define HYDRATION_MAX 6000

/* The monster is covetous, but should not warp, heal, or otherwise use
 * tactics() after an introduction. */
#define is_archfiend(ptr) (is_dlord(ptr) || is_dprince(ptr) \
    || (ptr)->msound == MS_NEMESIS)
#define covetous_nonwarper(ptr) (is_archfiend(ptr) \
                                 || (ptr) == &mons[PM_CTHULHU])
/* many boss-type monsters than have two or more spell attacks
   per turn are never able to fire off their second attack due
   to mspec always being greater than 0. So set to 0 for those
   types of monsters, either sometimes or all of the time
   depending on how powerful they are or what their role is */
#define power_caster(ptr) \
    (is_dlord(ptr) \
    || (ptr)->msound == MS_LEADER \
    || (ptr)->msound == MS_NEMESIS \
    || (ptr) == &mons[PM_GHOUL_QUEEN] \
    || (ptr) == &mons[PM_ELVEN_CLERIC] \
    || (ptr) == &mons[PM_HIGH_CLERIC])

#define tameable_by_orc(ptr) \
    ((ptr) == &mons[PM_WARG] || (ptr) == &mons[PM_WARG_PUP] \
    || (ptr) == &mons[PM_BARGHEST] || (ptr) == &mons[PM_GOBLIN] \
    || (ptr) == &mons[PM_TROLL] || (ptr) == &mons[PM_OGRE])

#endif /* MONDATA_H */
