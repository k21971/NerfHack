/* NetHack 3.7	muse.c	$NHDT-Date: 1737392015 2025/01/20 08:53:35 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.234 $ */
/*      Copyright (C) 1990 by Ken Arromdee                         */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * Monster item usage routines.
 */

#include "hack.h"

/* Let monsters use magic items.  Arbitrary assumptions: Monsters only use
 * scrolls when they can see, monsters know when wands have 0 charges,
 * monsters cannot recognize if items are cursed are not, monsters which
 * are confused don't know not to read scrolls, etc....
 */

staticfn int precheck(struct monst *, struct obj *);
staticfn boolean mzapwand(struct monst *, struct obj *, boolean) NONNULLPTRS;
staticfn void mplayhorn(struct monst *, struct obj *, boolean) NONNULLPTRS;
staticfn void mreadmsg(struct monst *, struct obj *) NONNULLPTRS;
staticfn void mquaffmsg(struct monst *, struct obj *) NONNULLPTRS;
staticfn boolean m_use_healing(struct monst *);
staticfn boolean m_sees_sleepy_soldier(struct monst *);
staticfn void m_tele(struct monst *, boolean, boolean, int);
staticfn boolean m_next2m(struct monst *);
staticfn void reveal_trap(struct trap *, boolean);
staticfn int mon_escape(struct monst *, boolean);
staticfn boolean linedup_chk_corpse(coordxy, coordxy);
staticfn void m_use_undead_turning(struct monst *, struct obj *);
staticfn boolean hero_behind_chokepoint(struct monst *);
staticfn boolean mon_has_friends(struct monst *);
staticfn int mbhitm(struct monst *, struct obj *);
staticfn boolean fhito_loc(struct obj *obj, coordxy x, coordxy y,
                           int (*fhito)(OBJ_P, OBJ_P));
staticfn void mbhit(struct monst *, int, int (*)(MONST_P, OBJ_P),
                  int (*)(OBJ_P, OBJ_P), struct obj *);
staticfn struct permonst *muse_newcham_mon(struct monst *);
staticfn int mloot_container(struct monst *mon, struct obj *, boolean);
staticfn void you_aggravate(struct monst *);
#if 0
staticfn boolean necrophiliac(struct obj *, boolean);
#endif
staticfn void mon_consume_unstone(struct monst *, struct obj *, boolean,
                                boolean);
staticfn boolean mcould_eat_tin(struct monst *);
staticfn boolean muse_unslime(struct monst *, struct obj *, struct trap *,
                            boolean);
staticfn int cures_sliming(struct monst *, struct obj *);
staticfn boolean green_mon(struct monst *);
staticfn int muse_wonder(void);
staticfn boolean reflectable_offense(void);
staticfn int muse_createmonster(struct monst *, struct obj *);


/* Defines for various types of stuff.  The order in which monsters prefer
 * to use them is determined by the order of the code logic, not the
 * numerical order in which they are defined.
 */

/* Dungeon feature*/
#define MUSE_TRAPDOOR               1 /* Defensive */
#define MUSE_TELEPORT_TRAP          2 /* Defensive */
#define MUSE_UPSTAIRS               3 /* Defensive */
#define MUSE_DOWNSTAIRS             4 /* Defensive */
#define MUSE_UP_LADDER              5 /* Defensive */
#define MUSE_DN_LADDER              6 /* Defensive */
#define MUSE_SSTAIRS                7 /* Defensive */
#define MUSE_POLY_TRAP              8

/* Wands */

#define MUSE_WAN_MAGIC_MISSILE      101 /* Offensive */
#define MUSE_WAN_FIRE               102 /* Offensive */
#define MUSE_WAN_COLD               103 /* Offensive */
#define MUSE_WAN_SLEEP              104 /* Offensive */
#define MUSE_WAN_DEATH              105 /* Offensive */
#define MUSE_WAN_LIGHTNING          106 /* Offensive */
#define MUSE_WAN_POISON_GAS         107 /* Offensive */
#define MUSE_WAN_CORROSION          108 /* Offensive */
#define MUSE_WAN_DRAINING           109 /* Offensive */
#define MUSE_WAN_STUNNING           110 /* Offensive */
    /* Wand of stunning is not a real wand,
     * the effect is for wands of wonder */
#define MUSE_WAN_CREATE_MONSTER     111 /* Both def & off */
#define MUSE_WAN_WONDER             112 /* Offensive */
#define MUSE_WAN_STRIKING           113 /* Offensive */
#define MUSE_WAN_SLOW_MONSTER       114 /* Offensive */
#define MUSE_WAN_UNDEAD_TURNING     115 /* Both def & off */
#define MUSE_WAN_POLYMORPH          116 /* Both def & off */
#define MUSE_WAN_CANCELLATION       117 /* Offensive */
#define MUSE_WAN_TELEPORTATION      118 //* Both def & off */
#define MUSE_WAN_TELEPORTATION_SELF 119 /* Defensive */
#define MUSE_WAN_DIGGING            120 /* Defensive */
#define MUSE_WAN_SPEED_MONSTER      121 /* misc */
#define MUSE_WAN_MAKE_INVISIBLE     122 /* misc */

/* Scrolls */

#define MUSE_SCR_CREATE_MONSTER     200 /* Defensive */
#define MUSE_SCR_TELEPORTATION      201 /* Defensive */
#define MUSE_SCR_FIRE               202 /* Offensive */
#define MUSE_SCR_EARTH              203 /* Offensive */
#define MUSE_SCR_CLONING            204 /* Offensive */
#define MUSE_SCR_STINKING_CLOUD     205 /* Offensive */
#define MUSE_SCR_REMOVE_CURSE       206 /* misc */
#define MUSE_SCR_FLOOD              207 /* Defensive */

/* Potions */

#define MUSE_POT_PHASING            300 /* Defensive */
#define MUSE_POT_RESTORE_ABILITY    301 /* Defensive */
#define MUSE_POT_CONFUSION          302 /* Offensive */
#define MUSE_POT_BLINDNESS          303 /* Offensive */
#define MUSE_POT_PARALYSIS          304 /* Offensive */
#define MUSE_POT_HALLUCINATION      305 /* Offensive */
#define MUSE_POT_HEALING            306 /* Defensive */
#define MUSE_POT_EXTRA_HEALING      307 /* Defensive */
#define MUSE_POT_FULL_HEALING       308 /* Defensive */
#define MUSE_POT_VAMPIRE_BLOOD      309 /* Defensive */
#define MUSE_POT_SLEEPING           310 /* Offensive */
#define MUSE_POT_POLYMORPH_THROW    311 /* Offensive */
#define MUSE_POT_ACID               312 /* Offensive */
#define MUSE_POT_OIL                313  /* Offensive */
#define MUSE_POT_GAIN_LEVEL         314 /* misc */
#define MUSE_POT_INVISIBILITY       315 /* misc */
#define MUSE_POT_SPEED              316 /* misc */
#define MUSE_POT_POLYMORPH          317 /* misc */
#define MUSE_POT_REFLECT            318
#define MUSE_POT_SICKNESS           319
#define MUSE_POT_MILK               320 /* Defensive */

/* Misc items */

#define MUSE_LIZARD_CORPSE          401 /* Defensive */
#define MUSE_EUCALYPTUS_LEAF        402 /* Defensive */
#define MUSE_BUGLE                  403 /* Defensive */
#define MUSE_UNICORN_HORN           404 /* Defensive */
#define MUSE_FROST_HORN             405 /* Offensive */
#define MUSE_FIRE_HORN              406 /* Offensive */
#define MUSE_CAMERA                 407 /* Offensive */
#define MUSE_MAGIC_FLUTE            408 /* Offensive */
#define MUSE_BULLWHIP               409 /* misc */
#define MUSE_BAG                    410 /* misc */
#define MUSE_FIGURINE               411 /* misc */

/* #define MUSE_INNATE_TPT 9999
 * We cannot use this.  Since monsters get unlimited teleportation, if they
 * were allowed to teleport at will you could never catch them.  Instead,
 * assume they only teleport at random times, despite the inconsistency
 * that if you polymorph into one you teleport at will.
 */


/* Any preliminary checks which may result in the monster being unable to use
 * the item.  Returns 0 if nothing happened, 2 if the monster can't do
 * anything (i.e. it teleported) and 1 if it's dead.
 */
staticfn int
precheck(struct monst *mon, struct obj *obj)
{
    boolean vis;

    if (!obj)
        return 0;
    vis = cansee(mon->mx, mon->my);

    if (obj->oclass == POTION_CLASS) {
        coord cc;
        static const char *const empty = "The potion turns out to be empty.";
        struct monst *mtmp;

        /* Rabid monsters can't stand the thought of drinking liquids.
         * Should there be a message?
         * Should they throw the potion on the ground in disgust? */
        if (mon->mrabid)
            return 0;

        if (objdescr_is(obj, "milky")) {
            if (!(svm.mvitals[PM_GHOST].mvflags & G_GONE)
                && !rn2(POTION_OCCUPANT_CHANCE(svm.mvitals[PM_GHOST].born))) {
                if (!enexto(&cc, mon->mx, mon->my, &mons[PM_GHOST]))
                    return 0;
                mquaffmsg(mon, obj);
                m_useup(mon, obj);
                mtmp = makemon(&mons[PM_GHOST], cc.x, cc.y, MM_NOMSG);
                if (!mtmp) {
                    if (vis)
                        pline1(empty);
                } else {
                    if (vis) {
                        pline(
                            "As %s opens the bottle, an enormous %s emerges!",
                              mon_nam(mon),
                              Hallucination ? rndmonnam(NULL)
                                            : (const char *) "ghost");
                        if (has_free_action(mon)) {
                            pline("%s stiffens momentarily.", Monnam(mon));
                        } else {
                            pline("%s is frightened to death, and unable to move.",
                                  Monnam(mon));
                        }
                    }
                    if (has_free_action(mon))
                        return 0;
                    paralyze_monst(mon, 3);
                }
                return 2;
            }
        }
        if (objdescr_is(obj, "smoky") && !obj->odiluted
            && !(svm.mvitals[PM_DJINNI].mvflags & G_GONE)
            && !rn2(POTION_OCCUPANT_CHANCE(svm.mvitals[PM_DJINNI].born))) {
            if (!enexto(&cc, mon->mx, mon->my, &mons[PM_DJINNI]))
                return 0;
            mquaffmsg(mon, obj);
            m_useup(mon, obj);
            mtmp = makemon(&mons[PM_DJINNI], cc.x, cc.y, MM_NOMSG);
            if (!mtmp) {
                if (vis)
                    pline1(empty);
            } else {
                if (vis)
                    pline_mon(mtmp, "In a cloud of smoke, %s emerges!", a_monnam(mtmp));
                pline("%s speaks.", vis ? Monnam(mtmp) : Something);
                /* I suspect few players will be upset that monsters */
                /* can't wish for wands of death here.... */
                SetVoice(mtmp, 0, 80, 0);
                if (rn2(2)) {
                    verbalize("You freed me!");
                    mtmp->mpeaceful = 1;
                    set_malign(mtmp);
                } else {
                    verbalize("It is about time.");
                    if (vis)
                        pline("%s vanishes.", Monnam(mtmp));
                    mongone(mtmp);
                }
            }
            return 2;
        }
        if ((objdescr_is(obj, "fizzy") || obj->otyp == POT_BOOZE)
            && breathless(mon->data) && !rn2(3)) {
            if (!Deaf)
                pline("%s lets out a loud belch!", Monnam(mon));
            wake_nearto(mon->mx, mon->my, 7 * 7);
        }
    }
    if (obj->oclass == WAND_CLASS && obj->cursed
        /* Monsters should know better, we are less forgiving to them... */
        && !rn2(WAND_BACKFIRE_CHANCE / 2)) {

        /* 3.6.1: no Deaf filter; 'if' message doesn't warrant it, 'else'
           message doesn't need it since You_hear() has one of its own */
        if (vis) {
            pline_mon(mon, "%s zaps %s, which suddenly explodes!", Monnam(mon),
                  an(xname(obj)));
        } else {
            /* same near/far threshold as mzapwand() */
            int range = couldsee(mon->mx, mon->my) /* 9 or 5 */
                           ? (BOLT_LIM + 1) : (BOLT_LIM - 3);

            Soundeffect(se_zap_then_explosion, 100);
            You_hear("a zap and an explosion %s.",
                     (mdistu(mon) <= range * range)
                        ? "nearby" : "in the distance");
        }
        wand_explode(obj, 0, mon);
        gm.m.has_defense = gm.m.has_offense = gm.m.has_misc = 0;
        /* Only one needed to be set to 0 but the others are harmless */
        return (DEADMONSTER(mon)) ? 1 : 2;
    }
    return 0;
}

/* when a monster zaps a wand give a message, deduct a charge, and if it
   isn't directly seen, remove hero's memory of the number of charges.
   Return TRUE if the zap succeeded, FALSE if it failed.
*/
staticfn boolean
mzapwand(
    struct monst *mtmp,
    struct obj *otmp,
    boolean self)
{
    boolean zapcard = otmp->otyp == SCR_ZAPPING,
            wrested = FALSE;

    if (otmp->spe < 0 && !zapcard) {
        ; /* No wresting cancelled wands, it turns to dust below. */
        if (canseemon(mtmp)) {
            pline("%s starts to wave %s... but it turns to dust.",
                          Monnam(mtmp), doname(otmp));
        }
    } else {
        /* Monsters can also wrest wands */
        if (!zapcard && otmp->spe == 0) {
            /* Same chance as players */
            if (!rn2(6 + (otmp->cursed ? 2 : (otmp->blessed ? - 2 : 0)))) {
                if (canseemon(mtmp))
                    pline("%s wrests a last charge from %s!",
                          Monnam(mtmp), doname(otmp));
                wrested = TRUE;
            } else {
                 if (canseemon(mtmp))
                    pline("%s attempts to zap %s!",
                          Monnam(mtmp), doname(otmp));
            }
        }

        if (zapcard || otmp->spe > 0 || wrested) {
            if (!canseemon(mtmp)) {
                int range = couldsee(mtmp->mx, mtmp->my) /* 9 or 5 */
                               ? (BOLT_LIM + 1) : (BOLT_LIM - 3);

                Soundeffect(se_zap, 100);
                You_hear("a %s zap.", (mdistu(mtmp) <= range * range)
                                         ? "nearby" : "distant");
            } else if (self) {
                pline("%s with %s!",
                      monverbself(mtmp, Monnam(mtmp), "zap", (char *) 0),
                      zapcard ? "a zap card" : doname(otmp));
            } else {
                pline_mon(mtmp, "%s %s %s!", Monnam(mtmp),
                zapcard ? "plays" : "zaps",
                zapcard ? "a zap card" : an(xname(otmp)));
                stop_occupation();
            }
        }
    }

    if (!zapcard && otmp->spe <= 0) {
        m_useup(mtmp, otmp);
        return wrested;
    }
    if (!zapcard)
        otmp->spe -= 1;
    return TRUE;
}

/* similar to mzapwand() but for magical horns (only instrument mons play) */
staticfn void
mplayhorn(
    struct monst *mtmp,
    struct obj *otmp,
    boolean self)
{
    char *objnamp, objbuf[BUFSZ];

    if (!canseemon(mtmp)) {
        int range = couldsee(mtmp->mx, mtmp->my) /* 9 or 5 */
                       ? (BOLT_LIM + 1) : (BOLT_LIM - 3);

        Soundeffect(se_horn_being_played, 50);
        You_hear("a horn being played %s.",
                 (mdistu(mtmp) <= range * range)
                    ? "nearby" : "in the distance");
    } else if (self) {
        otmp->dknown = 1;
        objnamp = xname(otmp);
        if (strlen(objnamp) >= QBUFSZ)
            objnamp = simpleonames(otmp);
        Sprintf(objbuf, "a %s directed at", objnamp);
        /* "<mon> plays a <horn> directed at himself!" */
        pline("%s!", monverbself(mtmp, Monnam(mtmp), "play", objbuf));
        makeknown(otmp->otyp); /* (wands handle this slightly differently) */
    } else {
        otmp->dknown = 1;
        objnamp = xname(otmp);
        if (strlen(objnamp) >= QBUFSZ)
            objnamp = simpleonames(otmp);
        pline("%s %s %s directed at you!",
              /* monverbself() would adjust the verb if hallucination made
                 subject plural; stick with singular here, at least for now */
              Monnam(mtmp), "plays", an(objnamp));
        makeknown(otmp->otyp);
        stop_occupation();
    }
    otmp->spe -= 1; /* use a charge */
}

/* see or hear a monster reading a scroll;
   when scroll hasn't been seen, its label is revealed unless hero is deaf */
staticfn void
mreadmsg(struct monst *mtmp, struct obj *otmp)
{
    char onambuf[BUFSZ];
    boolean vismon = canseemon(mtmp),
            tpindicator = (!vismon && sensemon(mtmp));
    boolean carto = Role_if(PM_CARTOMANCER);

    if (!vismon && Deaf)
        return; /* no feedback */

    otmp->dknown = 1; /* seeing or hearing scroll read reveals its label */
    Strcpy(onambuf, singular(otmp, vismon ? doname : ansimpleoname));

    if (vismon) {
        /* directly see the monster reading the scroll */
        pline_mon(mtmp, "%s %s %s!", Monnam(mtmp),
               carto ? "plays" : "reads", onambuf);
    } else { /* !Deaf, otherwise we wouldn't reach here */
        char blindbuf[BUFSZ];
        boolean similar = same_race(gy.youmonst.data, mtmp->data),
                uniqmon = ((mtmp->data->geno & G_UNIQ) != 0
                           /* shopkeepers aren't unique monsters but since
                              they have distinct names, treat them as such */
                           || mtmp->isshk),
                recognize = (!Hallucination
                             && (mtmp->meverseen || (similar && !uniqmon)));
        /* describe unseen monster accurately when not hallucinating if it
           has ever been seen or is the same race as the hero (not yet seen
           unique monsters excepted) */
        int mflags = (SUPPRESS_INVISIBLE | SUPPRESS_SADDLE
                      | (recognize ? SUPPRESS_IT : AUGMENT_IT));

        if (sensemon(mtmp)) {
            tpindicator = TRUE;
        } else if (couldsee(mtmp->mx, mtmp->my) && mdistu(mtmp) <= 10 * 10) {
            /* monster can't be seen or sensed; hero might be blind or monster
               might be at a spot that isn't in view or might be invisible;
               remember it if the spot is within line of sight and relatively
               close */
            map_invisible(mtmp->mx, mtmp->my);
        }

        Snprintf(blindbuf, sizeof blindbuf,
                 carto ? "playing %s" : "reading %s", onambuf);
        strsubst(blindbuf, carto ? "playing a card labeled"
                                 : "reading a scroll labeled",
                 mtmp->mconf ? "attempting to incant" : "incant");
        You_hear("%s %s.",
                 x_monnam(mtmp, ARTICLE_A, (char *) 0, mflags, FALSE),
                 blindbuf);
        if (tpindicator)
            flash_mon(mtmp);
    }
    if (mtmp->mconf) /* (note: won't get if not seen and hero can't hear) */
        pline("Being confused, %s mispronounces the magic words...",
              vismon ? mon_nam(mtmp) : mhe(mtmp));
}

staticfn void
mquaffmsg(struct monst *mtmp, struct obj *otmp)
{
    if (canseemon(mtmp)) {
        otmp->dknown = 1;
        pline_mon(mtmp, "%s drinks %s!", Monnam(mtmp), singular(otmp, doname));
    } else if (!Deaf) {
        Soundeffect(se_mon_chugging_potion, 25);
        You_hear("a chugging sound.");
    }
}

staticfn boolean
m_use_healing(struct monst *mtmp)
{
    struct obj *obj;

    if ((obj = m_carrying(mtmp, POT_FULL_HEALING)) != 0) {
        gm.m.defensive = obj;
        gm.m.has_defense = MUSE_POT_FULL_HEALING;
        return TRUE;
    }
    if ((obj = m_carrying(mtmp, POT_EXTRA_HEALING)) != 0) {
        gm.m.defensive = obj;
        gm.m.has_defense = MUSE_POT_EXTRA_HEALING;
        return TRUE;
    }
    if ((obj = m_carrying(mtmp, POT_HEALING)) != 0) {
        gm.m.defensive = obj;
        gm.m.has_defense = MUSE_POT_HEALING;
        return TRUE;
    }
    if (mtmp->mrabid || mtmp->mdiseased) {
        if ((obj = m_carrying(mtmp, EUCALYPTUS_LEAF)) != 0) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_EUCALYPTUS_LEAF;
            return TRUE;
        }
    }
    if (mtmp->mconf || mtmp->mstun) {
        if ((obj = m_carrying(mtmp, POT_MILK)) != 0) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_POT_MILK;
            return TRUE;
        }
    }
    if (is_vampire(mtmp->data) &&
        (obj = m_carrying(mtmp, POT_VAMPIRE_BLOOD)) != 0) {
        gm.m.defensive = obj;
        gm.m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
        return TRUE;
    }

    return FALSE;
}

/* return TRUE if monster mtmp can see at least one sleeping soldier */
staticfn boolean
m_sees_sleepy_soldier(struct monst *mtmp)
{
    coordxy x = mtmp->mx, y = mtmp->my;
    coordxy xx, yy;
    struct monst *mon;

    /* Distance is arbitrary.  What we really want to do is
     * have the soldier play the bugle when it sees or
     * remembers soldiers nearby...
     */
    for (xx = x - 3; xx <= x + 3; xx++)
        for (yy = y - 3; yy <= y + 3; yy++) {
            if (!isok(xx, yy) || (xx == x && yy == y))
                continue;
            if ((mon = m_at(xx, yy)) != 0 && is_mercenary(mon->data)
                && mon->data != &mons[PM_GUARD]
                && helpless(mon))
                return TRUE;
        }
    return FALSE;
}

staticfn void
m_tele(
    struct monst *mtmp, /* monst that might be teleported */
    boolean vismon,     /* can see it */
    boolean oseen,      /* have seen the object that triggered this */
    int how)            /* type of that object */
{
    if (tele_restrict(mtmp)) { /* mysterious force... */
        if (vismon && how)     /* mentions 'teleport' */
            makeknown(how);
        /* monster learns that teleportation isn't useful here */
        if (noteleport_level(mtmp))
            mon_learns_traps(mtmp, TELEP_TRAP);
    } else if ((mon_has_amulet(mtmp) || On_W_tower_level(&u.uz))) {
        if (vismon)
            pline_mon(mtmp, "%s seems disoriented for a moment.", Monnam(mtmp));
    } else {
        /* teleport monster 'mtmp' */
        if (how) {
            /* teleporation has been triggered by an object */
            if (oseen)
                makeknown(how);
            (void) rloc(mtmp, RLOC_MSG);
        } else {
            /* monster is voluntarily entering a teleporation trap; use the
               trap instead of rloc() in case it sends 'victim' to a vault */
            mtmp->mx = gt.trapx, mtmp->my = gt.trapy;
            (void) mintrap(mtmp, FORCETRAP);
        }
    }
}

/* return TRUE if monster mtmp has another monster next to it.
 * Called from find_defensive() where it is limited to Is_knox()
 * only, otherwise you could trap two monsters next to each other
 * in a boulder fort, and they would be happy to stay in there. */
staticfn boolean
m_next2m(struct monst *mtmp)
{
    coordxy x, y;
    struct monst *m2;

    if (DEADMONSTER(mtmp) || mon_offmap(mtmp))
        return FALSE;

    for (x = mtmp->mx - 1; x <= mtmp->mx + 1; x++)
        for (y = mtmp->my - 1; y <= mtmp->my + 1; y++) {
            if (!isok(x,y))
                continue;
            if ((m2 = m_at(x, y)) && m2 != mtmp)
                return TRUE;
        }
    return FALSE;
}

/* Select a defensive item/action for a monster.  Returns TRUE iff one is
   found. */
boolean
find_defensive(struct monst *mtmp, boolean tryescape)
{
    struct obj *obj;
    struct trap *t;
    int fraction;
    coordxy x = mtmp->mx, y = mtmp->my;
    boolean stuck = (mtmp == u.ustuck),
            immobile = (mtmp->data->mmove == 0);
    stairway *stway;

    gm.m.defensive = (struct obj *) 0;
    gm.m.has_defense = 0;

    if (is_animal(mtmp->data) || mindless(mtmp->data))
        return FALSE;
    if (!tryescape && dist2(x, y, mtmp->mux, mtmp->muy) > 25)
        return FALSE;
    if (tryescape && Is_knox(&u.uz)
        && !m_next2u(mtmp) && m_next2m(mtmp))
        return FALSE;
    if (u.uswallow && stuck)
        return FALSE;

    /*
     * Since unicorn horns don't get used up, the monster would look
     * silly trying to use the same cursed horn round after round,
     * so skip cursed unicorn horns.
     *
     * Unicorns use their own horns; they're excluded from inventory
     * scanning by nohands().  Ki-rin is depicted in the AD&D Monster
     * Manual with same horn as a unicorn, so let it use its horn too.
     * is_unicorn() doesn't include it; the class differs and it has
     * no interest in gems.
     */
    if (mtmp->mconf || mtmp->mstun || mtmp->mrabid
                    || mtmp->mdiseased || !mtmp->mcansee) {
        obj = 0;
        if (!nohands(mtmp->data)) {
            for (obj = mtmp->minvent; obj; obj = obj->nobj)
                if (obj->otyp == UNICORN_HORN && !obj->cursed)
                    break;
        }
        if (obj || is_unicorn(mtmp->data) || mtmp->data == &mons[PM_KI_RIN]) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_UNICORN_HORN;
            return TRUE;
        }
    }
    /* Check if they can use a potion of milk */
    if (mtmp->mconf || mtmp->mstun || !mtmp->mcansee) {
        if (!nohands(mtmp->data)) {
            for (obj = mtmp->minvent; obj; obj = obj->nobj) {
                if (obj && obj->otyp == POT_MILK && !obj->cursed) {
                    gm.m.defensive = obj;
                    gm.m.has_defense = MUSE_POT_MILK;
                    return TRUE;
                }
            }
        }
    }

    if (mtmp->mrabid || mtmp->mdiseased) {
        for (obj = mtmp->minvent; obj; obj = obj->nobj) {
            if (!nohands(mtmp->data)) {
                if (obj && obj->otyp == POT_HEALING && !obj->cursed)
                    break;
                if (obj && obj->otyp == POT_FULL_HEALING) {
                    gm.m.defensive = obj;
                    gm.m.has_defense = MUSE_POT_FULL_HEALING;
                    return TRUE;
                } else if (obj && obj->otyp == POT_EXTRA_HEALING
                           && !obj->cursed) {
                    gm.m.defensive = obj;
                    gm.m.has_defense = MUSE_POT_EXTRA_HEALING;
                    return TRUE;
                } else if (obj && obj->otyp == POT_HEALING
                           && obj->blessed) {
                    gm.m.defensive = obj;
                    gm.m.has_defense = MUSE_POT_HEALING;
                    return TRUE;
                }
            }
            if (obj && obj->otyp == EUCALYPTUS_LEAF) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_EUCALYPTUS_LEAF;
                return TRUE;
            }
        }
    }

    if (mtmp->mconf || mtmp->mstun) {
        struct obj *liztin = 0;

        for (obj = mtmp->minvent; obj; obj = obj->nobj) {
            if (obj->otyp == CORPSE && obj->corpsenm == PM_LIZARD) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_LIZARD_CORPSE;
                return TRUE;
            } else if (obj->otyp == TIN && obj->corpsenm == PM_LIZARD) {
                liztin = obj;
            }
        }
        /* confused or stunned monster might not be able to open tin */
        if (liztin && mcould_eat_tin(mtmp) && rn2(3)) {
            gm.m.defensive = liztin;
            /* tin and corpse ultimately end up being handled the same */
            gm.m.has_defense = MUSE_LIZARD_CORPSE;
            return TRUE;
        }
    }

    /* It so happens there are two unrelated cases when we might want to
     * check specifically for healing alone.  The first is when the monster
     * is blind (healing cures blindness).  The second is when the monster
     * is peaceful; then we don't want to flee the player, and by
     * coincidence healing is all there is that doesn't involve fleeing.
     * These would be hard to combine because of the control flow.
     * Pestilence won't use healing even when blind.
     */
    if ((!mtmp->mcansee || mtmp->mrabid || mtmp->mdiseased)
            && mtmp->data != &mons[PM_PESTILENCE]) {
        if (m_use_healing(mtmp))
            return TRUE;
    }

    if (mtmp->mcan && !nohands(mtmp->data)) {
        /* TODO? maybe only monsters for whom cancellation actually matters
         * should bother fixing it -- for a monster without any abilities that
         * are affected by cancellation, why bother drinking the potion? */
        if ((obj = m_carrying(mtmp, POT_RESTORE_ABILITY)) != 0) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_POT_RESTORE_ABILITY;
            return TRUE;
        }
    }

    /* monsters aren't given wands of undead turning but if they
       happen to have picked one up, use it against corpse wielder;
       when applicable, use it now even if 'mtmp' isn't wounded */
    if (!mtmp->mpeaceful && !nohands(mtmp->data)
        && uwep && uwep->otyp == CORPSE
        && touch_petrifies(&mons[uwep->corpsenm])
        && !poly_when_stoned(mtmp->data)
        && !(resists_ston(mtmp) || defended(mtmp, AD_STON))
        && lined_up(mtmp)) { /* only lines up if distu range is within 5*5 */
        /* could use m_carrying(), then nxtobj() when matching wand
           is empty, but direct traversal is actually simpler here */
        for (obj = mtmp->minvent; obj; obj = obj->nobj)
            if (obj->otyp == WAN_UNDEAD_TURNING) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_WAN_UNDEAD_TURNING;
                return TRUE;
            }
    }

    if (!tryescape) {
        /* do we try to heal? */
        fraction = u.ulevel < 10 ? 5 : u.ulevel < 14 ? 4 : 3;
        if (mtmp->mhp >= mtmp->mhpmax
            || (mtmp->mhp >= 10 && mtmp->mhp * fraction >= mtmp->mhpmax))
            return FALSE;

        if (mtmp->mpeaceful) {
            if (!nohands(mtmp->data)) {
                if (m_use_healing(mtmp))
                    return TRUE;
            }
            return FALSE;
        }
    }

    if (stuck || immobile || mtmp->mtrapped) {
        ; /* fleeing by stairs or traps is not possible */
    } else if (levl[x][y].typ == STAIRS) {
        stway = stairway_at(x,y);
        if (stway && !stway->up && stway->tolev.dnum == u.uz.dnum) {
            if (!mon_prop(mtmp, LEVITATION))
                gm.m.has_defense = MUSE_DOWNSTAIRS;
        } else if (stway && stway->up && stway->tolev.dnum == u.uz.dnum) {
            gm.m.has_defense = MUSE_UPSTAIRS;
        } else if (stway &&  stway->tolev.dnum != u.uz.dnum) {
            if (stway->up || !mon_prop(mtmp, LEVITATION))
                gm.m.has_defense = MUSE_SSTAIRS;
        }
    } else if (levl[x][y].typ == LADDER) {
        stway = stairway_at(x,y);
        if (stway && stway->up && stway->tolev.dnum == u.uz.dnum) {
            gm.m.has_defense = MUSE_UP_LADDER;
        } else if (stway && !stway->up && stway->tolev.dnum == u.uz.dnum) {
            if (!mon_prop(mtmp, LEVITATION))
                gm.m.has_defense = MUSE_DN_LADDER;
        } else if (stway && stway->tolev.dnum != u.uz.dnum) {
            if (stway->up || !mon_prop(mtmp, LEVITATION))
                gm.m.has_defense = MUSE_SSTAIRS;
        }
    } else {
        /* Note: trap doors take precedence over teleport traps. */
        coordxy xx, yy, i, locs[10][2];
        boolean ignore_boulders = (verysmall(mtmp->data)
                                   || throws_rocks(mtmp->data)
                                   || passes_walls(mtmp->data)),
            diag_ok = !NODIAG(monsndx(mtmp->data));

        for (i = 0; i < 10; ++i) /* 10: 9 spots plus sentinel */
            locs[i][0] = locs[i][1] = 0;
        /* collect viable spots; monster's <mx,my> comes first */
        locs[0][0] = x, locs[0][1] = y;
        i = 1;
        for (xx = x - 1; xx <= x + 1; xx++)
            for (yy = y - 1; yy <= y + 1; yy++)
                if (isok(xx, yy) && (xx != x || yy != y)) {
                    locs[i][0] = xx, locs[i][1] = yy;
                    ++i;
                }
        /* look for a suitable trap among the viable spots */
        for (i = 0; i < 10; ++i) {
            xx = locs[i][0], yy = locs[i][1];
            if (!xx)
                break; /* we've run out of spots */
            /* skip if it's hero's location
               or a diagonal spot and monster can't move diagonally
               or some other monster is there */
            if (u_at(xx, yy)
                || (xx != x && yy != y && !diag_ok)
                || (svl.level.monsters[xx][yy] && !(xx == x && yy == y)))
                continue;
            /* skip if there's no trap or can't/won't move onto trap */
            if ((t = t_at(xx, yy)) == 0
                || (!ignore_boulders && sobj_at(BOULDER, xx, yy))
                || onscary(xx, yy, mtmp))
                continue;
            /* use trap if it's the correct type */
            if (is_hole(t->ttyp)
                && !mon_prop(mtmp, LEVITATION)
                && !mtmp->isshk && !mtmp->isgd && !mtmp->ispriest
                && Can_fall_thru(&u.uz)) {
                gt.trapx = xx;
                gt.trapy = yy;
                gm.m.has_defense = MUSE_TRAPDOOR;
                break; /* no need to look at any other spots */
            } else if (t->ttyp == TELEP_TRAP) {
                gt.trapx = xx;
                gt.trapy = yy;
                gm.m.has_defense = MUSE_TELEPORT_TRAP;
            }
        }
    }

    if (nohands(mtmp->data)) /* can't use objects */
        goto botm;

    if (is_mercenary(mtmp->data) && (obj = m_carrying(mtmp, BUGLE)) != 0
        && m_sees_sleepy_soldier(mtmp)) {
        gm.m.defensive = obj;
        gm.m.has_defense = MUSE_BUGLE;
    }

    /* use immediate physical escape prior to attempting magic */
    if (gm.m.has_defense) /* stairs, trap door or tele-trap, bugle alert */
        goto botm;

    /* kludge to cut down on trap destruction (particularly portals) */
    t = t_at(x, y);
    if (t && (is_pit(t->ttyp) || t->ttyp == WEB
              || t->ttyp == BEAR_TRAP))
        t = 0; /* ok for monster to dig here */

#define nomore(x)       if (gm.m.has_defense == x) continue;
    /* selection could be improved by collecting all possibilities
       into an array and then picking one at random */
    for (obj = mtmp->minvent; obj; obj = obj->nobj) {
        /* don't always use the same selection pattern */
        if (gm.m.has_defense && !rn2(3))
            break;

        /* nomore(MUSE_WAN_DIGGING); */
        if (gm.m.has_defense == MUSE_WAN_DIGGING)
            break;
        if (obj->otyp == WAN_DIGGING && !stuck && !t
            && !mtmp->isshk && !mtmp->isgd && !mtmp->ispriest
            && !mon_prop(mtmp, LEVITATION)
            /* monsters digging in Sokoban can ruin things */
            && !Sokoban
            /* digging wouldn't be effective; assume they know that */
            && !(levl[x][y].wall_info & W_NONDIGGABLE)
            && !IS_FURNITURE(levl[x][y].typ)
            && !(Is_botlevel(&u.uz) || In_endgame(&u.uz))
            && !svl.level.flags.hardfloor
            && !(is_ice(x, y) || is_pool(x, y) || is_lava(x, y))
            && !(is_Vlad(mtmp) && In_V_tower(&u.uz))) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_WAN_DIGGING;
        }
        nomore(MUSE_WAN_TELEPORTATION_SELF);
        nomore(MUSE_WAN_TELEPORTATION);
        if (obj->otyp == WAN_TELEPORTATION) {
            /* use the TELEP_TRAP bit to determine if they know
             * about noteleport on this level or not.  Avoids
             * ineffective re-use of teleportation.  This does
             * mean if the monster leaves the level, they'll know
             * about teleport traps.
             */
            if (!noteleport_level(mtmp)
                || !mon_knows_traps(mtmp, TELEP_TRAP)) {
                gm.m.defensive = obj;
                gm.m.has_defense = (mon_has_amulet(mtmp))
                                    ? MUSE_WAN_TELEPORTATION
                                    : MUSE_WAN_TELEPORTATION_SELF;
            }
        }
        nomore(MUSE_SCR_TELEPORTATION);
        if (obj->otyp == SCR_TELEPORTATION && mtmp->mcansee
            && haseyes(mtmp->data)
            && (!obj->cursed || (!(mtmp->isshk && inhishop(mtmp))
                                 && !mtmp->isgd && !mtmp->ispriest))) {
            /* see WAN_TELEPORTATION case above */
            if (!noteleport_level(mtmp)
                || !mon_knows_traps(mtmp, TELEP_TRAP)) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_SCR_TELEPORTATION;
            }
        }

        nomore(MUSE_SCR_FLOOD);
        if (obj->otyp == SCR_FLOOD && mtmp->mcansee
            && haseyes(mtmp->data)
            && !(Flying || Levitation) /* This is obvious to monsters */
            && distu(mtmp->mx, mtmp->my) < (5*5)) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_SCR_FLOOD;
        }

        if (mtmp->data != &mons[PM_PESTILENCE]) {
            nomore(MUSE_POT_FULL_HEALING);
            if (obj->otyp == POT_FULL_HEALING) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_FULL_HEALING;
            }
            nomore(MUSE_POT_EXTRA_HEALING);
            if (obj->otyp == POT_EXTRA_HEALING) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_EXTRA_HEALING;
            }
            nomore(MUSE_WAN_CREATE_MONSTER);
            if (obj->otyp == WAN_CREATE_MONSTER) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_WAN_CREATE_MONSTER;
            }
            nomore(MUSE_POT_HEALING);
            if (obj->otyp == POT_HEALING) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_HEALING;
            }
            nomore(MUSE_POT_MILK);
            if (obj->otyp == POT_MILK) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_MILK;
            }
            nomore(MUSE_POT_VAMPIRE_BLOOD);
            if (is_vampire(mtmp->data) && obj->otyp == POT_VAMPIRE_BLOOD) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_VAMPIRE_BLOOD;
            }
        } else { /* Pestilence */
            nomore(MUSE_POT_FULL_HEALING);
            if (obj->otyp == POT_SICKNESS) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_POT_FULL_HEALING;
            }
            nomore(MUSE_WAN_CREATE_MONSTER);
            if (obj->otyp == WAN_CREATE_MONSTER) {
                gm.m.defensive = obj;
                gm.m.has_defense = MUSE_WAN_CREATE_MONSTER;
            }
        }
        nomore(MUSE_EUCALYPTUS_LEAF);
        if ((mtmp->mrabid || mtmp->mdiseased)
              && obj->otyp == EUCALYPTUS_LEAF) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_EUCALYPTUS_LEAF;
        }
        nomore(MUSE_POT_RESTORE_ABILITY);
        if (mtmp->mcan && obj->otyp == POT_RESTORE_ABILITY) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_POT_RESTORE_ABILITY;
        }
        nomore(MUSE_POT_PHASING);
        if (!passes_walls(mtmp->data) && obj->otyp == POT_PHASING) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_POT_PHASING;
        }
        nomore(MUSE_SCR_CREATE_MONSTER);
        if (obj->otyp == SCR_CREATE_MONSTER) {
            gm.m.defensive = obj;
            gm.m.has_defense = MUSE_SCR_CREATE_MONSTER;
        }
    }
 botm:
    return (boolean) !!gm.m.has_defense;
#undef nomore
}

/* when a monster deliberately enters a trap, make sure the spot becomes
   accessible (trap doors and teleporters inside niches are located at
   secret corridor locations; convert such into normal corridor even if
   hero doesn't see it happen) */
staticfn void
reveal_trap(struct trap *t, boolean seeit)
{
    struct rm *lev = &levl[t->tx][t->ty];

    if (lev->typ == SCORR) {
        lev->typ = CORR, lev->flags = 0; /* set_levltyp(,,CORR) */
        unblock_point(t->tx, t->ty);
    }
    if (seeit)
        seetrap(t);
}

/* Monsters without the Amulet escape the dungeon and
 * are gone for good when they leave up the up stairs.
 * A monster with the Amulet would leave it behind
 * (mongone -> mdrop_special_objs) but we force any
 * monster who manages to acquire it or the invocation
 * tools to stick around instead of letting it escape.
 * Don't let the Wizard escape even when not carrying
 * anything of interest unless there are more than 1
 * of him.
 */
staticfn int
mon_escape(struct monst *mtmp, boolean vismon)
{
    if (mon_has_special(mtmp)
        || (mtmp->iswiz && svc.context.no_of_wizards < 2)
        || mtmp->iscthulhu)
        return 0;
    if (vismon)
        pline_mon(mtmp, "%s escapes the dungeon!", Monnam(mtmp));
    mongone(mtmp);
    return 2;
}

/* Perform a defensive action for a monster.  Must be called immediately
 * after find_defensive().  Return values are 0: did something, 1: died,
 * 2: did something and can't attack again (i.e. teleported).
 */
int
use_defensive(struct monst *mtmp)
{
    static const char MissingDefensiveItem[] = "use_defensive: no %s";
    int i, fleetim;
    struct obj *otmp = gm.m.defensive;
    boolean vis, vismon, oseen;
    struct trap *t;
    stairway *stway;

    if ((i = precheck(mtmp, otmp)) != 0)
        return i;
    vis = cansee(mtmp->mx, mtmp->my);
    vismon = canseemon(mtmp);
    oseen = otmp && vismon;

    /* when using defensive choice to run away, we want monster to avoid
       rushing right straight back; don't override if already scared */
    fleetim = !mtmp->mflee ? (33 - (30 * mtmp->mhp / mtmp->mhpmax)) : 0;
#define m_flee(m)                                \
    if (fleetim && !m->iswiz && !m->iscthulhu) { \
        monflee(m, fleetim, FALSE, FALSE);       \
    }

    switch (gm.m.has_defense) {
    case MUSE_UNICORN_HORN:
        /* unlike most defensive cases, unicorn horn object is optional */
        if (vismon) {
            if (otmp)
                pline_mon(mtmp, "%s uses a unicorn horn!", Monnam(mtmp));
            else
                pline_The("tip of %s's horn glows!", mon_nam(mtmp));
        }
        if (!mtmp->mcansee) {
            mcureblindness(mtmp, vismon);
        } else if (mtmp->mrabid || mtmp->mdiseased) {
            mtmp->mrabid = mtmp->mdiseased = 0;
            if (vismon)
                pline("%s is no longer ill.", Monnam(mtmp));
        } else if (mtmp->mconf || mtmp->mstun) {
            mtmp->mconf = mtmp->mstun = 0;
            if (vismon)
                pline_mon(mtmp, "%s seems steadier now.", Monnam(mtmp));
        } else {
            impossible("No need for unicorn horn?");
        }
        return 2;
    case MUSE_BUGLE:
        if (!otmp)
            panic(MissingDefensiveItem, "bugle");
        if (vismon) {
            pline_mon(mtmp, "%s plays %s!", Monnam(mtmp), doname(otmp));
        } else if (!Deaf) {
            Soundeffect(se_bugle_playing_reveille, 100);
            You_hear("a bugle playing reveille!");
        }
        awaken_soldiers(mtmp);
        return 2;
    case MUSE_WAN_TELEPORTATION_SELF:
        if (!otmp)
            panic(MissingDefensiveItem, "wand of teleportation");
        if ((mtmp->isshk && inhishop(mtmp)) || mtmp->isgd || mtmp->ispriest)
            return 2;
        m_flee(mtmp);
        if (mzapwand(mtmp, otmp, TRUE))
            m_tele(mtmp, vismon, oseen, WAN_TELEPORTATION);
        return 2;
    case MUSE_WAN_TELEPORTATION:
        if (!otmp)
            panic(MissingDefensiveItem, "wand of teleportation");
        gz.zap_oseen = oseen;
        if (mzapwand(mtmp, otmp, TRUE)) {
            gm.m_using = TRUE;
            mbhit(mtmp, rn1(8, 6), mbhitm, bhito, otmp);
            /* monster learns that teleportation isn't useful here */
            if (noteleport_level(mtmp))
                mon_learns_traps(mtmp, TELEP_TRAP);
            gm.m_using = FALSE;
        }
        return 2;
    case MUSE_SCR_TELEPORTATION: {
        int obj_is_cursed;

        if (!otmp)
            panic(MissingDefensiveItem, "scroll of teleportation");
        obj_is_cursed = otmp->cursed;
        if (mtmp->isshk || mtmp->isgd || mtmp->ispriest)
            return 2;
        m_flee(mtmp);
        /* we want to be able to access otmp after the teleport but it
           might get destroyed if still in mtmp's inventory (maybe mtmp
           lands in lava or on a fire trap) so take it out in advance */
        if (otmp->quan > 1L)
            otmp = splitobj(otmp, 1L);
        extract_from_minvent(mtmp, otmp, FALSE, FALSE);
        /* 'last_msg' will be changed to PLNMSG_UNKNOWN if any messages
           are issued by mreadmsg(), 'if (vismon) pline()', or m_tele() */
        iflags.last_msg = PLNMSG_enum;
        mreadmsg(mtmp, otmp); /* sets otmp->dknown if !Blind or !Deaf */
        if (obj_is_cursed || mtmp->mconf) {
            int nlev;
            d_level flev;

            nlev = random_teleport_level();
            if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
                if (vismon)
                    pline_mon(mtmp, "%s seems very disoriented for a moment.",
                          Monnam(mtmp));
            } else if (nlev == depth(&u.uz)) {
                if (vismon)
                    pline_mon(mtmp, "%s shudders for a moment.", Monnam(mtmp));
            } else {
                get_level(&flev, nlev);
                migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
                                 (coord *) 0);
            }
        } else {
            m_tele(mtmp, vismon, oseen, SCR_TELEPORTATION);
        }
        /* m_tele() handles makeknown(); trycall() will be a no-op when
           otmp->otyp is already discovered */
        if (otmp->dknown && iflags.last_msg != PLNMSG_enum)
            trycall(otmp);
        /* already removed from mtmp->minvent so not 'm_useup(mtmp, otmp)' */
        obfree(otmp, (struct obj *) 0);
        return 2;
    }
    case MUSE_SCR_FLOOD:
        if (otmp->quan > 1L)
            otmp = splitobj(otmp, 1L);

        /* Note: do_clear_area did not work correctly when I tried
         * to pass the scroll from the monsters position. As a
         * consequence (and because I don't have the skilled to fix it)
         * we are assuming the flood is always intended to be centered
         * on the hero */
        mreadmsg(mtmp, otmp); /* sets otmp->dknown if !Blind or !Deaf */
        seffect_flood(&otmp, mtmp);
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    case MUSE_WAN_DIGGING:
        if (!otmp)
            panic(MissingDefensiveItem, "wand of digging");
        m_flee(mtmp);
        if (!mzapwand(mtmp, otmp, FALSE))
            return 2;
        if (oseen)
            makeknown(WAN_DIGGING);
        if (oseen && (IS_FURNITURE(levl[mtmp->mx][mtmp->my].typ)
            || IS_DRAWBRIDGE(levl[mtmp->mx][mtmp->my].typ)
            || (is_drawbridge_wall(mtmp->mx, mtmp->my) >= 0)
            || stairway_at(mtmp->mx, mtmp->my))) {
            pline_The("digging ray is ineffective.");
            return 2;
        }
        if (!Can_dig_down(&u.uz) && !levl[mtmp->mx][mtmp->my].candig) {
            /* can't dig further if there's already a pit (or other trap)
               here, or if pit creation fails for some reason */
            if (t_at(mtmp->mx, mtmp->my)
                || !(t = maketrap(mtmp->mx, mtmp->my, PIT))) {
                if (vismon) {
                    pline_The("%s here is too hard to dig in.",
                              surface(mtmp->mx, mtmp->my));
                }
                return 2;
            }
            /* pit creation succeeded */
            if (vis) {
                seetrap(t);
                pline_mon(mtmp, "%s has made a pit in the %s.", Monnam(mtmp),
                      surface(mtmp->mx, mtmp->my));
            }
            fill_pit(mtmp->mx, mtmp->my);
            recalc_block_point(mtmp->mx, mtmp->my);
            return (mintrap(mtmp, FORCEBUNGLE) == Trap_Killed_Mon) ? 1 : 2;
        }
        t = maketrap(mtmp->mx, mtmp->my, HOLE);
        if (!t)
            return 2;
        recalc_block_point(mtmp->mx, mtmp->my);
        seetrap(t);
        if (vis) {
            pline_mon(mtmp, "%s has made a hole in the %s.", Monnam(mtmp),
                  surface(mtmp->mx, mtmp->my));
            pline_mon(mtmp, "%s %s through...", Monnam(mtmp),
                  mon_prop(mtmp, FLYING) ? "dives" : "falls");
        } else if (!Deaf) {
            Soundeffect(se_crash_through_floor, 100);
            You_hear("%s crash through the %s.", something,
                     surface(mtmp->mx, mtmp->my));
        }
        fill_pit(mtmp->mx, mtmp->my);
        /* we made sure that there is a level for mtmp to go to */
        migrate_to_level(mtmp, ledger_no(&u.uz) + 1, MIGR_RANDOM,
                         (coord *) 0);
        return 2;
    case MUSE_WAN_UNDEAD_TURNING:
        if (!otmp)
            panic(MissingDefensiveItem, "wand of undead turning");
        gz.zap_oseen = oseen;
        if (mzapwand(mtmp, otmp, FALSE)) {
            gm.m_using = TRUE;
            mbhit(mtmp, rn1(8, 6), mbhitm, bhito, otmp);
            gm.m_using = FALSE;
        }
        return 2;
    case MUSE_WAN_CREATE_MONSTER:
        return muse_createmonster(mtmp, otmp);
        break;
    case MUSE_SCR_CREATE_MONSTER: {
        coord cc;
        struct permonst *pm = 0, *fish = 0;
        int cnt = 1;
        struct monst *mon;
        boolean known = FALSE;

        if (!otmp)
            panic(MissingDefensiveItem, "scroll of create monster");
        if (!rn2(73))
            cnt += rnd(4);
        if (mtmp->mconf || otmp->cursed)
            cnt += 12;
        if (mtmp->mconf)
            pm = fish = &mons[PM_ACID_BLOB];
        else if (is_damp_terrain(mtmp->mx, mtmp->my))
            fish = &mons[u.uinwater ? PM_GIANT_EEL : PM_CROCODILE];

        if (is_moncard(otmp)) {
            pm = &mons[otmp->corpsenm];
            cnt = 1; /* Same as for player */
        }

        mreadmsg(mtmp, otmp);
        while (cnt--) {
            /* `fish' potentially gives bias towards water locations;
               `pm' is what to actually create (0 => random) */
            if (!enexto(&cc, mtmp->mx, mtmp->my, fish))
                break;
            if (is_moncard(otmp))
                mon = make_msummoned(pm, mtmp, FALSE, cc.x, cc.y);
            else
                mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
            if (mon && canspotmon(mon))
                known = TRUE;
        }
        /* The only case where we don't use oseen.  For wands, you
         * have to be able to see the monster zap the wand to know
         * what type it is.  For teleport scrolls, you have to see
         * the monster to know it teleported.
         */
        if (known)
            makeknown(SCR_CREATE_MONSTER);
        else
            trycall(otmp);
        m_useup(mtmp, otmp);
        return 2;
    }
    case MUSE_TRAPDOOR:
        /* trap doors on "bottom" levels of dungeons are rock-drop
         * trap doors, not holes in the floor.  We check here for
         * safety.
         */
        if (Is_botlevel(&u.uz))
            return 0;
        m_flee(mtmp);
        t = t_at(gt.trapx, gt.trapy);
        if (vis) {
            pline_mon(mtmp, "%s %s into a %s!", Monnam(mtmp),
                  vtense(fakename[0], locomotion(mtmp->data, "jump")),
                  trapname(t->ttyp, FALSE));
        }
        /* if trap was in a concealed niche, it's no longer concealed */
        reveal_trap(t, vis);

        /*  don't use rloc_to() because worm tails must "move" */
        remove_monster(mtmp->mx, mtmp->my);
        newsym(mtmp->mx, mtmp->my); /* update old location */
        place_monster(mtmp, gt.trapx, gt.trapy);
        if (mtmp->wormno)
            worm_move(mtmp);
        newsym(gt.trapx, gt.trapy);

        migrate_to_level(mtmp, ledger_no(&u.uz) + 1, MIGR_RANDOM,
                         (coord *) 0);
        return 2;
    case MUSE_UPSTAIRS:
        m_flee(mtmp);
        stway = stairway_at(mtmp->mx, mtmp->my);
        if (!stway)
            return 0;
        if (ledger_no(&u.uz) == 1)
            /* impossible; level 1 upstairs are SSTAIRS */
            return mon_escape(mtmp, vismon);
        if (Inhell && mon_has_amulet(mtmp) && !rn2(4)
            && (dunlev(&u.uz) < dunlevs_in_dungeon(&u.uz) - 3)) {
            if (vismon)
                pline("As %s climbs the stairs, a mysterious force"
                      " momentarily surrounds %s...",
                      mon_nam(mtmp), mhim(mtmp));
            /* simpler than for the player; this will usually be
               the Wizard and he'll immediately go right to the
               upstairs, so there's not much point in having any
               chance for a random position on the current level */
            migrate_to_level(mtmp, ledger_no(&u.uz) + 1, MIGR_RANDOM,
                             (coord *) 0);
        } else {
            if (vismon)
                pline_mon(mtmp, "%s escapes upstairs!", Monnam(mtmp));
            migrate_to_level(mtmp, ledger_no(&(stway->tolev)),
                             MIGR_STAIRS_DOWN, (coord *) 0);
        }
        return 2;
    case MUSE_DOWNSTAIRS:
        m_flee(mtmp);
        stway = stairway_at(mtmp->mx, mtmp->my);
        if (!stway)
            return 0;
        if (vismon)
            pline_mon(mtmp, "%s escapes downstairs!", Monnam(mtmp));
        migrate_to_level(mtmp, ledger_no(&(stway->tolev)), MIGR_STAIRS_UP,
                         (coord *) 0);
        return 2;
    case MUSE_UP_LADDER:
        m_flee(mtmp);
        stway = stairway_at(mtmp->mx, mtmp->my);
        if (!stway)
            return 0;
        if (vismon)
            pline_mon(mtmp, "%s escapes up the ladder!", Monnam(mtmp));
        migrate_to_level(mtmp, ledger_no(&(stway->tolev)), MIGR_LADDER_DOWN,
                         (coord *) 0);
        return 2;
    case MUSE_DN_LADDER:
        m_flee(mtmp);
        stway = stairway_at(mtmp->mx, mtmp->my);
        if (!stway)
            return 0;
        if (vismon)
            pline_mon(mtmp, "%s escapes down the ladder!", Monnam(mtmp));
        migrate_to_level(mtmp, ledger_no(&(stway->tolev)), MIGR_LADDER_UP,
                         (coord *) 0);
        return 2;
    case MUSE_SSTAIRS:
        m_flee(mtmp);
        stway = stairway_at(mtmp->mx, mtmp->my);
        if (!stway)
            return 0;
        if (ledger_no(&u.uz) == 1) {
            return mon_escape(mtmp, vismon);
        }
        if (vismon)
            pline_mon(mtmp, "%s escapes %sstairs!", Monnam(mtmp),
                  stway->up ? "up" : "down");
        /* going from the Valley to Castle (Stronghold) has no sstairs
           to target, but having gs.sstairs.<sx,sy> == <0,0> will work the
           same as specifying MIGR_RANDOM when mon_arrive() eventually
           places the monster, so we can use MIGR_SSTAIRS unconditionally */
        migrate_to_level(mtmp, ledger_no(&(stway->tolev)), MIGR_SSTAIRS,
                         (coord *) 0);
        return 2;
    case MUSE_TELEPORT_TRAP:
        m_flee(mtmp);
        t = t_at(gt.trapx, gt.trapy);
        if (vis) {
            pline_mon(mtmp, "%s %s onto a %s!", Monnam(mtmp),
                  vtense(fakename[0], locomotion(mtmp->data, "jump")),
                  trapname(t->ttyp, FALSE));
        }
        /* if trap was in a concealed niche, it's no longer concealed */
        reveal_trap(t, vis);
        /*  don't use rloc_to() because worm tails must "move" */
        remove_monster(mtmp->mx, mtmp->my);
        newsym(mtmp->mx, mtmp->my); /* update old location */
        place_monster(mtmp, gt.trapx, gt.trapy);
        if (mtmp->wormno)
            worm_move(mtmp);
        maybe_unhide_at(mtmp->mx, mtmp->my);
        newsym(gt.trapx, gt.trapy);
        /* 0: 'no object' rather than STRANGE_OBJECT; FALSE: obj not seen */
        m_tele(mtmp, vismon, FALSE, 0);
        return 2;
    case MUSE_POT_HEALING:
        if (!otmp)
            panic(MissingDefensiveItem, "potioh of healing");
        mquaffmsg(mtmp, otmp);
        i = (8 + d(4 + 2 * bcsign(otmp), 4))
                    / (otmp->odiluted ? 2 : 1);
        healmon(mtmp, i, !otmp->cursed && !otmp->odiluted ? 1 : 0);
        if (!otmp->cursed && !mtmp->mcansee) {
            mcureblindness(mtmp, vismon);
        } else if (otmp->blessed && (mtmp->mrabid || mtmp->mdiseased)) {
            if (vismon)
                pline("%s is no longer ill.", Monnam(mtmp));
            mtmp->mrabid = mtmp->mdiseased = 0;
        }
        if (vismon)
            pline_mon(mtmp, "%s looks better.", Monnam(mtmp));
        if (oseen)
            makeknown(POT_HEALING);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_POT_EXTRA_HEALING:
        if (!otmp)
            panic(MissingDefensiveItem, "potioh of extra healing");
        mquaffmsg(mtmp, otmp);
        i = (16 + d(4 + 2 * bcsign(otmp), 8))
            / (otmp->odiluted ? 2 : 1);
        healmon(mtmp, i, (otmp->blessed ? 5 : 2)
                             / (otmp->odiluted ? 2 : 1));
        if (!mtmp->mcansee) {
            mcureblindness(mtmp, vismon);
        } else if (!otmp->cursed && (mtmp->mrabid || mtmp->mdiseased)) {
            if (vismon)
                pline("%s is no longer ill.", Monnam(mtmp));
            mtmp->mrabid = mtmp->mdiseased = 0;
        }
        if (vismon)
            pline_mon(mtmp, "%s looks much better.", Monnam(mtmp));
        if (oseen)
            makeknown(POT_EXTRA_HEALING);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_POT_FULL_HEALING:
        if (!otmp)
            panic(MissingDefensiveItem, "potioh of full healing");
        mquaffmsg(mtmp, otmp);
        if (otmp->otyp == POT_SICKNESS)
            unbless(otmp); /* Pestilence */
        healmon(mtmp, mtmp->mhpmax, (otmp->blessed ? 8 : 4)
                                        / (otmp->odiluted ? 2 : 1));
        if (!mtmp->mcansee && otmp->otyp != POT_SICKNESS) {
            mcureblindness(mtmp, vismon);
        } else if (mtmp->mrabid || mtmp->mdiseased) {
            if (vismon)
                pline("%s is no longer ill.", Monnam(mtmp));
            mtmp->mrabid = mtmp->mdiseased = 0;
        }
        if (vismon)
            pline_mon(mtmp, "%s looks completely healed.", Monnam(mtmp));
        if (oseen)
            makeknown(otmp->otyp);
        m_useup(mtmp, otmp);
        return 2;

    case MUSE_POT_MILK:
        /* Cancels:
         * confusion, stunning,
         * protection, reflection, phasing,
         * invisibility, unpolymorphs
         */
        mquaffmsg(mtmp, otmp);
        mtmp->mhp += 1;
        if (mtmp->mhp > mtmp->mhpmax)
            mtmp->mhp = ++mtmp->mhpmax;
        if (!mtmp->mcansee) {
            mcureblindness(mtmp, vismon);
        }
        if (mtmp->mconf || mtmp->mstun) {
            mtmp->mconf = mtmp->mstun = 0;
            if (vismon)
                pline_mon(mtmp, "%s seems steadier now.", Monnam(mtmp));
        }
        if (mtmp->mprotection) {
            if (canseemon(mtmp))
                pline_The("%s haze around %s %s.",
                          hcolor(NH_GOLDEN), mon_nam(mtmp), "disappears");
            mtmp->mprotection = mtmp->mprottime = 0;
        }
        if (has_reflection(mtmp)) {
            if (canseemon(mtmp))
                pline("%s shimmering globe disappears.",
                      s_suffix(Monnam(mtmp)));
            mtmp->mextrinsics &= ~(MR2_REFLECTION);
            mtmp->mreflecttime = 0;
        }
        if (has_phasing(mtmp)) {
            if (canseemon(mtmp))
                pline("%s looks less hazy.", Monnam(mtmp));
            mtmp->mextrinsics &= ~(MR2_PHASING);
            mtmp->mphasetime = 0;
        }
        if (mtmp->minvis) {
            mtmp->minvis = mtmp->perminvis = 0;
            newsym(mtmp->mx, mtmp->my);
        }

        /* force shapeshifter into its base form or mimic to unhide */
        normal_shape(mtmp);

        if (oseen)
            makeknown(POT_MILK);
        m_useup(mtmp, otmp);
        return 2;

    case MUSE_POT_VAMPIRE_BLOOD:
        mquaffmsg(mtmp, otmp);
        if (!otmp->cursed) {
            i = rnd(8) + rnd(2);
            mtmp->mhp += i;
            mtmp->mhpmax += i;
            if (vismon)
                pline("%s looks full of life.", Monnam(mtmp));
        } else if (vismon)
            pline("%s discards the congealed blood in disgust.", Monnam(mtmp));
        if (oseen)
            makeknown(POT_VAMPIRE_BLOOD);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_LIZARD_CORPSE:
        if (!otmp)
            panic(MissingDefensiveItem, "lizard corpse");
        /* not actually called for its unstoning effect */
        mon_consume_unstone(mtmp, otmp, FALSE, mtmp->mstone ? TRUE : FALSE);
        return 2;
    case MUSE_EUCALYPTUS_LEAF:
        /* not actually called for its unstoning effect */
        mon_consume_unstone(mtmp, otmp, FALSE, FALSE);
        return 2;
    case MUSE_POT_RESTORE_ABILITY:
        mquaffmsg(mtmp, otmp);
        mtmp->mcan = 0;
        if (canseemon(mtmp))
            pline("%s looks revitalized.", Monnam(mtmp));
        if (oseen)
            makeknown(otmp->otyp);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_POT_PHASING:
        mquaffmsg(mtmp, otmp);
        mtmp->mextrinsics |= MR2_PHASING;
        mtmp->mphasetime += rn1(50, otmp->odiluted ? 100 : 250);
        if (canseemon(mtmp))
            pline("%s turns hazy!", Monnam(mtmp));
        if (oseen)
            makeknown(otmp->otyp);
        m_useup(mtmp, otmp);
        return 2;
    case 0:
        return 0; /* i.e. an exploded wand */
    default:
        impossible("%s wanted to perform action %d?", Monnam(mtmp),
                   gm.m.has_defense);
        break;
    }
    return 0;
#undef m_flee
}

int
rnd_defensive_item(struct monst *mtmp)
{
    struct permonst *pm = mtmp->data;
    int difficulty = mons[(monsndx(pm))].difficulty;
    int trycnt = 0;

    if (is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
        || pm->mlet == S_GHOST || pm->mlet == S_KOP)
        return 0;
 try_again:
    switch (rn2(8 + (difficulty > 3) + (difficulty > 6) + (difficulty > 8))) {
    case 6:
    case 9:
        if (noteleport_level(mtmp) && ++trycnt < 2)
            goto try_again;
        if (!rn2(3))
            return WAN_TELEPORTATION;
        FALLTHROUGH;
        /*FALLTHRU*/
    case 0:
    case 1:
        return SCR_TELEPORTATION;
    case 8:
    case 10:
        if (!rn2(3))
            return WAN_CREATE_MONSTER;
        FALLTHROUGH;
        /*FALLTHRU*/
    case 2:
        return SCR_CREATE_MONSTER;
    case 3:
        return rn2(7) ? POT_HEALING : POT_MILK;
    case 4:
        return rn2(11) ? POT_EXTRA_HEALING : POT_MILK;
    case 5:
        return (mtmp->data != &mons[PM_PESTILENCE]) ? POT_FULL_HEALING
                                                    : POT_SICKNESS;
    case 7: /* wand of digging */
        /* usually avoid digging in Sokoban */
        if (Sokoban && rn2(4))
            goto try_again;
        /* some creatures shouldn't dig down to another level when hurt */
        if (mon_prop(mtmp, LEVITATION) || mtmp->isshk || mtmp->isgd || mtmp->ispriest)
            return 0;
        return WAN_DIGGING;
    }
    /*NOTREACHED*/
    return 0;
}


staticfn boolean
linedup_chk_corpse(coordxy x, coordxy y)
{
    return (sobj_at(CORPSE, x, y) != 0);
}

staticfn void
m_use_undead_turning(struct monst *mtmp, struct obj *obj)
{
    coordxy ax = u.ux + sgn(mtmp->mux - mtmp->mx) * 3,
            ay = u.uy + sgn(mtmp->muy - mtmp->my) * 3;
    coordxy bx = mtmp->mx, by = mtmp->my;

    if (!(obj->otyp == WAN_UNDEAD_TURNING))
        return;

    /* not necrophiliac(); unlike deciding whether to pick this
       type of wand up, we aren't interested in corpses within
       carried containers until they're moved into open inventory;
       we don't check whether hero is poly'd into an undead--the
       wand's turning effect is too weak to be a useful direct
       attack--only whether hero is carrying at least one corpse */
    if (carrying(CORPSE)
        /*
         * If hero is carrying one or more corpses but isn't wielding
         * a cockatrice corpse (unless being hit by one won't do
         * the monster much harm); otherwise we'd be using this wand
         * as a defensive item with higher priority.
         *
         * Might be cockatrice intended as a weapon (or being denied
         * to glove-wearing monsters for use as a weapon) or lizard
         * intended as a cure or lichen intended as veggy food or
         * sacrifice fodder being lugged to an altar.  Zapping with
         * this will deprive hero of one from each stack although
         * they might subsequently be recovered after killing again.
         * In the sacrifice fodder case, it could even be to the
         * player's advantage (fresher corpse if a new one gets
         * dropped; player might not choose to spend a wand charge
         * on that when/if hero acquires this wand).
         */
        || linedup_callback(ax, ay, bx, by, linedup_chk_corpse)
        /* or there's a corpse on the ground in a direct line from the
           monster to the hero, and up to 3 steps beyond. */
        ) {
        gm.m.offensive = obj;
        gm.m.has_offense = MUSE_WAN_UNDEAD_TURNING;
    }
}

/* from monster's point of view, is hero behind a chokepoint? */
staticfn boolean
hero_behind_chokepoint(struct monst *mtmp)
{
    coordxy dx = sgn(mtmp->mx - mtmp->mux);
    coordxy dy = sgn(mtmp->my - mtmp->muy);

    coordxy x = mtmp->mux + dx;
    coordxy y = mtmp->muy + dy;

    int dir = xytod(dx, dy);
    int dir_l = DIR_CLAMP(DIR_LEFT2(dir));
    int dir_r = DIR_CLAMP(DIR_RIGHT2(dir));

    coord c1, c2;

    dtoxy(&c1, dir_l);
    dtoxy(&c2, dir_r);
    c1.x += x, c2.x += x;
    c1.y += y, c2.y += y;

    if ((!isok(c1.x, c1.y) || !accessible(c1.x, c1.y))
        && (!isok(c2.x, c2.y) || !accessible(c2.x, c2.y)))
        return TRUE;
    return FALSE;
}

/* hostile monster has another hostile next to it */
staticfn boolean
mon_has_friends(struct monst *mtmp)
{
    coordxy dx, dy;
    struct monst *mon2;

    if (mtmp->mtame || mtmp->mpeaceful)
        return FALSE;

    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++) {
            coordxy x = mtmp->mx + dx;
            coordxy y = mtmp->my + dy;

            if (isok(x, y) && (mon2 = m_at(x, y)) != 0
                && mon2 != mtmp
                && !mon2->mtame && !mon2->mpeaceful)
                return TRUE;
        }

    return FALSE;
}

/* Select an offensive item/action for a monster.  Returns TRUE iff one is
 * found.
 */
boolean
find_offensive(struct monst *mtmp)
{
    struct obj *obj, *mtmp_helmet;
    boolean reflection_skip = m_seenres(mtmp, M_SEEN_REFL) != 0
        || monnear(mtmp, mtmp->mux, mtmp->muy);

    gm.m.offensive = (struct obj *) 0;
    gm.m.has_offense = 0;
    if (mtmp->mpeaceful || is_animal(mtmp->data) || mindless(mtmp->data)
        || nohands(mtmp->data))
        return FALSE;
    if (u.uswallow)
        return FALSE;
    if (in_your_sanctuary(mtmp, 0, 0))
        return FALSE;
    if (dmgtype(mtmp->data, AD_HEAL)
        && !uwep && !uarmu && !uarm && !uarmh
        && !uarms && !uarmg && !uarmc && !uarmf)
        return FALSE;
    /* all offensive items require orthogonal or diagonal targeting */
    if (!lined_up(mtmp))
        return FALSE;

#define nomore(x)       if (gm.m.has_offense == x) continue;
    reflection_skip = (m_seenres(mtmp, M_SEEN_REFL) != 0
                       || monnear(mtmp, mtmp->mux, mtmp->muy));
    mtmp_helmet = which_armor(mtmp, W_ARMH);
    /* this picks the last viable item rather than prioritizing choices */
    for (obj = mtmp->minvent; obj; obj = obj->nobj) {
        int otyp = obj->otyp;
        boolean can_zap = obj->oclass == WAND_CLASS || otyp == SCR_ZAPPING;

        if (otyp == SCR_ZAPPING)
            otyp = obj->corpsenm;

        if (!reflection_skip) {
            nomore(MUSE_WAN_DEATH);
            if (otyp == WAN_DEATH && can_zap
                && !m_seenres(mtmp, M_SEEN_MAGR)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_DEATH;
            }
            nomore(MUSE_WAN_DRAINING);
            if (otyp == WAN_DRAINING && can_zap
                && !resists_drain(gy.youmonst.data)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_DRAINING;
            }
            nomore(MUSE_WAN_SLEEP);
            if (otyp == WAN_SLEEP && can_zap && gm.multi >= 0
                && !m_seenres(mtmp, M_SEEN_SLEEP)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_SLEEP;
            }
            nomore(MUSE_WAN_CORROSION);
            if (otyp == WAN_CORROSION && can_zap
                && !m_seenres(mtmp, M_SEEN_ACID)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_CORROSION;
            }
            nomore(MUSE_WAN_FIRE);
            if (otyp == WAN_FIRE && can_zap
                && !m_seenres(mtmp, M_SEEN_FIRE)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_FIRE;
            }
            nomore(MUSE_FIRE_HORN);
            if (otyp == FIRE_HORN && obj->spe > 0 && can_blow(mtmp)
                && !m_seenres(mtmp, M_SEEN_FIRE)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_FIRE_HORN;
            }
            nomore(MUSE_WAN_COLD);
            if (otyp == WAN_COLD && can_zap
                && !m_seenres(mtmp, M_SEEN_COLD)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_COLD;
            }
            nomore(MUSE_FROST_HORN);
            if (otyp == FROST_HORN && obj->spe > 0 && can_blow(mtmp)
                && !m_seenres(mtmp, M_SEEN_COLD)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_FROST_HORN;
            }
            nomore(MUSE_WAN_LIGHTNING);
            if (otyp == WAN_LIGHTNING && can_zap
                && !m_seenres(mtmp, M_SEEN_ELEC)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_LIGHTNING;
            }
            nomore(MUSE_WAN_POISON_GAS);
            if (otyp == WAN_POISON_GAS && can_zap
                && !m_seenres(mtmp, M_SEEN_POISON)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_POISON_GAS;
            }
            nomore(MUSE_WAN_MAGIC_MISSILE);
            if (otyp == WAN_MAGIC_MISSILE && can_zap
                && !m_seenres(mtmp, M_SEEN_MAGR)) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_WAN_MAGIC_MISSILE;
            }
        }
        nomore(MUSE_WAN_UNDEAD_TURNING);
        m_use_undead_turning(mtmp, obj);
        nomore(MUSE_WAN_STRIKING);
        if (otyp == WAN_STRIKING && can_zap
            && !m_seenres(mtmp, M_SEEN_MAGR)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_WAN_STRIKING;
        }
        nomore(MUSE_WAN_CANCELLATION);
        if (otyp == WAN_CANCELLATION && can_zap) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_WAN_CANCELLATION;
        }
        nomore(MUSE_WAN_TELEPORTATION);
        if (otyp == WAN_TELEPORTATION && can_zap
            /* don't give controlled hero a free teleport */
            && !Teleport_control
            /* same hack as MUSE_WAN_TELEPORTATION_SELF */
            && (!noteleport_level(mtmp)
                || !mon_knows_traps(mtmp, TELEP_TRAP))
            /* do try to move hero to a more vulnerable spot */
            && (onscary(u.ux, u.uy, mtmp)
                || (hero_behind_chokepoint(mtmp) && mon_has_friends(mtmp))
                || stairway_at(u.ux, u.uy))) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_WAN_TELEPORTATION;
        }
        nomore(MUSE_WAN_SLOW_MONSTER);
        if (otyp == WAN_SLOW_MONSTER && can_zap
            && HFast) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_WAN_SLOW_MONSTER;
        }
        nomore(MUSE_WAN_WONDER);
        if (otyp == WAN_WONDER && can_zap) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_WAN_WONDER;
        }
        nomore(MUSE_POT_PARALYSIS);
        if (otyp == POT_PARALYSIS && gm.multi >= 0) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_PARALYSIS;
        }
        nomore(MUSE_POT_BLINDNESS);
        if (otyp == POT_BLINDNESS && !attacktype(mtmp->data, AT_GAZE)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_BLINDNESS;
        }
        nomore(MUSE_SCR_CLONING);
        if (otyp == SCR_CLONING
            && distu(mtmp->mx, mtmp->my) < (7*7)
            && mtmp->mcansee && haseyes(mtmp->data)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_SCR_CLONING;
        }
        nomore(MUSE_SCR_STINKING_CLOUD)
        if (otyp == SCR_STINKING_CLOUD
            && m_canseeu(mtmp) && haseyes(mtmp->data)
            && mcast_dist_ok(mtmp) && !m_seenres(mtmp, M_SEEN_POISON)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_SCR_STINKING_CLOUD;
        }
        nomore(MUSE_POT_HALLUCINATION);
        if (otyp == POT_HALLUCINATION && !attacktype(mtmp->data, AT_GAZE)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_HALLUCINATION;
        }
        nomore(MUSE_POT_POLYMORPH_THROW);
        if (otyp == POT_POLYMORPH
            && !m_seenres(mtmp, M_SEEN_MAGR)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_POLYMORPH_THROW;
        }
        nomore(MUSE_POT_CONFUSION);
        if (otyp == POT_CONFUSION) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_CONFUSION;
        }
        nomore(MUSE_POT_SLEEPING);
        if (otyp == POT_SLEEPING
            && !m_seenres(mtmp, M_SEEN_SLEEP)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_SLEEPING;
        }
        nomore(MUSE_POT_ACID);
        if (otyp == POT_ACID
            && !m_seenres(mtmp, M_SEEN_ACID)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_ACID;
        }
        nomore(MUSE_POT_OIL);
        if (otyp == POT_OIL
            && !m_seenres(mtmp, M_SEEN_FIRE)
            /* don't throw oil if point-blank AND mtmp is low on HP AND mtmp is
             * not fire resistant */
            && (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) > 2
                || mtmp->mhp > 10
                || resists_fire(mtmp))) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_POT_OIL;
        }
        nomore(MUSE_MAGIC_FLUTE);
        if (otyp == MAGIC_FLUTE && !u.usleep && !rn2(3)
            && obj->spe > 0 && gm.multi >= 0) {
            int dist = dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy);
            if (dist <= mtmp->m_lev * 5) {
                gm.m.offensive = obj;
                gm.m.has_offense = MUSE_MAGIC_FLUTE;
            }
        }
        /* we can safely put this scroll here since the locations that
         * are in a 1 square radius are a subset of the locations that
         * are in wand or throwing range (in other words, always lined_up())
         */
        nomore(MUSE_SCR_EARTH);
        if (otyp == SCR_EARTH
            && (hard_helmet(mtmp_helmet) || mtmp->mconf
                || amorphous(mtmp->data) || passes_walls(mtmp->data)
                || noncorporeal(mtmp->data) || unsolid(mtmp->data)
                || !rn2(10))
            && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 2
            && mtmp->mcansee && haseyes(mtmp->data)
            && !Is_rogue_level(&u.uz)
            && (!In_endgame(&u.uz) || Is_earthlevel(&u.uz))) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_SCR_EARTH;
        }
        nomore(MUSE_CAMERA);
        if (otyp == EXPENSIVE_CAMERA
            && ((!Blind
                && !(resists_blnd(&gy.youmonst)
                || defended(&gy.youmonst, AD_BLND)))
                || hates_light(gy.youmonst.data))
            && lined_up(mtmp)
            && obj->spe > 0 && !rn2(3)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_CAMERA;
        }
        nomore(MUSE_SCR_FIRE);
        if (otyp == SCR_FIRE
            && m_canseeu(mtmp) && haseyes(mtmp->data)
            && mcast_dist_ok(mtmp) && !m_seenres(mtmp, M_SEEN_FIRE)) {
            gm.m.offensive = obj;
            gm.m.has_offense = MUSE_SCR_FIRE;
        }
    }
    return (boolean) !!gm.m.has_offense;
#undef nomore
}

staticfn int
mbhitm(struct monst *mtmp, struct obj *otmp)
{
    int tmp;
    boolean reveal_invis = FALSE, learnit = FALSE,
            hits_you = (mtmp == &gy.youmonst);
    boolean zapcard = gc.current_wand
                   && gc.current_wand->otyp == SCR_ZAPPING;
    const char *otxt = zapcard ? "card" : "wand";

    if (!hits_you && otmp->otyp != WAN_UNDEAD_TURNING) {
        mtmp->msleeping = 0;
        if (mtmp->m_ap_type)
            seemimic(mtmp);
    }
    switch (otmp->otyp) {
    case WAN_STRIKING: {
        int orig_dmg;
        reveal_invis = TRUE;
        if (hits_you) {
            orig_dmg = tmp = d(2, 12);
            if (Antimagic) {
                monstseesu(M_SEEN_MAGR); /* monsters notice hero resisting */
                shieldeff(u.ux, u.uy);
                Soundeffect(se_boing, 40);
                pline("Boing!");
                learnit = TRUE;
                tmp = 0;
            } else if (rnd(20) < 10 + u.uac) {
                monstunseesu(M_SEEN_MAGR); /* mons see hero not resisting */
                pline_The("%s hits you!", otxt);
                if (Half_spell_damage)
                    tmp -= (tmp + 1) / 4;
                (void) destroy_items(&gy.youmonst, AD_PHYS, orig_dmg);
                losehp(tmp, otxt, KILLED_BY_AN);
                learnit = TRUE;
            } else {
                pline_The("%s misses you.", otxt);
            }
            stop_occupation();
            nomul(0);
        } else if (resists_magm(mtmp)) {
            shieldeff(mtmp->mx, mtmp->my);
            Soundeffect(se_boing, 40);
            pline("Boing!");
            learnit = TRUE;
        } else if (rnd(20) < 10 + find_mac(mtmp)) {
            orig_dmg = tmp = d(2, 12);
            hit(otxt, mtmp, exclam(tmp));
            (void) resist(mtmp, otmp->oclass, tmp, TELL);
            tmp += destroy_items(mtmp, AD_PHYS, orig_dmg);
            learnit = TRUE;
        } else {
            miss(otxt, mtmp);
        }
        /* need to see the wand being zapped and also the spot where the
           target is hit; don't have to see the target itself though */
        if (learnit && gz.zap_oseen && !zapcard
            && (hits_you || cansee(mtmp->mx, mtmp->my)))
            makeknown(WAN_STRIKING);
        break;
    }
    case WAN_TELEPORTATION:
        if (hits_you) {
            tele();
            if (gz.zap_oseen)
                makeknown(WAN_TELEPORTATION);
        } else {
            /* for consistency with zap.c, don't identify */
            if (mtmp->ispriest && *in_rooms(mtmp->mx, mtmp->my, TEMPLE)) {
                if (cansee(mtmp->mx, mtmp->my))
                    pline_mon(mtmp, "%s resists the magic!", Monnam(mtmp));
            } else if (!tele_restrict(mtmp))
                (void) rloc(mtmp, RLOC_MSG);
        }
        break;
    case WAN_CANCELLATION:
    case SPE_CANCELLATION:
        (void) cancel_monst(mtmp, otmp, FALSE, TRUE, FALSE);
        break;
    case WAN_UNDEAD_TURNING:
        if (hits_you) {
            unturn_you();
            learnit = gz.zap_oseen;
        } else {
            boolean wake = FALSE;

            if (unturn_dead(mtmp)) /* affects mtmp's invent, not mtmp */
                wake = TRUE;
            if (is_undead(mtmp->data) || is_vampshifter(mtmp)) {
                wake = reveal_invis = TRUE;
                /* context.bypasses=True: if resist() happens to be fatal,
                   make_corpse() will set obj->bypass on the new corpse
                   so that mbhito() will skip it instead of reviving it */
                svc.context.bypasses = TRUE; /* for make_corpse() */
                (void) resist(mtmp, WAND_CLASS, rnd(8), NOTELL);
            }
            if (wake) {
                if (!DEADMONSTER(mtmp))
                    wakeup(mtmp, FALSE);
                learnit = gz.zap_oseen;
            }
        }
        if (learnit)
            makeknown(WAN_UNDEAD_TURNING);
        break;
     case WAN_SLOW_MONSTER:
        if (hits_you) {
            u_slow_down();
            learnit = gz.zap_oseen;
        }
        if (learnit)
            makeknown(WAN_SLOW_MONSTER);
        break;
    case WAN_POLYMORPH:
        if (hits_you) {
            if (Antimagic) {
                shieldeff(u.ux, u.uy);
                You_feel("momentarily different.");
                monstseesu(M_SEEN_MAGR);
                if (gz.zap_oseen)
                    makeknown(otmp->otyp);
            } else if (!Unchanging) {
                if (gz.zap_oseen)
                    makeknown(otmp->otyp);
                polyself(FALSE);
            }
        } else if (resists_magm(mtmp) || defended(mtmp, AD_MAGM)) {
            /* magic resistance protects from polymorph traps, so make
               it guard against involuntary polymorph attacks too... */
            shieldeff(mtmp->mx, mtmp->my);
        } else if (!resist(mtmp, otmp->oclass, 0, NOTELL)) {
            struct obj *obj;
            /* dropped inventory shouldn't be hit by this zap */
            for (obj = mtmp->minvent; obj; obj = obj->nobj)
                bypass_obj(obj);
            /* natural shapechangers aren't affected by system shock
               (unless protection from shapechangers is interfering
               with their metabolism...) */
            if (!is_shapeshifter(mtmp->data) && !rn2(25)) {
                if (canseemon(mtmp)) {
                    pline("%s shudders!", Monnam(mtmp));
                    if (gz.zap_oseen)
                        makeknown(otmp->otyp);
                }
                if (canseemon(mtmp))
                    pline("%s is killed!", Monnam(mtmp));
                mtmp->mhp = 0;
                if (DEADMONSTER(mtmp))
                    mondied(mtmp);
            } else if (newcham(mtmp, (struct permonst *) 0, NC_SHOW_MSG)) {
                if (!Hallucination && gz.zap_oseen)
                    makeknown(otmp->otyp);
            }
        }
        break;
    default:
        break;
    }
    if (reveal_invis && !DEADMONSTER(mtmp)
        && cansee(gb.bhitpos.x, gb.bhitpos.y) && !canspotmon(mtmp))
        map_invisible(gb.bhitpos.x, gb.bhitpos.y);

    return 0;
}

/* hit all objects at x,y with fhito function */
staticfn boolean
fhito_loc(struct obj *obj,
          coordxy tx, coordxy ty,
          int (*fhito)(OBJ_P, OBJ_P))
{
    struct obj *otmp, *next_obj;
    int hitanything = 0;

    if (!fhito || !OBJ_AT(tx, ty))
        return FALSE;

    for (otmp = svl.level.objects[tx][ty]; otmp; otmp = next_obj) {
        next_obj = otmp->nexthere;

        if (otmp->where != OBJ_FLOOR || otmp->ox != tx || otmp->oy != ty)
            continue;
        hitanything += (*fhito)(otmp, obj);
    }

    return hitanything ? TRUE : FALSE;
}

/* A modified bhit() for monsters.  Based on bhit() in zap.c.  Unlike
 * buzz(), bhit() doesn't take into account the possibility of a monster
 * zapping you, so we need a special function for it.  (Unless someone wants
 * to merge the two functions...)
 */
staticfn void
mbhit(
    struct monst *mon,            /* monster shooting the wand */
    int range,                    /* direction and range */
    int (*fhitm)(MONST_P, OBJ_P), /* must be non-Null */
    int (*fhito)(OBJ_P, OBJ_P),   /* fns called when mon/obj hit */
    struct obj *obj)              /* 2nd arg to fhitm/fhito */
{
    struct monst *mtmp;
    struct trap *ttmp;
    uchar ltyp;
    int ddx, ddy, otyp = obj->otyp;

    /* Monsters in pits can only zap a range of 1. */
    if (mon->mtrapped) {
        ttmp = t_at(mon->mx, mon->my);
        if (ttmp && is_pit(ttmp->ttyp))
            range = 1;
    }

    gb.bhitpos.x = mon->mx;
    gb.bhitpos.y = mon->my;
    ddx = sgn(mon->mux - mon->mx);
    ddy = sgn(mon->muy - mon->my);

    while (range-- > 0) {
        coordxy x, y, dbx, dby;

        gb.bhitpos.x += ddx;
        gb.bhitpos.y += ddy;
        x = gb.bhitpos.x;
        y = gb.bhitpos.y;

        if (!isok(x, y)) {
            gb.bhitpos.x -= ddx;
            gb.bhitpos.y -= ddy;
            break;
        }
        if (u_at(gb.bhitpos.x, gb.bhitpos.y)) {
            (*fhitm)(&gy.youmonst, obj);
            range -= 3;
        } else if ((mtmp = m_at(gb.bhitpos.x, gb.bhitpos.y)) != 0) {
            if (cansee(gb.bhitpos.x, gb.bhitpos.y) && !canspotmon(mtmp))
                map_invisible(gb.bhitpos.x, gb.bhitpos.y);
            (*fhitm)(mtmp, obj);
            range -= 3;
        }
        if (fhito_loc(obj, gb.bhitpos.x, gb.bhitpos.y, fhito))
            range--;
        ltyp = levl[gb.bhitpos.x][gb.bhitpos.y].typ;
        dbx = x, dby = y;
        if (otyp == WAN_STRIKING
            /* if levl[x][y].typ is DRAWBRIDGE_UP then the zap is passing
               over the moat in front of a closed drawbridge and doesn't
               hit any part of the bridge's mechanism (yet; it might be
               about to hit the closed portcullis on the next iteration) */
            && ltyp != DRAWBRIDGE_UP && find_drawbridge(&dbx, &dby)) {
            /* this might kill mon and destroy obj but they'll remain
               accessible; (*fhitm)() and (*fhito)() use obj for zap type */
            destroy_drawbridge(dbx, dby);
        } else if (IS_DOOR(ltyp) || ltyp == SDOOR) {
            switch (otyp) {
            /* note: monsters don't use opening or locking magic
               at present, but keep these as placeholders */
            case WAN_OPENING:
            case WAN_LOCKING:
            case WAN_STRIKING:
                if (doorlock(obj, gb.bhitpos.x, gb.bhitpos.y)) {
                    if (gz.zap_oseen)
                        makeknown(otyp);
                    /* if a shop door gets broken, add it to
                       the shk's fix list (no cost to player) */
                    if (levl[gb.bhitpos.x][gb.bhitpos.y].doormask == D_BROKEN
                        && *in_rooms(gb.bhitpos.x, gb.bhitpos.y, SHOPBASE))
                        add_damage(gb.bhitpos.x, gb.bhitpos.y, 0L);
                }
                break;
            }
        }
        if (!ZAP_POS(ltyp)
            || (IS_DOOR(ltyp) && (levl[gb.bhitpos.x][gb.bhitpos.y].doormask
                                  & (D_LOCKED | D_CLOSED)))) {
            gb.bhitpos.x -= ddx;
            gb.bhitpos.y -= ddy;
            break;
        }
        maybe_explode_trap(t_at(x, y), obj, &gz.zap_oseen);
        /* note: ttmp might be now gone */
    }
}

/* Perform an offensive action for a monster.  Must be called immediately
 * after find_offensive().  Return values are same as use_defensive().
 */
int
use_offensive(struct monst *mtmp)
{
    struct obj *otmp = gm.m.offensive;
    int i, maxdmg = 0;
    int otyp = otmp->otyp;
    boolean oseen;
    boolean zapcard = otmp->otyp == SCR_ZAPPING;
    struct attack* mattk;

    /* offensive potions are not drunk, they're thrown */
    if (otmp->oclass != POTION_CLASS && (i = precheck(mtmp, otmp)) != 0)
        return i;
    oseen = canseemon(mtmp);

    /* Handle wand of wonder */
    boolean wonder = gm.m.has_offense == MUSE_WAN_WONDER;
    if (wonder)
        otyp = muse_wonder();

    if (zapcard)
        otyp = otmp->corpsenm;

    /* From SporkHack (modified): some monsters would be better served if they
       were to melee attack instead using whatever offensive item they possess
       (read: master mind flayer zapping a wand of striking at the player repeatedly
       while in melee range). If the monster has an attack that is potentially
       better than its offensive item, or if it's wielding an artifact, and they're
       in melee range, don't give priority to the offensive item */
    for (i = 0; i < NATTK; i++) {
        mattk = &mtmp->data->mattk[i];
        /* total up the possible damage for just swinging */
        maxdmg += mattk->damn * mattk->damd;
    }

    /* If the monsters' combined damage from a melee attack exceeds 21
       (the average of an offensive wands 6d6), or if
       their wielded weapon is an artifact, use it if close enough. Exception
       being certain wands that can incapacitate or can already do significant
       damage. Because intelligent monsters know not to use a certain attack if
       they've seen that the player is resistant to it, the monster will switch
       offensive items appropriately */
    if ((maxdmg > 21
         || (MON_WEP(mtmp) && MON_WEP(mtmp)->oartifact))
        && (monnear(mtmp, mtmp->mux, mtmp->muy)
            && gm.m.has_offense != MUSE_WAN_DEATH
            && gm.m.has_offense != MUSE_WAN_SLEEP)) {
        return 0;
    }

    switch (gm.m.has_offense) {
    case MUSE_WAN_DEATH:
    case MUSE_WAN_DRAINING:
    case MUSE_WAN_SLEEP:
    case MUSE_WAN_FIRE:
    case MUSE_WAN_COLD:
    case MUSE_WAN_LIGHTNING:
    case MUSE_WAN_MAGIC_MISSILE:
    case MUSE_WAN_CORROSION:
    case MUSE_WAN_POISON_GAS:
    case MUSE_WAN_STUNNING:
        if (!mzapwand(mtmp, otmp, FALSE))
            return 2;
        if (oseen && !zapcard)
            makeknown(otmp->otyp);
        gm.m_using = TRUE;
        gc.current_wand = otmp;
        gb.buzzer = mtmp;

        /* Could be fatal to mon, useup before it could be dropped. */
        if (zapcard)
            m_useup(mtmp, otmp);

        buzz(BZ_M_WAND(BZ_OFS_WAN(otyp)),
             (otyp == WAN_MAGIC_MISSILE || otyp == (WAN_DRAINING + 1))
                ? 2 : 6, mtmp->mx, mtmp->my,
             sgn(mtmp->mux - mtmp->mx), sgn(mtmp->muy - mtmp->my));
        gb.buzzer = 0;
        gc.current_wand = 0;
        gm.m_using = FALSE;
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    case MUSE_FIRE_HORN:
    case MUSE_FROST_HORN:
        mplayhorn(mtmp, otmp, FALSE);
        gm.m_using = TRUE;
        gb.buzzer = mtmp;
        gc.current_wand = otmp; /* needed by zhitu() */
        buzz(BZ_M_WAND(BZ_OFS_AD((otmp->otyp == FROST_HORN) ? AD_COLD
                                                            : AD_FIRE)),
             rn1(6, 6), mtmp->mx, mtmp->my, sgn(mtmp->mux - mtmp->mx),
             sgn(mtmp->muy - mtmp->my));
        gb.buzzer = 0;
        gc.current_wand = 0;
        gm.m_using = FALSE;
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    case MUSE_MAGIC_FLUTE: {
        const char* music = Hallucination ? "piping" : "soft";
        if (oseen) {
            makeknown(otmp->otyp);
            pline("%s plays a %s!", Monnam(mtmp), xname(otmp));
            if (!Deaf)
                pline("%s produces %s music.", Monnam(mtmp), music);
        } else {
            You_hear("%s music being played.", music);
        }
        gm.m_using = TRUE;
        put_monsters_to_sleep(mtmp, mtmp->m_lev * 5);
        if (!Deaf) {
            /* No effects on you if you can't hear the music, but the monster
             * doesn't know that so it won't be prevented from trying. */
            if (fully_resistant(SLEEP_RES))
                You("yawn.");
            else {
                You("fall asleep.");
                /* sleep time is same as put_monsters_to_sleep */
                fall_asleep(-d(10, 10), TRUE);
            }
        }
        otmp->spe--;
        gm.m_using = FALSE;
        return 2;
    }
    case MUSE_WAN_TELEPORTATION:
    case MUSE_WAN_UNDEAD_TURNING:
    case MUSE_WAN_STRIKING:
    case MUSE_WAN_CANCELLATION:
    case MUSE_WAN_SLOW_MONSTER:
    case MUSE_WAN_POLYMORPH:
    {
        struct obj pseudo;
        pseudo = cg.zeroobj;

        gz.zap_oseen = oseen;
        if (!mzapwand(mtmp, otmp, FALSE))
            return 2;
        gm.m_using = TRUE;
        if (wonder)
            otmp->otyp = otyp;

        /* This is ugly but it's cleaner than the alternative */
        if (zapcard) {
            pseudo.otyp = otyp;
            gc.current_wand = otmp; /* For wand of striking descriptions */
        }

        mbhit(mtmp, rn1(8, 6), mbhitm, bhito, zapcard ? &pseudo : otmp);
        /* note: 'otmp' might have been destroyed (drawbridge destruction) */
        gm.m_using = FALSE;
        if (wonder)
            otmp->otyp = WAN_WONDER;

        if (zapcard) {
            m_useup(mtmp, otmp);
            gc.current_wand = 0;
        }
        return 2;
    }
    case MUSE_SCR_EARTH: {
        /* TODO: handle steeds */
        coordxy x, y;
        /* don't use monster fields after killing it */
        boolean confused = (mtmp->mconf ? TRUE : FALSE);
        int mmx = mtmp->mx, mmy = mtmp->my;
        boolean is_cursed = otmp->cursed, is_blessed = otmp->blessed;

        mreadmsg(mtmp, otmp);
        /* Identify the scroll */
        if (canspotmon(mtmp)) {
            pline_The("%s rumbles %s %s!", ceiling(mtmp->mx, mtmp->my),
                      otmp->blessed ? "around" : "above", mon_nam(mtmp));
            if (oseen)
                makeknown(otmp->otyp);
        } else if (cansee(mtmp->mx, mtmp->my)) {
            pline_The("%s rumbles in the middle of nowhere!",
                      ceiling(mtmp->mx, mtmp->my));
            if (mtmp->minvis)
                map_invisible(mtmp->mx, mtmp->my);
            if (oseen)
                makeknown(otmp->otyp);
        }

        /* could be fatal to monster, so use up the scroll before
           there's a chance that monster's inventory will be dropped */
        m_useup(mtmp, otmp);

        /* Loop through the surrounding squares */
        for (x = mmx - 1; x <= mmx + 1; x++) {
            for (y = mmy - 1; y <= mmy + 1; y++) {
                /* Is this a suitable spot? */
                if (isok(x, y) && !closed_door(x, y)
                    && !IS_OBSTRUCTED(levl[x][y].typ) && !IS_AIR(levl[x][y].typ)
                    && (((x == mmx) && (y == mmy)) ? !is_blessed : !is_cursed)
                    && (x != u.ux || y != u.uy)) {
                    (void) drop_boulder_on_monster(x, y, confused, FALSE);
                }
            }
        }
        /* Attack the player */
        if (distmin(mmx, mmy, u.ux, u.uy) == 1 && !is_cursed) {
            drop_boulder_on_player(confused, !is_cursed, FALSE, TRUE);
        }

        return (DEADMONSTER(mtmp)) ? 1 : 2;
    } /* case MUSE_SCR_EARTH */
    case MUSE_CAMERA: {
        if (Blind)
            return 0;

        if (Hallucination) {
            SetVoice(mtmp, 0, 80, 0);
            verbalize("Say cheese!");
        } else if (m_next2u(mtmp)) {
            pline("%s takes a picture of you with %s!",
                  Monnam(mtmp), an(xname(otmp)));
        } else {
             pline("%s takes a picture of you!", Monnam(mtmp));
        }
        gm.m_using = TRUE;
        if (!Blind && !(resists_blnd(&gy.youmonst)
                || defended(&gy.youmonst, AD_BLND))) {
            You("are blinded by the flash of light!");
            make_blinded(BlindedTimeout + (long) rnd(1 + 50), FALSE);
        }
        lightdamage(otmp, TRUE, 5);
        gm.m_using = FALSE;
        otmp->spe--;
        return 1;
    } /* case MUSE_CAMERA */
    case MUSE_SCR_CLONING: {
        /* We won't bother with confused - scrolls of cloning always clone
         * the monster iteself. */
        struct monst *mtmp2;
        boolean vis = cansee(mtmp->mx, mtmp->my);
        mreadmsg(mtmp, otmp);
        /* Rodney can use this - but follows same rules as Double Trouble */
        if (mtmp->iswiz && svc.context.no_of_wizards == 1) {
            pline("Double Trouble...");
            clonewiz();
        } else if (mtmp->iscthulhu) {
            pline_mon(mtmp, "%s trembles violently then explodes!", Monnam(mtmp));
            mtmp->mhp = 0;
            monkilled(mtmp, "", AD_PHYS);
        } else if (type_is_pname(mtmp->data)) {
            /* Other uniques - no limit. */
            if ((mtmp2 = makemon(&mons[mtmp->mnum],
                    mtmp->mx, mtmp->my, MM_NOWAIT)) != 0) {
                mtmp2->msleeping = mtmp2->mtame = mtmp2->mpeaceful = 0;
                pline("%s multiplies!", Monnam(mtmp));
            }
        } else if (clone_mon(mtmp, 0, 0) && vis)
            pline("%s multiplies!", Monnam(mtmp));
        m_useup(mtmp, otmp);
        if (oseen)
            makeknown(SCR_CLONING);
        return 2;
    } /* case MUSE_SCR_CLONING */
    case MUSE_SCR_FIRE: {
        /* Similar to the scroll of stinking cloud, we simply hand wave and
         * let monsters read this as if it were blessed. */
        boolean vis = cansee(mtmp->mx, mtmp->my);
        int dam, cval;
        cval = bcsign(otmp);
        dam = (2 * (rn1(3, 3) + 2 * cval) + 1) / 3;;
        mreadmsg(mtmp, otmp);
        m_useup(mtmp, otmp);
        if (mtmp->mconf) {
            if (vis)
                pline("Oh, what a pretty fire!");
            mtmp->mhp -= 1;
            if (DEADMONSTER(mtmp)) {
                if (vis)
                    pline_mon(mtmp, "%s dies!", Monnam(mtmp));
                mondead(mtmp);
            }
        } else
            explode(mtmp->mux, mtmp->muy, BZ_M_SPELL(ZT_FIRE), dam, SCROLL_CLASS, EXPL_FIERY);
        if (oseen)
            makeknown(SCR_FIRE);
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    } /* case MUSE_SCR_FIRE */
    case MUSE_POT_PARALYSIS:
    case MUSE_POT_BLINDNESS:
    case MUSE_POT_HALLUCINATION:
    case MUSE_POT_CONFUSION:
    case MUSE_POT_SLEEPING:
    case MUSE_POT_ACID:
    case MUSE_POT_OIL:
    case MUSE_POT_POLYMORPH_THROW:{
        /* Note: this setting of dknown doesn't suffice.  A monster
         * which is out of sight might throw and it hits something _in_
         * sight, a problem not existing with wands because wand rays
         * are not objects.  Also set dknown in mthrowu.c.
         */
        boolean isoil = (otmp->otyp == POT_OIL);
        struct obj *minvptr;
        if (cansee(mtmp->mx, mtmp->my)) {
            otmp->dknown = 1;
            pline_mon(mtmp, "%s hurls %s!",
                      Monnam(mtmp), singular(otmp, doname));
        }
        if (isoil && !otmp->lamplit && (!mtmp->mconf || rn2(3))) {
            /* A monster throwing oil probably wants it to explode; assume they
             * lit it just before throwing for simplicity;
             * a confused monster might forget to light it */
            begin_burn(otmp, FALSE);
        }
        m_throw(mtmp, mtmp->mx, mtmp->my, sgn(mtmp->mux - mtmp->mx),
                sgn(mtmp->muy - mtmp->my),
                distmin(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy), otmp);
        if (isoil) {
            /* Possible situation: monster lights and throws 1 of a stack of oil
             * point blank -> it explodes -> monster is caught in explosion ->
             * monster's remaining oil ignites and explodes -> otmp is no longer
             * valid. So we need to check whether otmp is still in monster's
             * inventory or not. */
            for (minvptr = mtmp->minvent; minvptr; minvptr = minvptr->nobj) {
                if (minvptr == otmp)
                    break;
            }
            if (minvptr == otmp && otmp->lamplit)
                end_burn(otmp, TRUE);
        }
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    }
    case MUSE_SCR_STINKING_CLOUD:
        mreadmsg(mtmp, otmp);
        if (oseen)
            makeknown(otmp->otyp);
        (void) create_gas_cloud(mtmp->mux, mtmp->muy, 15 + 10 * bcsign(otmp),
                        8 + 4 * bcsign(otmp));
        m_useup(mtmp, otmp);
        return (DEADMONSTER(mtmp)) ? 1 : 2;
    case MUSE_WAN_CREATE_MONSTER:
        return muse_createmonster(mtmp, otmp);
    case 0:
        return 0; /* i.e. an exploded wand */
    default:
        impossible("%s wanted to perform action %d?", Monnam(mtmp),
                   gm.m.has_offense);
        break;
    }
    return 0;
}

int
rnd_offensive_item(struct monst *mtmp)
{
    struct permonst *pm = mtmp->data;
    int difficulty = mons[(monsndx(pm))].difficulty;

    if (is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
        || pm->mlet == S_GHOST || pm->mlet == S_KOP)
        return 0;
    if (difficulty > 7 && !rn2(35))
        return WAN_DEATH;
    switch (rn2(9 - (difficulty < 4) + 4 * (difficulty > 6))) {
    case 0: {
        struct obj *mtmp_helmet = which_armor(mtmp, W_ARMH);

        if (hard_helmet(mtmp_helmet) || amorphous(pm)
            || passes_walls(pm) || noncorporeal(pm) || unsolid(pm))
            return SCR_EARTH;
    }
        FALLTHROUGH;
        /*FALLTHRU*/
    case 1:
        return WAN_STRIKING;
    case 2:
        return POT_ACID;
    case 3:
        return POT_CONFUSION;
    case 4:
        return POT_BLINDNESS;
    case 5:
        return rn2(3) ? POT_SLEEPING : POT_HALLUCINATION;
    case 6:
        return POT_PARALYSIS;
    case 7:
    case 8:
        return WAN_MAGIC_MISSILE;
    case 9:
        return WAN_SLEEP;
    case 10:
        return rn2(3) ? WAN_FIRE : POT_OIL;
    case 11:
        return WAN_COLD;
    case 12:
        return rn2(5) ? WAN_LIGHTNING : WAN_CORROSION;
    case 13:
        return SCR_STINKING_CLOUD;
    case 14:
        return rn2(3) ? WAN_CANCELLATION : WAN_DRAINING;
    case 15:
        return WAN_SLOW_MONSTER;
    }
    /*NOTREACHED*/
    return 0;
}

boolean
find_misc(struct monst *mtmp)
{
    struct obj *obj;
    struct permonst *mdat = mtmp->data;
    coordxy x = mtmp->mx, y = mtmp->my;
    struct trap *t;
    coordxy xx, yy;
    int pmidx = NON_PM;
    boolean immobile = (mdat->mmove == 0);
    boolean stuck = (mtmp == u.ustuck);

    gm.m.misc = (struct obj *) 0;
    gm.m.has_misc = 0;
    if (is_animal(mdat) || mindless(mdat))
        return 0;
    if (u.uswallow && stuck)
        return FALSE;

    /* We arbitrarily limit to times when a player is nearby for the
     * same reason as Junior Pac-Man doesn't have energizers eaten until
     * you can see them...
     */
    if (dist2(x, y, mtmp->mux, mtmp->muy) > 36)
        return FALSE;

    if (!stuck && !immobile && !mtmp->mtrapped && (mtmp->cham == NON_PM)
        && mons[(pmidx = monsndx(mdat))].difficulty < 6) {
        boolean ignore_boulders = (verysmall(mdat) || throws_rocks(mdat)
                                   || passes_walls(mdat)),
            diag_ok = !NODIAG(pmidx);

        for (xx = x - 1; xx <= x + 1; xx++)
            for (yy = y - 1; yy <= y + 1; yy++)
                if (isok(xx, yy) && !u_at(xx, yy)
                    && (diag_ok || xx == x || yy == y)
                    && ((xx == x && yy == y) || !svl.level.monsters[xx][yy]))
                    if ((t = t_at(xx, yy)) != 0
                        && (ignore_boulders || !sobj_at(BOULDER, xx, yy))
                        && !onscary(xx, yy, mtmp)) {
                        /* use trap if it's the correct type */
                        if (t->ttyp == POLY_TRAP) {
                            gt.trapx = xx;
                            gt.trapy = yy;
                            gm.m.has_misc = MUSE_POLY_TRAP;
                            return TRUE;
                        }
                    }
    }
    if (nohands(mdat))
        return 0;

    /* normally we would want to bracket a macro expansion containing
       'if' without matching 'else' with 'do { ... } while (0)' but we
       can't do that here because it would intercept 'continue' */
#define nomore(x)       if (gm.m.has_misc == (x)) continue
    /*
     * [bug?]  Choice of item is not prioritized; the last viable one
     * in the monster's inventory will be chosen.
     * 'nomore()' is nearly worthless because it only screens checking
     * of duplicates when there is no alternate type in between them.
     *
     * MUSE_BAG issues:
     * should allow looting floor container instead of needing the
     * monster to have picked it up and now be carrying it which takes
     * extra time and renders heavily filled containers immune;
     * hero should have a chance to see the monster fail to open a
     * locked container instead of monster always knowing lock state
     * (may not be feasible to implement--requires too much per-object
     * info for each monster);
     * monster with key should be able to unlock a locked floor
     * container and not know whether it is trapped.
     */
    for (obj = mtmp->minvent; obj; obj = obj->nobj) {
        /* Monsters shouldn't recognize cursed items; this kludge is
           necessary to prevent serious problems though... */
        if (obj->otyp == POT_GAIN_LEVEL
            && (!obj->cursed
                || (!mtmp->isgd && !mtmp->isshk && !mtmp->ispriest))) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_GAIN_LEVEL;
        }
        nomore(MUSE_FIGURINE);
        if (obj->otyp == FIGURINE && !mtmp->mpeaceful) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_FIGURINE;
        }
        nomore(MUSE_POT_SICKNESS);
        if (obj->otyp == POT_SICKNESS && !mtmp->mpeaceful
            && (MON_WEP(mtmp) && !MON_WEP(mtmp)->opoisoned)
            && mtmp->data->maligntyp != AM_LAWFUL
            && !m_seenres(mtmp, M_SEEN_POISON)) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_SICKNESS;
        }
        nomore(MUSE_BULLWHIP);
        if (obj->otyp == BULLWHIP && !mtmp->mpeaceful
            /* the random test prevents whip-wielding
               monster from attempting disarm every turn */
            && uwep && !rn2(5) && obj == MON_WEP(mtmp)
            /* hero's location must be known and adjacent */
            && u_at(mtmp->mux, mtmp->muy)
            && m_next2u(mtmp)
            /* don't bother if it can't work (this doesn't
               prevent cursed weapons from being targeted) */
            && !u.uswallow
            && (canletgo(uwep, "")
                || (u.twoweap && canletgo(uswapwep, "")))) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_BULLWHIP;
        }
        /* Note: peaceful/tame monsters won't make themselves
         * invisible unless you can see them.  Not really right, but...
         */
        nomore(MUSE_WAN_MAKE_INVISIBLE);
        if (obj->otyp == WAN_MAKE_INVISIBLE && !mtmp->minvis
            && !mtmp->invis_blkd && !mtmp->mpeaceful
            && (!attacktype(mtmp->data, AT_GAZE) || mtmp->mcan)) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_WAN_MAKE_INVISIBLE;
        }
        nomore(MUSE_POT_INVISIBILITY);
        if (obj->otyp == POT_INVISIBILITY && !mtmp->minvis
            && !mtmp->invis_blkd && !mtmp->mpeaceful
            && (!attacktype(mtmp->data, AT_GAZE) || mtmp->mcan)) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_INVISIBILITY;
        }
        nomore(MUSE_WAN_SPEED_MONSTER);
        if (obj->otyp == WAN_SPEED_MONSTER
            && mtmp->mspeed != MFAST && !mtmp->isgd) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_WAN_SPEED_MONSTER;
        }
        nomore(MUSE_POT_SPEED);
        if (obj->otyp == POT_SPEED && mtmp->mspeed != MFAST && !mtmp->isgd) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_SPEED;
        }
        nomore(MUSE_WAN_POLYMORPH);
        if (obj->otyp == WAN_POLYMORPH
            && (mtmp->cham == NON_PM) && mons[monsndx(mdat)].difficulty < 6) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_WAN_POLYMORPH;
        }
        nomore(MUSE_POT_POLYMORPH);
        if (obj->otyp == POT_POLYMORPH && (mtmp->cham == NON_PM)
            && mons[monsndx(mdat)].difficulty < 6) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_POLYMORPH;
        }
        nomore(MUSE_POT_REFLECT);
        if (obj->otyp == POT_REFLECTION && !mtmp->mpeaceful
            && m_canseeu(mtmp) && reflectable_offense() &&
            !mon_prop(mtmp, REFLECTING)) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_POT_REFLECT;
        }
        nomore(MUSE_SCR_REMOVE_CURSE);
        if (obj->otyp == SCR_REMOVE_CURSE) {
            struct obj *otmp;
            for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
                if (otmp->cursed &&
                    ((otmp->otyp == LOADSTONE || otmp->otyp == FOULSTONE)
                        || otmp->owornmask)) {
                    gm.m.misc = obj;
                    gm.m.has_misc = MUSE_SCR_REMOVE_CURSE;
                }
            }
        }
        nomore(MUSE_BAG);
        if (Is_container(obj) && obj->otyp != BAG_OF_TRICKS && !rn2(5)
            && !SchroedingersBox(obj)
            && !gm.m.has_misc && Has_contents(obj)
            && !obj->olocked && !obj->otrapped) {
            gm.m.misc = obj;
            gm.m.has_misc = MUSE_BAG;
        }
    }
    return (boolean) !!gm.m.has_misc;
#undef nomore
}

/* type of monster to polymorph into; defaults to one suitable for the
   current level rather than the totally arbitrary choice of newcham() */
staticfn struct permonst *
muse_newcham_mon(struct monst *mon)
{
    int pm = armor_to_dragon(mon);
    if (pm != NON_PM) {
        return &mons[pm];
    }
    /* not wearing anything that would turn it into a dragon */
    return rndmonst();
}

staticfn int
mloot_container(
    struct monst *mon,
    struct obj *container,
    boolean vismon)
{
    char contnr_nam[BUFSZ], mpronounbuf[20];
    boolean nearby;
    int takeout_indx, takeout_count, howfar, res = 0;

    if (!container || !Has_contents(container) || container->olocked)
        return res; /* 0 */
    /* FIXME: handle cursed bag of holding */
    if (Is_mbag(container) && container->cursed)
        return res; /* 0 */
    if (SchroedingersBox(container))
        return res;

    switch (rn2(10)) {
    default: /* case 0, 1, 2, 3: */
        takeout_count = 1;
        break;
    case 4: case 5: case 6:
        takeout_count = 2;
        break;
    case 7: case 8:
        takeout_count = 3;
        break;
    case 9:
        takeout_count = 4;
        break;
    }
    howfar = mdistu(mon);
    nearby = (howfar <= 7 * 7);
    contnr_nam[0] = mpronounbuf[0] = '\0';
    if (vismon) {
        /* do this once so that when hallucinating it won't change
           from one item to the next */
        Strcpy(mpronounbuf, mhe(mon));
    }

    for (takeout_indx = 0; takeout_indx < takeout_count; ++takeout_indx) {
        struct obj *xobj;
        int nitems;

        if (!Has_contents(container)) /* might have removed all items */
            break;
        /* TODO?
         *  Monster ought to prioritize on something it wants to use.
         */
        nitems = 0;
        for (xobj = container->cobj; xobj != 0; xobj = xobj->nobj)
            ++nitems;
        /* nitems is always greater than 0 due to Has_contents() check;
           throttle item removal as the container becomes less filled */
        if (!rn2(nitems + 1))
            break;
        nitems = rn2(nitems);
        for (xobj = container->cobj; xobj != 0; xobj = xobj->nobj)
            if (--nitems < 0)
                break;
        assert(xobj != NULL);

        container->cknown = 0; /* hero no longer knows container's contents
                                * even if [attempted] removal is observed */
        if (!*contnr_nam) {
            /* xname sets dknown, distant_name might depending on its own
               idea about nearness */
            Strcpy(contnr_nam, an(nearby ? xname(container)
                                         : distant_name(container, xname)));
        }
        /* this was originally just 'can_carry(mon, xobj)' which
           covers objects a monster shouldn't pick up but also
           checks carrying capacity; for that, it ended up counting
           xobj's weight twice when container is carried; so take
           xobj out, check whether it can be carried, and then put
           it back (below) if it can't be */
        obj_extract_self(xobj); /* this reduces container's weight */
        /* check whether mon can handle xobj and whether weight of xobj plus
           minvent (including container, now without xobj) can be carried */
        if (can_carry(mon, xobj)) {
            if (vismon) {
                if (howfar > 2) /* not adjacent */
                    Norep("%s rummages through %s.", Monnam(mon), contnr_nam);
                else if (takeout_indx == 0) /* adjacent, first item */
                    pline_mon(mon, "%s removes %s from %s.", Monnam(mon),
                          doname(xobj), contnr_nam);
                else /* adjacent, additional items */
                    pline("%s removes %s.", upstart(mpronounbuf),
                          doname(xobj));
            }
            if (container->otyp == ICE_BOX)
                removed_from_icebox(xobj); /* resume rotting for corpse */
            /* obj_extract_self(xobj); -- already done above */
            (void) mpickobj(mon, xobj);
            res = 2;
        } else { /* couldn't carry xobj separately so put back inside */
            /* an achievement prize (castle's wand?) might already be
               marked nomerge (when it hasn't been in invent yet) */
            boolean already_nomerge = xobj->nomerge != 0,
                    just_xobj = !Has_contents(container);

            /* this doesn't restore the original contents ordering
               [shouldn't be a problem; even though this item didn't
               give the rummage message, that's what mon was doing] */
            xobj->nomerge = 1;
            xobj = add_to_container(container, xobj);
            if (!already_nomerge)
                xobj->nomerge = 0;
            container->owt = weight(container);
            if (just_xobj)
                break; /* out of takeout_count loop */
        } /* can_carry */
    } /* takeout_count */
    return res;
}

DISABLE_WARNING_UNREACHABLE_CODE

int
use_misc(struct monst *mtmp)
{
    static const char MissingMiscellaneousItem[] = "use_misc: no %s";
    char nambuf[BUFSZ];
    boolean vis, vismon, vistrapspot, oseen;
    int i;
    struct trap *t;
    struct obj *otmp = gm.m.misc;
    struct obj *otmp2;

    if ((i = precheck(mtmp, otmp)) != 0)
        return i;
    vis = cansee(mtmp->mx, mtmp->my);
    vismon = canseemon(mtmp);
    oseen = otmp && vismon;

    switch (gm.m.has_misc) {
    case MUSE_FIGURINE: {
        coord cc;
        struct permonst *pm;
        struct monst *mon;
        int mndx = otmp->corpsenm;

        pm = &mons[mndx];
        if (vismon)
            pline("%s activates a figurine!", Monnam(mtmp));
        else if (!Deaf)
            You("hear a figurine being activated.");
        /* activating a figurine provides one way to exceed the
           maximum number of the target critter created--unless
           it has a special limit (erinys, Nazgul) */
        if ((svm.mvitals[mndx].mvflags & G_EXTINCT)
            && mbirth_limit(mndx) != MAXMONNO) {
            pline("... but the figurine refused.");
            break; /* mtmp is null */
        }
        if (!enexto(&cc, mtmp->mx, mtmp->my, pm))
            return 0;
        m_useup(mtmp, otmp);
        mon = makemon(pm, cc.x, cc.y, NO_MM_FLAGS);
        mon->mpeaceful = 0;
        set_malign(mon);
        return 2;
    }
    case MUSE_POT_GAIN_LEVEL:
        if (!otmp)
            panic(MissingMiscellaneousItem, "potion of gain level");
        mquaffmsg(mtmp, otmp);
        if (otmp->cursed) {
            if (!otmp->odiluted && Can_rise_up(mtmp->mx, mtmp->my, &u.uz)) {
                int tolev = depth(&u.uz) - 1;
                d_level tolevel;

                get_level(&tolevel, tolev);
                /* insurance against future changes... */
                if (on_level(&tolevel, &u.uz))
                    goto skipmsg;
                if (vismon) {
                    pline_mon(mtmp, "%s rises up, through the %s!", Monnam(mtmp),
                            ceiling(mtmp->mx, mtmp->my));
                    makeknown(POT_GAIN_LEVEL);
                }
                m_useup(mtmp, otmp);
                migrate_to_level(mtmp, ledger_no(&tolevel), MIGR_RANDOM,
                                    (coord *) 0);
                return 2;
            } else {
                skipmsg:
                if (vismon) {
                    pline_mon(mtmp, "%s looks uneasy.", Monnam(mtmp));
                    makeknown(POT_GAIN_LEVEL);
                }
                m_useup(mtmp, otmp);
                return 2;
            }
        }
        if (vismon)
            pline_mon(mtmp, "%s seems more experienced.", Monnam(mtmp));
        if (oseen)
            makeknown(POT_GAIN_LEVEL);
        m_useup(mtmp, otmp);
        if (!grow_up(mtmp, (struct monst *) 0))
            return 1;
        /* grew into exiled monster */
        return 2;
    case MUSE_WAN_MAKE_INVISIBLE:
    case MUSE_POT_INVISIBILITY:
        if (!otmp)
            panic(MissingMiscellaneousItem, "potion of invisibility");
        if (otmp->otyp == WAN_MAKE_INVISIBLE) {
            if (!mzapwand(mtmp, otmp, TRUE))
                return 2;
        } else
            mquaffmsg(mtmp, otmp);
        /* format monster's name before altering its visibility */
        Strcpy(nambuf, mon_nam(mtmp));
        mon_set_minvis(mtmp);
        if (vismon && mtmp->minvis) { /* was seen, now invisible */
            if (canspotmon(mtmp)) {
                pline("%s body takes on a %s transparency.",
                        upstart(s_suffix(nambuf)),
                        Hallucination ? "normal" : "strange");
            } else {
                pline("Suddenly you cannot see %s.", nambuf);
                if (vis)
                    map_invisible(mtmp->mx, mtmp->my);
            }
            if (oseen)
                makeknown(otmp->otyp);
        }
        if (otmp->otyp == POT_INVISIBILITY) {
            if (otmp->cursed)
                you_aggravate(mtmp);
            m_useup(mtmp, otmp);
        }
        return 2;
    case MUSE_WAN_SPEED_MONSTER:
        if (!otmp)
            panic(MissingMiscellaneousItem, "wand of speed monster");
        if (mzapwand(mtmp, otmp, TRUE))
            mon_adjust_speed(mtmp, 1, otmp);
        return 2;
    case MUSE_POT_SPEED:
        if (!otmp)
            panic(MissingMiscellaneousItem, "potion of speed");
        mquaffmsg(mtmp, otmp);
        /* note difference in potion effect due to substantially
            different methods of maintaining speed ratings:
            player's character becomes "very fast" temporarily;
            monster becomes "one stage faster" permanently */
        mon_adjust_speed(mtmp, 1, otmp);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_POT_REFLECT:
        mquaffmsg(mtmp, otmp);
        mtmp->mextrinsics |= MR2_REFLECTION;
        mtmp->mreflecttime = otmp->blessed
            ? rn1(50, otmp->odiluted ? 100 : 250)
            : rn1(10, otmp->odiluted ? 50 : 100);
        if (canseemon(mtmp) && !Blind)
            pline("%s is covered in a silvery sheen!", Monnam(mtmp));
        if (oseen)
            makeknown(otmp->otyp);
        m_useup(mtmp, otmp);
        return 2;
    case MUSE_WAN_POLYMORPH:
        if (!otmp)
            panic(MissingMiscellaneousItem, "wand of polymorph");
        if (mzapwand(mtmp, otmp, TRUE)) {
            (void) newcham(mtmp, muse_newcham_mon(mtmp),
                            NC_VIA_WAND_OR_SPELL | NC_SHOW_MSG);
            if (oseen)
                makeknown(WAN_POLYMORPH);
        }
        return 2;
    case MUSE_POT_POLYMORPH:
        if (!otmp)
            panic(MissingMiscellaneousItem, "potion of polymorph");
        mquaffmsg(mtmp, otmp);
        m_useup(mtmp, otmp);
        if (vismon)
            pline_mon(mtmp, "%s suddenly mutates!", Monnam(mtmp));
        (void) newcham(mtmp, muse_newcham_mon(mtmp), NC_SHOW_MSG);
        if (oseen)
            makeknown(POT_POLYMORPH);
        return 2;
    case MUSE_POT_SICKNESS:
        /* Does mon have a weapon wielded they can poison? */
        otmp2 = MON_WEP(mtmp);
        if (!otmp2)
            return 0;

        if (is_poisonable(otmp2) && !otmp2->opoisoned) {
            if (vismon)
                pline("%s dips %s %s into a %s!", Monnam(mtmp), mhis(mtmp),
                xname(otmp2), xname(otmp));
            otmp2->opoisoned = TRUE;
            m_useup(mtmp, otmp);
        } else
            return 0;
        /* TODO: Does mon have projectiles they can poison? */
        return 2;
    case MUSE_SCR_REMOVE_CURSE: {
        struct obj *obj;
        mreadmsg(mtmp, otmp);
        if (canseemon(mtmp)) {
            if (mtmp->mconf)
                You("feel as though %s needs some help.", mon_nam(mtmp));
            else
                You("feel like someone is helping %s.", mon_nam(mtmp));
            makeknown(SCR_REMOVE_CURSE);
        }
        for (obj = mtmp->minvent; obj; obj = obj->nobj) {
            /* gold isn't subject to cursing and blessing */
            if (obj->oclass == COIN_CLASS)
                continue;
            if (otmp->blessed || otmp->owornmask
                || obj->otyp == LOADSTONE
                || obj->otyp == FOULSTONE) {
                if (mtmp->mconf)
                    blessorcurse(obj, 2);
                else
                    uncurse(obj);
            }
        }
        m_useup(mtmp, otmp);
        return 0;
    }
    case MUSE_POLY_TRAP:
        t = t_at(gt.trapx, gt.trapy);
        vistrapspot = cansee(t->tx, t->ty);
        if (vis || vistrapspot)
            seetrap(t);
        if (vismon || vistrapspot) {
            pline_mon(mtmp, "%s deliberately %s onto a %s!", Some_Monnam(mtmp),
                  vtense(fakename[0], locomotion(mtmp->data, "jump")),
                  t->tseen ? trapname(t->ttyp, FALSE) : "hidden trap");
            /* note: if mtmp is unseen because it is invisible, its new
               shape will also be invisible and could produce "Its armor
               falls off" messages during the transformation; those make
               more sense after we've given "Someone jumps onto a trap." */
        }

        /*  don't use rloc() due to worms */
        remove_monster(mtmp->mx, mtmp->my);
        newsym(mtmp->mx, mtmp->my);
        place_monster(mtmp, gt.trapx, gt.trapy);
        maybe_unhide_at(gt.trapx, gt.trapy);
        if (mtmp->wormno)
            worm_move(mtmp);
        newsym(gt.trapx, gt.trapy);

        (void) newcham(mtmp, (struct permonst *) 0, NC_SHOW_MSG);
        return 2;
    case MUSE_BAG:
        if (!otmp)
            panic(MissingMiscellaneousItem, "container");
        return mloot_container(mtmp, otmp, vismon);
    case MUSE_BULLWHIP:
        /* attempt to disarm hero */
        {
            const char *The_whip = vismon ? "The bullwhip" : "A whip";
            int where_to = rn2(4);
            struct obj *obj = uwep;
            const char *hand;
            char the_weapon[BUFSZ];
            boolean freegrease = FALSE;

            if (!obj || !canletgo(obj, "")
                || (u.twoweap && canletgo(uswapwep, "") && rn2(2)))
                obj = uswapwep;
            if (!obj)
                break; /* shouldn't happen after find_misc() */

            Strcpy(the_weapon, the(xname(obj)));
            hand = body_part(HAND);
            if (bimanual(obj))
                hand = makeplural(hand);

            if (vismon)
                pline_mon(mtmp, "%s flicks a bullwhip towards your %s!",
                          Monnam(mtmp), hand);
            if (obj->otyp == HEAVY_IRON_BALL) {
                pline("%s fails to wrap around %s.", The_whip, the_weapon);
                return 1;
            }
            urgent_pline("%s wraps around %s you're wielding!", The_whip,
                         the_weapon);
            if (welded(obj)) {
                pline("%s welded to your %s%c",
                      !is_plural(obj) ? "It is" : "They are", hand,
                      !obj->bknown ? '!' : '.');
                /* obj->bknown = 1; */ /* welded() takes care of this */
                where_to = 0;
            }
            if (obj->greased) {
                pline("%s slippery from being greased.",
                      !is_plural(obj) ? "It is" : "They are");
                where_to = 0;
                freegrease = TRUE;
            }
            if (!where_to) {
                pline_The("whip slips free."); /* not `The_whip' */
                if (freegrease)
                    maybe_grease_off(obj);
                return 1;
            } else if (where_to == 3 && mon_hates_silver(mtmp)
                       && is_silver(obj)) {
                /* this monster won't want to catch a silver
                   weapon; drop it at hero's feet instead */
                where_to = 2;
            }
            remove_worn_item(obj, FALSE);
            freeinv(obj);
            switch (where_to) {
            case 1: /* onto floor beneath mon */
                pline_mon(mtmp, "%s yanks %s from your %s!", Monnam(mtmp),
                          the_weapon, hand);
                place_object(obj, mtmp->mx, mtmp->my);
                break;
            case 2: /* onto floor beneath you */
                pline_mon(mtmp, "%s yanks %s to the %s!", Monnam(mtmp),
                          the_weapon, surface(u.ux, u.uy));
                dropy(obj);
                break;
            case 3: /* into mon's inventory */
                pline_mon(mtmp, "%s snatches %s!", Monnam(mtmp), the_weapon);
                (void) mpickobj(mtmp, obj);
                break;
            }
            return 1;
        }
        /*NOTREACHED*/
        return 0;
    case 0:
        return 0; /* i.e. an exploded wand */
    default:
        impossible("%s wanted to perform action %d?", Monnam(mtmp),
                   gm.m.has_misc);
        break;
    }
    return 0;
}

RESTORE_WARNINGS

staticfn void
you_aggravate(struct monst *mtmp)
{
    pline("For some reason, %s presence is known to you.",
          s_suffix(noit_mon_nam(mtmp)));
    cls();
#ifdef CLIPPING
    cliparound(mtmp->mx, mtmp->my);
#endif
    show_glyph(mtmp->mx, mtmp->my, mon_to_glyph(mtmp, rn2_on_display_rng));
    display_self();
    You_feel("aggravated at %s.", noit_mon_nam(mtmp));
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    if (unconscious()) {
        gm.multi = -1;
        gn.nomovemsg = "Aggravated, you are jolted into full consciousness.";
    }
    newsym(mtmp->mx, mtmp->my);
    if (!canspotmon(mtmp))
        map_invisible(mtmp->mx, mtmp->my);
}

int
rnd_misc_item(struct monst *mtmp)
{
    struct permonst *pm = mtmp->data;
    int difficulty = mons[(monsndx(pm))].difficulty;

    if (is_animal(pm) || attacktype(pm, AT_EXPL) || mindless(mtmp->data)
        || pm->mlet == S_GHOST || pm->mlet == S_KOP)
        return 0;
    /* Unlike other rnd_item functions, we only allow _weak_ monsters
     * to have this item; after all, the item will be used to strengthen
     * the monster and strong monsters won't use it at all...
     */
    if (difficulty < 6 && !rn2(30))
        return POT_POLYMORPH;

    if (!rn2(40) && !nonliving(pm) && !is_vampshifter(mtmp))
        return AMULET_OF_LIFE_SAVING;

    switch (rn2(4)) {
    case 0:
        if (mtmp->isgd)
            return 0;
        return rn2(6) ? POT_SPEED : WAN_SPEED_MONSTER;
    case 1:
        if (mtmp->mpeaceful && !See_invisible)
            return 0;
        return rn2(6) ? POT_INVISIBILITY : WAN_MAKE_INVISIBLE;
    case 2:
        return POT_GAIN_LEVEL;
    case 3:
        return POT_REFLECTION;
    }
    /*NOTREACHED*/
    return 0;
}

#if 0
/* check whether hero is carrying a corpse or contained petrifier corpse */
staticfn boolean
necrophiliac(struct obj *objlist, boolean any_corpse)
{
    while (objlist) {
        if (objlist->otyp == CORPSE
            && (any_corpse || touch_petrifies(&mons[objlist->corpsenm])))
            return TRUE;
        if (Has_contents(objlist) && necrophiliac(objlist->cobj, FALSE))
            return TRUE;
        objlist = objlist->nobj;
    }
    return FALSE;
}
#endif

boolean
searches_for_item(struct monst *mon, struct obj *obj)
{
    int typ = obj->otyp;

    /* don't let monsters interact with protected items on the floor */
    if (obj->where == OBJ_FLOOR
        && (obj->ox == mon->mx && obj->oy == mon->my)
        && onscary(obj->ox, obj->oy, mon)) {
        return FALSE;
    }

    if (is_animal(mon->data) || mindless(mon->data)
        || mon->data == &mons[PM_GHOST]) /* don't loot bones piles */
        return FALSE;

    if (typ == WAN_MAKE_INVISIBLE
        || typ == POT_INVISIBILITY
        || typ == RIN_INVISIBILITY)
        return (boolean) (!mon->minvis && !mon->invis_blkd
                          && !attacktype(mon->data, AT_GAZE));
    if (typ == WAN_SPEED_MONSTER || typ == POT_SPEED)
        return (boolean) (mon->mspeed != MFAST);
     if (typ == POT_REFLECTION)
        return !mon_prop(mon, REFLECTING);

    switch (obj->oclass) {
    case WAND_CLASS:
        if (obj->spe < 0)
            return FALSE;
        if (typ == WAN_DIGGING)
            return (boolean) !mon_prop(mon, LEVITATION);
        if (typ == WAN_POLYMORPH)
            return (boolean) (mons[monsndx(mon->data)].difficulty < 6);
        if (objects[typ].oc_dir == RAY
            || typ == WAN_STRIKING
            || typ == WAN_UNDEAD_TURNING
            || typ == WAN_CANCELLATION
            || typ == WAN_TELEPORTATION
            || typ == WAN_CREATE_MONSTER
            || typ == WAN_SLOW_MONSTER)
            return TRUE;
        break;
    case POTION_CLASS:
        if (typ == POT_HEALING
            || typ == POT_EXTRA_HEALING
            || typ == POT_FULL_HEALING
            || typ == POT_POLYMORPH
            || typ == POT_GAIN_LEVEL
            || typ == POT_PARALYSIS
            || typ == POT_SLEEPING
            || typ == POT_ACID
            || typ == POT_CONFUSION
            || typ == POT_HALLUCINATION
            || typ == POT_OIL
            || typ == POT_REFLECTION
            || typ == POT_PHASING
            || (typ == POT_RESTORE_ABILITY && mon->mcan)
            || typ == POT_SICKNESS
            || (typ == POT_VAMPIRE_BLOOD && is_vampire(mon->data)))
            return TRUE;
        if (typ == POT_BLINDNESS && !attacktype(mon->data, AT_GAZE))
            return TRUE;
        break;
    case SCROLL_CLASS:
        if (typ == SCR_TELEPORTATION
            || typ == SCR_CREATE_MONSTER
            || typ == SCR_EARTH
            || typ == SCR_FIRE
            || typ == SCR_FLOOD
            || typ == SCR_REMOVE_CURSE
            || typ == SCR_CLONING
            || typ == SCR_STINKING_CLOUD)
            return TRUE;
        break;
    case AMULET_CLASS:
        if (typ == AMULET_OF_LIFE_SAVING)
            return (boolean) !(nonliving(mon->data) || is_vampshifter(mon));
        if (can_muse_amulet(typ))
            return TRUE;
        break;
    case TOOL_CLASS:
        if (typ == PICK_AXE)
            return (boolean) needspick(mon->data);
        if (typ == UNICORN_HORN)
            return (boolean) (!obj->cursed && !is_unicorn(mon->data)
                              && mon->data != &mons[PM_KI_RIN]);
        if (typ == FROST_HORN || typ == FIRE_HORN || typ == MAGIC_FLUTE)
            return (obj->spe > 0 && can_blow(mon));
        if ((typ == SKELETON_KEY || typ == LOCK_PICK)
            && !nohands(mon->data) && !verysmall(mon->data)
            && !mindless(mon->data)
            && !m_carrying(mon, SKELETON_KEY) && !m_carrying(mon, LOCK_PICK))
            return TRUE;
        if (Is_container(obj) && !(Is_mbag(obj) && obj->cursed)
            && !obj->olocked)
            return TRUE;
        if (typ == EXPENSIVE_CAMERA)
            return (obj->spe > 0);
        if (typ == FIGURINE)
            return TRUE;
        break;
    case FOOD_CLASS:
        if (typ == CORPSE)
            return (boolean) ((safegloves(which_armor(mon, W_ARMG))
                               && touch_petrifies(&mons[obj->corpsenm]))
                              || (!(resists_ston(mon) || defended(mon, AD_STON))
                                  && cures_stoning(mon, obj, FALSE)));
        if (typ == TIN)
            return (boolean) (mcould_eat_tin(mon)
                              && (!(resists_ston(mon) || defended(mon, AD_STON))
                                  && cures_stoning(mon, obj, TRUE)));
        if (typ == EGG && ismnum(obj->corpsenm))
            return (boolean) touch_petrifies(&mons[obj->corpsenm]);
        break;
     case RING_CLASS:
        /* Should match the list in m_dowear_type.
         * Uniques don't go for invisibility or teleportation;
         * it would probably be a waste of time. */
        if (typ == RIN_PROTECTION
            || typ == RIN_INCREASE_DAMAGE || typ == RIN_INCREASE_ACCURACY)
            return (obj->spe > 0);
        if (typ == RIN_SEE_INVISIBLE)
            return (!mon_prop(mon, SEE_INVIS));
        if (typ == RIN_FIRE_RESISTANCE)
            return (!(resists_fire(mon) || defended(mon, AD_FIRE)));
        if (typ == RIN_COLD_RESISTANCE)
            return (!(resists_cold(mon) || defended(mon, AD_COLD)));
        if (typ == RIN_SHOCK_RESISTANCE)
            return (!(resists_elec(mon) || defended(mon, AD_ELEC)));
        if (typ == RIN_POISON_RESISTANCE)
            return (!(resists_poison(mon) || defended(mon, AD_DRST)));
        if (typ == RIN_SLOW_DIGESTION)
            return (!mon_prop(mon, SLOW_DIGESTION));
        if (typ == RIN_REGENERATION)
            return (!mon_prop(mon, REGENERATION));
        if (typ == RIN_LEVITATION)
            return (grounded(mon->data));
        if (typ == RIN_FREE_ACTION)
            return TRUE;
        /* Below this line are off-limits to uniques */
        if (mon->data->geno & G_UNIQ)
            return (FALSE);
        if (typ == RIN_INVISIBILITY)
            return !(mon->minvis);
        if (typ == RIN_TELEPORTATION)
            return (!mon_prop(mon, TELEPORT));
        if (typ == RIN_TELEPORT_CONTROL)
            return (!mon_prop(mon, TELEPORT_CONTROL));
        break;
    default:
        break;
    }

    return FALSE;
}

DISABLE_WARNING_FORMAT_NONLITERAL

const char *
mon_reflectsrc(struct monst *mon)
{
    struct obj *orefl = which_armor(mon, W_ARMS);

    /* TODO: Make sure the reflecting item is seen before identifying */

    if (orefl && orefl->otyp == SHIELD_OF_REFLECTION) {
        makeknown(SHIELD_OF_REFLECTION);
        return "shield";
    } else if (arti_reflects(MON_WEP(mon))) {
        /* due to wielded artifact weapon */
        return "weapon";
    } else if ((orefl = which_armor(mon, W_AMUL))
               && orefl->otyp == AMULET_OF_REFLECTION) {
        makeknown(AMULET_OF_REFLECTION);
        return "amulet";
    } else if ((orefl = which_armor(mon, W_ARM))
               && Is_dragon_scaled_armor(orefl)
               && Dragon_armor_to_scales(orefl) == SILVER_DRAGON_SCALES) {
        return "armor";
    } else if ((orefl = which_armor(mon, W_ARMC))
              && orefl->otyp == SILVER_DRAGON_SCALES) {
        return "set of scales";
    } else if (innate_reflector(mon->data)) {
        /* Silver dragons only reflect when mature; babies do not */
        return "luster";
    } else if (has_reflection(mon)) {
        /* specifically for the monster spell MGC_REFLECTION */
        return "shimmering globe";
    } else if (m_carrying(mon, MIRROR)) {
        /* Also applies to the Magic Mirror of Merlin - put this before the
         * W_ART check */
        return Role_if(PM_CARTOMANCER) ? "shiny card" : "mirror";
    } else if (orefl && orefl->oartifact == ART_HOLOGRAPHIC_VOID_LILY) {
        /* Due to any carried artifact which grants reflection, which shows as W_ART */
        return "card";
    }
    return (const char*) NULL;
}

/* Check whether the player is reflecting right now.
 * Return a string which is the source of the player's reflection,
 * or 0 if the player does not reflect the thing. */
const char *
ureflectsrc(void)
{
    /* Check from outermost to innermost objects */
    if (EReflecting & W_ARMS) {
        /* assumes shield of reflection */
        makeknown(SHIELD_OF_REFLECTION);
        return "shield";
    } else if (HReflecting) {
        /* Potion of reflection */
        return "shimmering globe";
    } else if (EReflecting & (W_WEP | W_SWAPWEP)) {
        /* Due to wielded artifact weapon */
        return "weapon";
    } else if (EReflecting & W_ARMC) {
        if (uarmc->otyp == SILVER_DRAGON_SCALES) {
            return "set of scales";
        } else {
            /* no other cloaks give this */
            impossible("reflecting cloak?");
            return "cloak";
        }
    } else if (EReflecting & W_AMUL) {
        makeknown(AMULET_OF_REFLECTION);
        return "amulet";
    } else if (EReflecting & W_ARM) {
        if (uskin) {
            return "luster";
        } else {
            return "armor";
        }
    } else if (gy.youmonst.data == &mons[PM_SILVER_DRAGON]) {
        return "scales";
    } else if (carrying(MIRROR)) {
        /* carried mirror offers reflection but easily breaks */
        /* Also applies to the Magic Mirror of Merlin - put this before the
         * W_ART check */
        makeknown(MIRROR);
        return Role_if(PM_CARTOMANCER) ? "shiny card" : "mirror";
    } else if (EReflecting & W_ART) {
        /* Only carried artifact which grants reflection is
         * the Holographic Void Lily, which shows as W_ART */
        return "card";
    }
    return (const char*) NULL;
}

RESTORE_WARNING_FORMAT_NONLITERAL

/* cure mon's blindness (use_defensive, dog_eat, meatobj) */
void
mcureblindness(struct monst *mon, boolean verbos)
{
    if (!mon->mcansee) {
        mon->mcansee = 1;
        mon->mblinded = 0;
        if (verbos && haseyes(mon->data))
            pline_mon(mon, "%s can see again.", Monnam(mon));
    }
}

/* TRUE if the monster ate something */
boolean
munstone(struct monst *mon, boolean by_you)
{
    struct obj *obj;
    boolean tinok;
    boolean spellcaster =
        attacktype_fordmg(mon->data, AT_MAGC, AD_SPEL)
        || attacktype_fordmg(mon->data, AT_MAGC, AD_CLRC);

    if ((resists_ston(mon) || defended(mon, AD_STON)))
        return FALSE;
    if (mon->meating || helpless(mon))
        return FALSE;
    mon->mstrategy &= ~STRAT_WAITFORU;

   if (spellcaster && !mon->mcan && !mon->mspec_used
        && !mon->mconf && mon->m_lev >= 5) {
        struct obj *otmp, *onext, *pseudo;

        pseudo = mksobj(SPE_STONE_TO_FLESH, FALSE, FALSE);
        pseudo->blessed = pseudo->cursed = 0;
        mon->mspec_used = mon->mspec_used + rn2(7);
        if (canspotmon(mon))
            pline("%s casts a spell!", canspotmon(mon)
                  ? Monnam(mon) : Something);
        if (canspotmon(mon)) {
            if (Hallucination)
                pline("Look!  The Pillsbury Doughboy!");
            else
                pline("%s seems limber!", Monnam(mon));
        }

        for (otmp = mon->minvent; otmp; otmp = onext) {
            onext = otmp->nobj;
            mon->misc_worn_check &= ~otmp->owornmask;
            update_mon_extrinsics(mon, otmp, FALSE, TRUE);
            otmp->owornmask = 0L; /* obfree() expects this */
            (void) bhito(otmp, pseudo);
        }
        obfree(pseudo, (struct obj *) 0);
        mon->mlstmv = svm.moves; /* it takes a turn */
        return TRUE;
    }


    tinok = mcould_eat_tin(mon);
    for (obj = mon->minvent; obj; obj = obj->nobj) {
        if (cures_stoning(mon, obj, tinok)) {
            mon_consume_unstone(mon, obj, by_you, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

staticfn void
mon_consume_unstone(
    struct monst *mon,
    struct obj *obj,
    boolean by_you,
    boolean stoning) /* T: stop petrification, F: cure stun && confusion */
{
    boolean vis = canseemon(mon), tinned = obj->otyp == TIN,
            food = obj->otyp == CORPSE || tinned,
            acid = obj->otyp == POT_ACID
                   || (food && acidic(&mons[obj->corpsenm])),
            lizard = food && obj->corpsenm == PM_LIZARD,
            leaf = obj->otyp == EUCALYPTUS_LEAF;
    int nutrit = food ? dog_nutrition(mon, obj) : 0; /* also sets meating */

    /* give a "<mon> is slowing down" message and also remove
       intrinsic speed (comparable to similar effect on the hero) */
    if (stoning)
        mon_adjust_speed(mon, -3, (struct obj *) 0);

    if (vis) {
        long save_quan = obj->quan;

        obj->quan = 1L;
        pline_mon(mon, "%s %s %s.", Monnam(mon),
              ((obj->oclass == POTION_CLASS) ? "quaffs"
               : (obj->otyp == TIN) ? "opens and eats the contents of"
                 : "eats"),
              distant_name(obj, doname));
        obj->quan = save_quan;
    } else if (!Deaf)
        You_hear("%s.",
                 (obj->oclass == POTION_CLASS) ? "drinking" : "chewing");

    m_useup(mon, obj);
    /* obj is now gone */

    if (acid && !tinned && !(resists_acid(mon) || defended(mon, AD_ACID))) {
        mon->mhp -= rnd(15);
        if (vis)
            pline_mon(mon, "%s has a very bad case of stomach acid.", Monnam(mon));
        if (DEADMONSTER(mon)) {
            pline_mon(mon, "%s dies!", Monnam(mon));
            if (by_you)
                /* hero gets credit (experience) and blame (possible loss
                   of alignment and/or luck and/or telepathy depending on
                   mon) for the kill but does not break pacifism conduct */
                xkilled(mon, XKILL_NOMSG | XKILL_NOCONDUCT);
            else
                mondead(mon);
            return;
        }
    }
    if (stoning) {
        mon->mstone = 0;
       if (!vis) {
           ; /* no feedback */
       } else if (Hallucination) {
            pline("What a pity - %s just ruined a future piece of art!",
                  mon_nam(mon));
        } else {
            pline_mon(mon, "%s seems limber!", Monnam(mon));
        }
    }
    if (lizard && (mon->mconf || mon->mstun)) {
        mon->mconf = 0;
        mon->mstun = 0;
        if (vis && !is_bat(mon->data) && mon->data != &mons[PM_STALKER])
            pline_mon(mon, "%s seems steadier now.", Monnam(mon));
    }
    if (leaf && (mon->mrabid || mon->mdiseased)) {
        mon->mrabid = 0;
        mon->mdiseased = 0;
        if (vis)
            pline("%s is no longer ill.", Monnam(mon));
    }
    if (mon->mtame && !mon->isminion && nutrit > 0) {
        struct edog *edog = EDOG(mon);

        if (edog->hungrytime < svm.moves)
            edog->hungrytime = svm.moves;
        edog->hungrytime += nutrit;
        mon->mconf = 0;
    }
    /* use up monster's next move */
    mon->movement -= NORMAL_SPEED;
    mon->mlstmv = svm.moves;
}

/* decide whether obj can cure petrification; also used when picking up */
boolean
cures_stoning(struct monst *mon, struct obj *obj, boolean tinok)
{
    if (obj->otyp == POT_ACID)
        return TRUE;
    if (obj->otyp == GLOB_OF_GREEN_SLIME)
        return (boolean) slimeproof(mon->data);
    if (obj->otyp != CORPSE && (obj->otyp != TIN || !tinok))
        return FALSE;
    /* corpse, or tin that mon can open */
    if (obj->corpsenm == NON_PM) /* empty/special tin */
        return FALSE;
    return (boolean) (obj->corpsenm == PM_LIZARD
                      || acidic(&mons[obj->corpsenm]));
}

staticfn boolean
mcould_eat_tin(struct monst *mon)
{
    struct obj *obj, *mwep;
    boolean welded_wep;

    /* monkeys who manage to steal tins can't open and eat them
       even if they happen to also have the appropriate tool */
    if (is_animal(mon->data))
        return FALSE;

    mwep = MON_WEP(mon);
    welded_wep = mwep && mwelded(mwep);
    /* this is different from the player; tin opener or dagger doesn't
       have to be wielded, and knife can be used instead of dagger */
    for (obj = mon->minvent; obj; obj = obj->nobj) {
        /* if stuck with a cursed weapon, don't check rest of inventory */
        if (welded_wep && obj != mwep)
            continue;

        if (obj->otyp == TIN_OPENER
            || (obj->oclass == WEAPON_CLASS
                && (objects[obj->otyp].oc_skill == P_DAGGER
                    || objects[obj->otyp].oc_skill == P_KNIFE)))
            return TRUE;
    }
    return FALSE;
}

/* TRUE if monster does something to avoid turning into green slime */
boolean
munslime(struct monst *mon, boolean by_you)
{
    struct obj *obj, odummy;
    struct permonst *mptr = mon->data;

    /*
     * muse_unslime() gives "mon starts turning green", "mon zaps
     * itself with a wand of fire", and "mon's slime burns away"
     * messages.  Monsters who don't get any chance at that just have
     * (via our caller) newcham()'s "mon turns into slime" feedback.
     */

    if (slimeproof(mptr))
        return FALSE;
    if (mon->meating || helpless(mon))
        return FALSE;
    mon->mstrategy &= ~STRAT_WAITFORU;

    /* if monster can breathe fire, do so upon self; a monster who deals
       fire damage by biting, clawing, gazing, and especially exploding
       isn't able to cure itself of green slime with its own attack
       [possible extension: monst capable of casting high level clerical
       spells could toss pillar of fire at self--probably too suicidal] */
    if (!mon->mcan && !mon->mspec_used
        && attacktype_fordmg(mptr, AT_BREA, AD_FIRE)) {
        odummy = cg.zeroobj; /* otyp == STRANGE_OBJECT */
        return muse_unslime(mon, &odummy, (struct trap *) 0, by_you);
    }

    /* same MUSE criteria as use_defensive() */
    if (!is_animal(mptr) && !mindless(mptr)) {
        struct trap *t;

        for (obj = mon->minvent; obj; obj = obj->nobj)
            if (cures_sliming(mon, obj))
                return muse_unslime(mon, obj, (struct trap *) 0, by_you);

        if (((t = t_at(mon->mx, mon->my)) == 0 || t->ttyp != FIRE_TRAP)
            && mptr->mmove && !mon->mtrapped) {
            coordxy xy[2][8], x, y, idx, ridx, nxy = 0;

            for (x = mon->mx - 1; x <= mon->mx + 1; ++x)
                for (y = mon->my - 1; y <= mon->my + 1; ++y)
                    if (isok(x, y) && accessible(x, y)
                        && !m_at(x, y) && (x != u.ux || y != u.uy)) {
                        xy[0][nxy] = x, xy[1][nxy] = y;
                        ++nxy;
                    }
            for (idx = 0; idx < nxy; ++idx) {
                ridx = rn1(nxy - idx, idx);
                if (ridx != idx) {
                    x = xy[0][idx];
                    xy[0][idx] = xy[0][ridx];
                    xy[0][ridx] = x;
                    y = xy[1][idx];
                    xy[1][idx] = xy[1][ridx];
                    xy[1][ridx] = y;
                }
                if ((t = t_at(xy[0][idx], xy[1][idx])) != 0
                    && t->ttyp == FIRE_TRAP)
                    break;
            }
        }
        if (t && t->ttyp == FIRE_TRAP)
            return muse_unslime(mon, &hands_obj, t, by_you);

    } /* MUSE */

    return FALSE;
}

/* mon uses an item--selected by caller--to burn away incipient slime */
staticfn boolean
muse_unslime(
    struct monst *mon,
    struct obj *obj,
    struct trap *trap,
    boolean by_you) /* true: if mon kills itself, hero gets credit/blame */
{                   /* [by_you not honored if 'mon' triggers fire trap]. */
    struct obj *odummyp;
    int otyp = obj->otyp, dmg = 0;
    boolean vis = canseemon(mon), res = TRUE;

    if (vis)
        pline_mon(mon, "%s starts turning %s.", Monnam(mon),
              green_mon(mon) ? "into ooze" : hcolor(NH_GREEN));
    /* -4 => sliming, causes quiet loss of enhanced speed */
    mon_adjust_speed(mon, -4, (struct obj *) 0);

    if (trap) {
        const char *Mnam = vis ? Monnam(mon) : 0;

        if (mon->mx == trap->tx && mon->my == trap->ty) {
            if (vis)
                pline("%s triggers %s fire trap!", Mnam,
                      trap->tseen ? "the" : "a");
        } else {
            remove_monster(mon->mx, mon->my);
            newsym(mon->mx, mon->my);
            place_monster(mon, trap->tx, trap->ty);
            if (mon->wormno) /* won't happen; worms don't MUSE to unslime */
                worm_move(mon);
            newsym(mon->mx, mon->my);
            if (vis)
                pline("%s %s %s %s fire trap!", Mnam,
                      vtense(fakename[0], locomotion(mon->data, "move")),
                      mon_prop(mon, LEVITATION) ? "over" : "onto",
                      trap->tseen ? "the" : "a");
        }
        (void) mintrap(mon, FORCETRAP);
    } else if (otyp == STRANGE_OBJECT) {
        /* monster is using fire breath on self */
        if (vis)
            pline_mon(mon, "%s.",
                      monverbself(mon, Monnam(mon), "breath", "fire on"));
        if (!rn2(3))
            mon->mspec_used = rn1(10, 5);
        /* -21 => monster's fire breath; 1 => # of damage dice */
        dmg = zhitm(mon, by_you ? 21 : -21, 1, &odummyp);
    } else if (otyp == SCR_FIRE) {
        mreadmsg(mon, obj);
        if (mon->mconf) {
            if (cansee(mon->mx, mon->my))
                pline("Oh, what a pretty fire!");
            if (vis)
                makeknown(SCR_FIRE);
            m_useup(mon, obj); /* after trycall() */
            vis = FALSE;       /* skip makeknown() below */
            res = FALSE;       /* failed to cure sliming */
        } else {
            dmg = (2 * (rn1(3, 3) + 2 * bcsign(obj)) + 1) / 3;
            m_useup(mon, obj); /* before explode() */
            /* -11 => monster's fireball */
            explode(mon->mx, mon->my, BZ_M_SPELL(ZT_FIRE), dmg, SCROLL_CLASS,
                    /* by_you: override -11 for mon but not others */
                    by_you ? -EXPL_FIERY : EXPL_FIERY);
            dmg = 0; /* damage has been applied by explode() */
        }
    } else if (otyp == POT_OIL) {
        char Pronoun[40];
        boolean was_lit = obj->lamplit ? TRUE : FALSE, saw_lit = FALSE;
        /*
         * If not already lit, requires two actions.  We cheat and let
         * monster do both rather than render the potion unusable.
         *
         * Monsters don't start with oil and don't actively pick up oil
         * so this may never occur in a real game.  (Possible though;
         * nymph can steal potions of oil; shapechanger could take on
         * nymph form or vacuum up stuff as a g.cube and then eventually
         * engage with a green slime.)
         */

        if (obj->quan > 1L)
            obj = splitobj(obj, 1L);
        if (vis && !was_lit) {
            pline_mon(mon, "%s ignites %s.", Monnam(mon), ansimpleoname(obj));
            saw_lit = TRUE;
        }
        begin_burn(obj, was_lit);
        vis |= canseemon(mon); /* burning potion may improve visibility */
        if (vis) {
            if (!Unaware)
                obj->dknown = 1; /* hero is watching mon drink obj */
            pline("%s quaffs a burning %s",
                  saw_lit ? upstart(strcpy(Pronoun, mhe(mon))) : Monnam(mon),
                  simpleonames(obj));
            makeknown(POT_OIL);
        }
        dmg = d(3, 4); /* [**TEMP** (different from hero)] */
        m_useup(mon, obj);
    } else { /* wand/horn of fire w/ positive charge count */
        if (obj->otyp == FIRE_HORN) {
            mplayhorn(mon, obj, TRUE);
        } else {
            if (!mzapwand(mon, obj, TRUE))
                return res;
        }
        /* -1 => monster's wand of fire; 2 => # of damage dice */
        dmg = zhitm(mon, by_you ? 1 : -1, 2, &odummyp);
    }

    if (dmg) {
        /* zhitm() applies damage but doesn't kill creature off;
           for fire breath, dmg is going to be 0 (fire breathers are
           immune to fire damage) but for wand of fire or fire horn,
           'mon' could have taken damage so might die */
        if (DEADMONSTER(mon)) {
            if (by_you) {
                /* mon killed self but hero gets credit and blame (except
                   for pacifist conduct); xkilled()'s message would say
                   "You killed/destroyed <mon>" so give our own message */
                if (vis)
                    pline_mon(mon, "%s is %s by the fire!", Monnam(mon),
                          nonliving(mon->data) ? "destroyed" : "killed");
                xkilled(mon, XKILL_NOMSG | XKILL_NOCONDUCT);
            } else
                monkilled(mon, "fire", AD_FIRE);
        } else {
            /* non-fatal damage occurred */
            if (vis)
                pline_mon(mon, "%s is burned%s", Monnam(mon), exclam(dmg));
        }
    }
    if (vis) {
        if (res && !DEADMONSTER(mon))
            pline_mon(mon, "%s slime is burned away!", s_suffix(Monnam(mon)));
        if (otyp != STRANGE_OBJECT)
            makeknown(otyp);
    }
    /* use up monster's next move */
    mon->movement -= NORMAL_SPEED;
    mon->mlstmv = svm.moves;
    return res;
}

/* decide whether obj can be used to cure green slime */
staticfn int
cures_sliming(struct monst *mon, struct obj *obj)
{
    /* scroll of fire */
    if (obj->otyp == SCR_FIRE)
        return (haseyes(mon->data) && mon->mcansee && !nohands(mon->data));

    /* potion of oil; will be set burning if not already */
    if (obj->otyp == POT_OIL)
        return !nohands(mon->data);

    /* non-empty wand or horn of fire;
       hero doesn't need hands or even limbs to zap, so mon doesn't either */
    return ((obj->otyp == WAN_FIRE
             || (obj->otyp == FIRE_HORN && can_blow(mon) && obj->spe > 0)));
}

/* TRUE if monster appears to be green; we go by the display color.
   The alternative was to just pick things that
   seem plausibly green (which didn't necessarily match the categorization
   by the color of the text).
   iflags.use_color is not meant for game behavior decisions */
staticfn boolean
green_mon(struct monst *mon)
{
    struct permonst *ptr = mon->data;

    if (Hallucination)
        return FALSE;
    return (ptr->mcolor == CLR_GREEN || ptr->mcolor == CLR_BRIGHT_GREEN);
#if 0
    if (iflags.use_color)
        return (ptr->mcolor == CLR_GREEN || ptr->mcolor == CLR_BRIGHT_GREEN);
    else {
        /* approximation */
        if (strstri(ptr->pmnames[NEUTRAL], "green")
            || (ptr->pmnames[MALE] && strstri(ptr->pmnames[MALE], "green"))
            || (ptr->pmnames[FEMALE]
                && strstri(ptr->pmnames[FEMALE], "green")))
            return TRUE;
        switch (monsndx(ptr)) {
        case PM_FOREST_CENTAUR:
        case PM_GARTER_SNAKE:
        case PM_GECKO:
        case PM_GREMLIN:
        case PM_HOMUNCULUS:
        case PM_JUIBLEX:
        case PM_LEPRECHAUN:
        case PM_LICHEN:
        case PM_LIZARD:
        case PM_WOOD_NYMPH:
            return TRUE;
        default:
            if (is_elf(ptr) && !is_prince(ptr) && !is_lord(ptr)
                && ptr != &mons[PM_GREY_ELF])
                return TRUE;
            break;
        }
    }
    return FALSE;
#endif
}

staticfn int
muse_wonder(void)
{
    int wondertemp = 0;

    while (gm.m.has_offense == MUSE_WAN_WONDER) {
        /* Choose a random wand */
        wondertemp = WAN_LIGHT + rn2(LAST_WAND - WAN_LIGHT);

        /* Be a little lenient... */
        if (wondertemp == WAN_DEATH && rn2(2))
            wondertemp = WAN_DRAINING;

        switch (wondertemp) {
        case WAN_DEATH:
            gm.m.has_offense = MUSE_WAN_DEATH;
            break;
        case WAN_DRAINING:
            gm.m.has_offense = MUSE_WAN_DRAINING;
            break;
        case WAN_SLEEP:
            gm.m.has_offense = MUSE_WAN_SLEEP;
            break;
        case WAN_FIRE:
            gm.m.has_offense = MUSE_WAN_FIRE;
            break;
        case WAN_COLD:
            gm.m.has_offense = MUSE_WAN_COLD;
            break;
        case WAN_LIGHTNING:
            gm.m.has_offense = MUSE_WAN_LIGHTNING;
            break;
        case WAN_MAGIC_MISSILE:
            gm.m.has_offense = MUSE_WAN_MAGIC_MISSILE;
            break;
        case WAN_CORROSION:
            gm.m.has_offense = MUSE_WAN_CORROSION;
            break;
        case WAN_POISON_GAS:
            gm.m.has_offense = MUSE_WAN_POISON_GAS;
            break;
        case WAN_TELEPORTATION:
            gm.m.has_offense = MUSE_WAN_TELEPORTATION;
            break;
        case WAN_UNDEAD_TURNING:
            gm.m.has_offense = MUSE_WAN_UNDEAD_TURNING;
            break;
        case WAN_STRIKING:
            gm.m.has_offense = MUSE_WAN_STRIKING;
            break;
        case WAN_CANCELLATION:
            gm.m.has_offense = MUSE_WAN_CANCELLATION;
            break;
        case WAN_SLOW_MONSTER:
            gm.m.has_offense = MUSE_WAN_SLOW_MONSTER;
            break;
        case WAN_POLYMORPH:
            /* Monsters can only get poly zaps here via
             * wands of wonder, so they are rare. */
            gm.m.has_offense = MUSE_WAN_POLYMORPH;
            break;
        case WAN_CREATE_MONSTER:
            gm.m.has_offense = MUSE_WAN_CREATE_MONSTER;
            break;

        /* Some of these could be crafted to their own effects, but for now
         * having them default to stun ray reduces the chances of other
         * really nasty effects (ie: death, polymorph, cancellation, etc) */
        case WAN_ENLIGHTENMENT:
        case WAN_PROBING:
        case WAN_SECRET_DOOR_DETECTION:
        case WAN_LIGHT:
        case WAN_OPENING:
        case WAN_LOCKING:
        case WAN_DIGGING:
        case WAN_MAKE_INVISIBLE:
        case WAN_SPEED_MONSTER:
        case WAN_WISHING:
            /* This is a weird kludge until we (maybe) implement wands
             * of stunning */
            gm.m.has_offense = MUSE_WAN_STUNNING;
            wondertemp = WAN_DRAINING + 1;
            break;
        default:
            ;
        }
    }
    return wondertemp;
}

/* Do you have some sort of reflectable ray attack available? */
staticfn boolean
reflectable_offense(void)
{
    struct obj *otmp;
    for (otmp = gi.invent; otmp; otmp = otmp->nobj) {
        switch (otmp->otyp) {
            /* ray-based wands */
            case WAN_MAGIC_MISSILE:
            case WAN_FIRE:
            case WAN_COLD:
            case WAN_SLEEP:
            case WAN_DEATH:
            case WAN_LIGHTNING:
            case WAN_POISON_GAS:
            case WAN_CORROSION:
            case WAN_DRAINING:
            case FROST_HORN:
            case FIRE_HORN:
                if (otmp->spe > 0)
                    return TRUE;
        }
    }

    /* Can they know what spells you have?
     * There is no other way to track that... */
    if (known_spell(SPE_MAGIC_MISSILE) >= spe_Fresh)
        return TRUE;
    if (known_spell(SPE_CONE_OF_COLD) >= spe_Fresh)
        return TRUE;
    if (known_spell(SPE_SLEEP) >= spe_Fresh)
        return TRUE;
    if (known_spell(SPE_LIGHTNING) >= spe_Fresh)
        return TRUE;
    if (known_spell(SPE_POISON_BLAST) >= spe_Fresh)
        return TRUE;
    if (known_spell(SPE_FINGER_OF_DEATH) >= spe_Fresh)
        return TRUE;

    /* Are you polyd into something with a ray breath? */
    if (Upolyd && attacktype(gy.youmonst.data, AT_BREA)) {
        return TRUE;
    }

    return FALSE;
}

staticfn int
muse_createmonster(struct monst *mtmp, struct obj *otmp)
{
    coord cc;
    struct monst *mon;
    static const char MissingDefensiveItem[] = "use_defensive: no %s";
    /* pm: 0 => random, eel => aquatic, croc => amphibious */
    struct permonst *pm = !is_damp_terrain(mtmp->mx, mtmp->my) ? 0
                        : &mons[u.uinwater ? PM_GIANT_EEL : PM_CROCODILE];

    if (!otmp)
        panic(MissingDefensiveItem, "wand of create monster");
    if (!enexto(&cc, mtmp->mx, mtmp->my, pm))
        return 0;
    if (mzapwand(mtmp, otmp, FALSE)) {
        mon = makemon((struct permonst *) 0, cc.x, cc.y, NO_MM_FLAGS);
        if (mon && canspotmon(mon) && canseemon(mtmp))
            makeknown(WAN_CREATE_MONSTER);
    }
    return 2;
}

/*muse.c*/
