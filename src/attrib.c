/* NetHack 3.7	attrib.c	$NHDT-Date: 1726168587 2024/09/12 19:16:27 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.129 $ */
/*      Copyright 1988, 1989, 1990, 1992, M. Stephenson           */
/* NetHack may be freely redistributed.  See license for details. */

/*  attribute modification routines. */

#include "hack.h"

/* part of the output on gain or loss of attribute */
static const char
    *const plusattr[] = { "strong", "smart", "wise",
                          "agile",  "tough", "charismatic" },
    *const minusattr[] = { "weak",    "stupid",
                           "foolish", "clumsy",
                           "fragile", "repulsive" };
/* also used by enlightenment in insight.c for non-abbreviated status info */
extern const char *const attrname[6];

const char
    *const attrname[] = { "strength", "intelligence", "wisdom",
                          "dexterity", "constitution", "charisma" };

static const struct innate {
    schar ulevel;
    long *ability;
    const char *gainstr, *losestr;
} arc_abil[] = { { 1, &(HSearching), "", "" },
                 { 5, &(HStealth), "stealthy", "" },
                 { 10, &(HFast), "quick", "slow" },
                 { 0, 0, 0, 0 } },

  bar_abil[] = { { 1, &(HPoison_resistance), "", "" },
                 { 7, &(HFast), "quick", "slow" },
                 { 15, &(HStealth), "stealthy", "" },
                 { 0, 0, 0, 0 } },

  car_abil[] = { { 1, &(HSearching), "perceptive", "unaware" },
                 { 7, &(HWarning), "in touch with the cards", "insensitive" },
                 { 15, &(HFast), "quick", "slow" },
                 { 0, 0, 0, 0 } },

  cav_abil[] = { { 7, &(HFast), "quick", "slow" },
                 { 15, &(HWarning), "sensitive", "" },
                 { 0, 0, 0, 0 } },

  hea_abil[] = { { 1, &(HPoison_resistance), "", "" },
                 { 15, &(HWarning), "sensitive", "" },
                 { 0, 0, 0, 0 } },

  kni_abil[] = { { 7, &(HFast), "quick", "slow" }, { 0, 0, 0, 0 } },

  mon_abil[] = { { 1, &(HFast), "", "" },
                 { 1, &(HSleep_resistance), "", "" },
                 { 1, &(HSee_invisible), "", "" },
                 { 3, &(HPoison_resistance), "healthy", "" },
                 { 5, &(HStealth), "stealthy", "" },
                 { 7, &(HWarning), "sensitive", "" },
                 { 9, &(HSearching), "perceptive", "unaware" },
                 { 11, &(HFire_resistance), "cool", "warmer" },
                 { 13, &(HCold_resistance), "warm", "cooler" },
                 { 15, &(HShock_resistance), "insulated", "conductive" },
                 { 17, &(HTeleport_control), "controlled", "uncontrolled" },
                 { 19, &(HAcid_resistance), "thick-skinned","soft-skinned" },
                 { 0, 0, 0, 0 } },

  pri_abil[] = { { 15, &(HWarning), "sensitive", "" },
                 { 20, &(HFire_resistance), "cool", "warmer" },
                 { 0, 0, 0, 0 } },

  ran_abil[] = { { 1, &(HSearching), "", "" },
                 { 7, &(HStealth), "stealthy", "" },
                 { 15, &(HSee_invisible), "insightful", "obtuse" },
                 { 0, 0, 0, 0 } },

  rog_abil[] = { { 1, &(HStealth), "", "" },
                 { 10, &(HSearching), "perceptive", "" },
                 { 0, 0, 0, 0 } },

  sam_abil[] = { { 1, &(HFast), "", "" },
                 { 15, &(HStealth), "stealthy", "" },
                 { 0, 0, 0, 0 } },

  tou_abil[] = { { 10, &(HSearching), "perceptive", "" },
                 { 20, &(HPoison_resistance), "hardy", "" },
                 { 0, 0, 0, 0 } },

  und_abil[] = { { 1, &(HUndead_warning), "", "" },
                 { 1, &(HDrain_resistance), "", "" },
                 { 1, &(HSick_resistance), "", "" },
                 { 1, &(HStealth), "", "" },
                 { 10, &(HFast), "quick", "slow" },
                 { 15, &(HPoison_resistance), "hardy", "" },
                 { 0, 0, 0, 0 } },

  val_abil[] = { { 1, &(HCold_resistance), "", "" },
                 { 3, &(HStealth), "stealthy", "" },
                 { 7, &(HFast), "quick", "slow" },
                 { 0, 0, 0, 0 } },

  wiz_abil[] = { { 15, &(HWarning), "sensitive", "" },
                 { 17, &(HTeleport_control), "controlled", "uncontrolled" },
                 { 0, 0, 0, 0 } },

  /* Intrinsics conferred by race */
  dwa_abil[] = { { 0, 0, 0, 0 } },

  elf_abil[] = { { 4, &HSleep_resistance, "awake", "tired" },
                 { 8, &(HSee_invisible), "insightful", "obtuse" },
                 { 0, 0, 0, 0 } },

  gno_abil[] = { { 5, &(HStealth), "stealthy", "" },
                 { 0, 0, 0, 0 } },

  orc_abil[] = { { 1, &HPoison_resistance, "", "" },
                 { 0, 0, 0, 0 } },

  vam_abil[] = { { 1, &HDrain_resistance, "", "" },
                 { 1, &HBreathless, "", "" },
                 { 1, &HHunger, "", "" },
                 /* dhampir start out only able to use their bite or weapon
                    attack in melee combat, but not both in the same round.
                    At level 6, they silently unlock the ability to use both. */
                 { 0, 0, 0, 0 } },

  gru_abil[] = { { 1, &HPoison_resistance, "", "" },
                 { 1, &(HSearching), "", "" },
                 { 1, &(HSwimming), "", "" },
                 { 5, &(EJumping), "light on your feet", "stuck to the ground" },
                 { 7, &(HFast), "quick", "slow" },
                 { 0, 0, 0, 0 } },

  hum_abil[] = { { 0, 0, 0, 0 } };

staticfn void exerper(void);
staticfn int rnd_attr(void);
staticfn int init_attr_role_redist(int, boolean);
staticfn void postadjabil(long *);
staticfn const struct innate *role_abil(int);
staticfn const struct innate *check_innate_abil(long *, long);
staticfn int innately(long *);

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(
    int ndx,    /* which characteristic */
    int incr,   /* amount of change */
    int msgflg) /* positive => no message, zero => message, and */
{               /* negative => conditional (msg if change made) */
    int old_acurr, old_abase, old_amax, decr;
    boolean abonflg;
    const char *attrstr;

    if (Fixed_abil || !incr)
        return FALSE;

    if ((ndx == A_INT || ndx == A_WIS) && uarmh && uarmh->otyp == DUNCE_CAP) {
        if (msgflg == 0)
            Your("cap constricts briefly, then relaxes again.");
        return FALSE;
    }

    old_acurr = ACURR(ndx);
    old_abase = ABASE(ndx);
    old_amax = AMAX(ndx);
    ABASE(ndx) += incr; /* when incr is negative, this reduces ABASE() */
    if (incr > 0) {
        if (ABASE(ndx) > AMAX(ndx)) {
            AMAX(ndx) = ABASE(ndx);
            if (AMAX(ndx) > ATTRMAX(ndx))
                ABASE(ndx) = AMAX(ndx) = ATTRMAX(ndx);
        }
        attrstr = plusattr[ndx];
        abonflg = (ABON(ndx) < 0);
    } else { /* incr is negative */
        if (ABASE(ndx) < ATTRMIN(ndx)) {
            /*
             * If base value has dropped so low that it is trying to be
             * taken below the minimum, reduce max value (peak reached)
             * instead.  That means that restore ability and repeated
             * applications of unicorn horn will not be able to recover
             * all the lost value.  As of 3.6.2, we only take away
             * some (average half, possibly zero) of the excess from max
             * instead of all of it, but without intervening recovery, it
             * can still eventually drop to the minimum allowed.  After
             * that, it can't be recovered, only improved with new gains.
             *
             * This used to assign a new negative value to incr and then
             * add it, but that could affect messages below, possibly
             * making a large decrease be described as a small one.
             *
             * decr = rn2(-(ABASE - ATTRMIN) + 1);
             */
            decr = rn2(ATTRMIN(ndx) - ABASE(ndx) + 1);
            ABASE(ndx) = ATTRMIN(ndx);
            AMAX(ndx) -= decr;
            if (AMAX(ndx) < ATTRMIN(ndx))
                AMAX(ndx) = ATTRMIN(ndx);
        }
        attrstr = minusattr[ndx];
        abonflg = (ABON(ndx) > 0);
    }
    if (ACURR(ndx) == old_acurr) {
        if (msgflg == 0 && flags.verbose) {
            if (ABASE(ndx) == old_abase && AMAX(ndx) == old_amax) {
                pline("You're %s as %s as you can get.",
                      abonflg ? "currently" : "already", attrstr);
            } else {
                /* current stayed the same but base value changed, or
                   base is at minimum and reduction caused max to drop */
                Your("innate %s has %s.", attrname[ndx],
                     (incr > 0) ? "improved" : "declined");
            }
        }
        return FALSE;
    }

    disp.botl = TRUE;
    if (msgflg <= 0)
        You_feel("%s%s!", (incr > 1 || incr < -1) ? "very " : "", attrstr);
    if (program_state.in_moveloop && (ndx == A_STR || ndx == A_CON))
        (void) encumber_msg();
    return TRUE;
}

/* strength gain */
void
gainstr(struct obj *otmp, int incr, boolean givemsg)
{
    int num = incr;

    if (!num) {
        if (ABASE(A_STR) < 18)
            num = (rn2(4) ? 1 : rnd(3));
        else if (ABASE(A_STR) < STR18(85))
            num = rnd(5);
        else
            num = 1;
    }
    (void) adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num,
                     givemsg ? -1 : 1);
}

/* strength loss, may kill you; cause may be poison or monster like 'a' */
void
losestr(int num, const char *knam, schar k_format)
{
    int uhpmin = minuhpmax(1), olduhpmax = u.uhpmax;
    int ustr = ABASE(A_STR) - num, amt, dmg;
    boolean waspolyd = Upolyd;

    if (num <= 0 || ABASE(A_STR) < ATTRMIN(A_STR)) {
        impossible("losestr: %d - %d", ABASE(A_STR), num);
        return;
    }
    dmg = 0;
    while (ustr < ATTRMIN(A_STR)) {
        ++ustr;
        --num;
        amt = rn1(4, 3); /* (0..(4-1))+3 => 3..6; used to use flat 6 here */
        dmg += amt;
    }
    if (dmg) {
        /* in case damage is fatal and caller didn't supply killer reason */
        if (!knam || !*knam) {
            knam = "terminal frailty";
            k_format = KILLED_BY;
        }
        losehp(dmg, knam, k_format);

        if (Upolyd) {
            /* when still poly'd, reduce you-as-monst maxHP; never below 1 */
            setuhpmax(max(u.mhmax - dmg, 1), FALSE); /* acts as setmhmax() */
        } else if (!waspolyd) {
            /* not polymorphed now and didn't rehumanize when taking damage;
               reduce max HP, but not below uhpmin */
            if (u.uhpmax > uhpmin)
                setuhpmax(max(u.uhpmax - dmg, uhpmin), FALSE);
        }
        disp.botl = TRUE;
    }
#if 0   /* only possible if uhpmax was already less than uhpmin */
    if (!Upolyd && u.uhpmax < uhpmin) {
        setuhpmax(min(olduhpmax, uhpmin), FALSE);
        if (!Drain_resistance)
            losexp(NULL); /* won't be fatal when no 'drainer' is supplied */
    }
#else
    nhUse(olduhpmax);
#endif
    /* 'num' could have been reduced to 0 in the minimum strength loop;
       '(Upolyd || !waspolyd)' is True unless damage caused rehumanization */
    if (num > 0 && (Upolyd || !waspolyd))
        (void) adjattrib(A_STR, -num, 1);
}

/* combined strength loss and damage from some poisons */
void
poison_strdmg(int strloss, int dmg, const char *knam, schar k_format)
{
    strloss = resist_reduce(strloss, POISON_RES);
    /* strloss may be 0 due to 100% poison resistance */
    if (strloss > 0)
        losestr(strloss, knam, k_format);

    dmg = resist_reduce(dmg, POISON_RES);
    if (dmg > 0)
        losehp(dmg, knam, k_format);
}

static const struct poison_effect_message {
    void (*delivery_func)(const char *, ...);
    const char *effect_msg;
} poiseff[] = {
    { You_feel, "weaker" },             /* A_STR */
    { Your, "brain is on fire" },       /* A_INT */
    { Your, "judgement is impaired" },  /* A_WIS */
    { Your, "muscles won't obey you" }, /* A_DEX */
    { You_feel, "very sick" },          /* A_CON */
    { You, "break out in hives" }       /* A_CHA */
};

/* feedback for attribute loss due to poisoning */
void
poisontell(int typ,         /* which attribute */
           boolean exclaim) /* emphasis */
{
    void (*func)(const char *, ...) = poiseff[typ].delivery_func;
    const char *msg_txt = poiseff[typ].effect_msg;

    /*
     * "You feel weaker" or "you feel very sick" aren't appropriate when
     * wearing or wielding something (gauntlets of power, Ogresmasher)
     * which forces the attribute to maintain its maximum value.
     * Phrasing for other attributes which might have fixed values
     * (dunce cap) is such that we don't need message fixups for them.
     */
    if (typ == A_STR && ACURR(A_STR) == STR19(25))
        msg_txt = "innately weaker";
    else if (typ == A_CON && ACURR(A_CON) == 25)
        msg_txt = "sick inside";

    (*func)("%s%c", msg_txt, exclaim ? '!' : '.');
}

/* called when an attack or trap has poisoned hero (used to be in mon.c) */
void
poisoned(
    const char *reason,    /* controls what messages we display */
    int typ,
    const char *pkiller,   /* for score+log file if fatal */
    int fatal,             /* if fatal is 0, limit damage to adjattrib */
    boolean thrown_weapon) /* thrown weapons are less deadly */
{
    int i, loss, kprefix = KILLED_BY_AN;
    boolean blast = !strcmp(reason, "blast");

    /* inform player about being poisoned unless that's already been done;
       "blast" has given a "blast of poison gas" message; "poison arrow",
       "poison dart", etc have implicitly given poison messages too... */
    if (!blast && !strstri(reason, "poison")) {
        boolean plural = (reason[strlen(reason) - 1] == 's') ? 1 : 0;

        /* avoid "The" Orcus's sting was poisoned... */
        pline("%s%s %s poisoned!",
              isupper((uchar) *reason) ? "" : "The ", reason,
              plural ? "were" : "was");
    }
    if (fully_resistant(POISON_RES)) {
        if (blast)
            shieldeff(u.ux, u.uy);
        pline_The("poison doesn't seem to affect you.");
        return;
    }

    /* suppress killer prefix if it already has one */
    i = name_to_mon(pkiller, (int *) 0);
    if (ismnum(i) && (mons[i].geno & G_UNIQ)) {
        kprefix = KILLED_BY;
        if (!type_is_pname(&mons[i]))
            pkiller = the(pkiller);
    } else if (!strncmpi(pkiller, "the ", 4) || !strncmpi(pkiller, "an ", 3)
               || !strncmpi(pkiller, "a ", 2)) {
        /*[ does this need a plural check too? ]*/
        kprefix = KILLED_BY;
    }

    /*
     * FIXME:
     *  this operates on u.uhp[max] even when hero is polymorphed....
     */

    i = !fatal ? 1 : rn2(fatal + (thrown_weapon ? 20 : 0));
    if (i == 0 && typ != A_CHA) {
        /* sometimes survivable instant kill */
        loss = resist_reduce(6 + d(4, 6), POISON_RES); /* 6 + 4d6 => 10..34 */
        if (u.uhp <= loss) {
            u.uhp = -1;
            disp.botl = TRUE;
            pline_The("poison was deadly...");
        } else {
            /* survived, but with severe reaction */
            int olduhp = u.uhp,
                newuhpmax = u.uhpmax - (loss / 2);

            setuhpmax(max(newuhpmax, minuhpmax(3)), TRUE); /*True: see FIXME*/
            loss = adjuhploss(loss, olduhp);

            losehp(loss, pkiller, kprefix); /* poison damage */
            if (adjattrib(A_CON, (typ != A_CON) ? -1 : -3, TRUE))
                poisontell(A_CON, TRUE);
            if (typ != A_CON && adjattrib(typ, -3, 1))
                poisontell(typ, TRUE);
        }
    } else if (i > 5) {
        boolean cloud = !strcmp(reason, "gas cloud");

        /* HP damage; more likely--but less severe--with missiles */
        loss = thrown_weapon ? rnd(6) : rn1(10, 6);
        loss = resist_reduce(loss, POISON_RES);
        if ((blast || cloud) && No_gas_damage) /* worn towel */
            loss = (loss + 1) / 2;
        losehp(loss, pkiller, kprefix); /* poison damage */
    } else {
        /* attribute loss; if typ is A_STR, reduction in current and
           maximum HP will occur once strength has dropped down to 3 */
        loss = (thrown_weapon || !fatal)
                   ? 1 : resist_reduce(d(2, 2), POISON_RES); /* was rn1(3,3) */

        /* check that a stat change was made */
        if (adjattrib(typ, -loss, 1))
            poisontell(typ, TRUE);
    }

    if (u.uhp > u.uhpmax)
        u.uhp = u.uhpmax; /* Safeguard */

    if (u.uhp < 1) {
        svk.killer.format = kprefix;
        Strcpy(svk.killer.name, pkiller);
        /* "Poisoned by a poisoned ___" is redundant */
        done(strstri(pkiller, "poison") ? DIED : POISONING);
    }
    (void) encumber_msg();
}

void
change_luck(schar n)
{
    u.uluck += n;
    if (u.uluck < 0 && u.uluck < LUCKMIN)
        u.uluck = LUCKMIN;
    if (u.uluck > 0 && u.uluck > LUCKMAX)
        u.uluck = LUCKMAX;
}

/* decide whether there are more blessed luckstones (plus luck-conferring
   artifacts) than cursed ones; optionally combine uncursed with blessed */
int
stone_luck(boolean include_uncursed)
{
    struct obj *otmp;
    long bonchance = 0;

    for (otmp = gi.invent; otmp; otmp = otmp->nobj)
        if (confers_luck(otmp)) {
            if (otmp->cursed)
                bonchance -= otmp->quan;
            else if (otmp->blessed || include_uncursed)
                bonchance += otmp->quan;
        }

    return sgn((int) bonchance);
}

boolean
has_luckitem(void) {
    struct obj *otmp;

    for (otmp = gi.invent; otmp; otmp = otmp->nobj)
        if (confers_luck(otmp))
            return TRUE;
    return FALSE;
}

/* there has just been an inventory change affecting a luck-granting item */
void
set_moreluck(void)
{
    if (!has_luckitem())
        u.moreluck = 0;
    else if (stone_luck(TRUE) >= 0)
        u.moreluck = LUCKADD;
    else
        u.moreluck = -LUCKADD;
}

/* KMH, balance patch -- new function for healthstones */
void
recalc_health(void)
{
    struct obj *otmp;

    u.uhealbonus = 0;
    for (otmp = gi.invent; otmp; otmp = otmp->nobj)
        if (otmp->otyp == HEALTHSTONE)
            u.uhealbonus += otmp->quan *
                            (otmp->blessed ? 2 : otmp->cursed ? -2 : 1);
    return;
}

/* (not used) */
void
restore_attrib(void)
{
    int i, equilibrium;;

    /*
     * Note:  this used to get called by moveloop() on every turn but
     * ATIME() is never set to non-zero anywhere so didn't do anything.
     * Presumably it once supported something like potion of heroism
     * which conferred temporary characteristics boost(s).
     *
     * ATEMP() is used for strength loss from hunger, which doesn't
     * time out, and for dexterity loss from wounded legs, which has
     * its own timeout routine.
     */

    for (i = 0; i < A_MAX; i++) { /* all temporary losses/gains */
        equilibrium = ((i == A_STR && u.uhs >= WEAK)
                       || (i == A_DEX && Wounded_legs)) ? -1 : 0;
        if (ATEMP(i) != equilibrium && ATIME(i) != 0) {
            if (!(--(ATIME(i)))) { /* countdown for change */
                ATEMP(i) += (ATEMP(i) > 0) ? -1 : 1;
                disp.botl = TRUE;
                if (ATEMP(i)) /* reset timer */
                    ATIME(i) = 100 / ACURR(A_CON);
            }
        }
    }
    if (disp.botl)
        (void) encumber_msg();
}

#define AVAL 50 /* tune value for exercise gains */

void
exercise(int i, boolean inc_or_dec)
{
    debugpline0("Exercise:");
    if (i == A_INT || i == A_CHA)
        return; /* can't exercise these */

    /* no physical exercise while polymorphed; the body's temporary */
    if (Upolyd && i != A_WIS)
        return;

    if (abs(AEXE(i)) < AVAL) {
        /*
         *      Law of diminishing returns (Part I):
         *
         *      Gain is harder at higher attribute values.
         *      79% at "3" --> 0% at "18"
         *      Loss is even at all levels (50%).
         *
         *      Note: *YES* ACURR is the right one to use.
         */
        AEXE(i) += (inc_or_dec) ? (rn2(19) > ACURR(i)) : -rn2(2);
        debugpline3("%s, %s AEXE = %d",
                    (i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" : (i == A_DEX)
                                                                      ? "Dex"
                                                                      : "Con",
                    (inc_or_dec) ? "inc" : "dec", AEXE(i));
    }
    if (svm.moves > 0 && (i == A_STR || i == A_CON))
        (void) encumber_msg();
}

staticfn void
exerper(void)
{
    if (!(svm.moves % 10)) {
        /* Hunger Checks */
        int hs = (u.uhunger > 1000) ? SATIATED
                 : (u.uhunger > 150) ? NOT_HUNGRY
                   : (u.uhunger > 50) ? HUNGRY
                     : (u.uhunger > 0) ? WEAK
                       : FAINTING;

        debugpline0("exerper: Hunger checks");
        switch (hs) {
        case SATIATED:
	        /* Don't punish vampires for eating too much */
            if (!maybe_polyd(is_vampire(gy.youmonst.data), Race_if(PM_DHAMPIR)))
                exercise(A_DEX, FALSE);
            if (Role_if(PM_MONK) || Role_if(PM_SAMURAI))
                exercise(A_WIS, FALSE);
            break;
        case NOT_HUNGRY:
            exercise(A_CON, TRUE);
            break;
        case WEAK:
            exercise(A_STR, FALSE);
            if (Role_if(PM_MONK)) /* fasting */
                exercise(A_WIS, TRUE);
            break;
        case FAINTING:
        case FAINTED:
            exercise(A_CON, FALSE);
            break;
        }

        /* Encumbrance Checks */
        debugpline0("exerper: Encumber checks");
        switch (near_capacity()) {
        case MOD_ENCUMBER:
            exercise(A_STR, TRUE);
            break;
        case HVY_ENCUMBER:
            exercise(A_STR, TRUE);
            exercise(A_DEX, FALSE);
            break;
        case EXT_ENCUMBER:
            exercise(A_DEX, FALSE);
            exercise(A_CON, FALSE);
            break;
        }
    }

    /* status checks */
    if (!(svm.moves % 5)) {
        debugpline0("exerper: Status checks");
        if ((HClairvoyant & (INTRINSIC | TIMEOUT)) && !BClairvoyant)
            exercise(A_WIS, TRUE);
        if (HRegeneration)
            exercise(A_STR, TRUE);

        if (Sick || Vomiting)
            exercise(A_CON, FALSE);
        if (Confusion || Hallucination)
            exercise(A_WIS, FALSE);
        if ((Wounded_legs && !u.usteed) || Fumbling || HStun)
            exercise(A_DEX, FALSE);
    }
}

/* exercise/abuse text (must be in attribute order, not botl order);
   phrased as "You must have been [][0]." or "You haven't been [][1]." */
static NEARDATA const char *const exertext[A_MAX][2] = {
    { "exercising diligently", "exercising properly" },           /* Str */
    { 0, 0 },                                                     /* Int */
    { "very observant", "paying attention" },                     /* Wis */
    { "working on your reflexes", "working on reflexes lately" }, /* Dex */
    { "leading a healthy life-style", "watching your health" },   /* Con */
    { 0, 0 },                                                     /* Cha */
};

void
exerchk(void)
{
    int i, ax, mod_val, lolim, hilim;

    /*  Check out the periodic accumulations */
    exerper();

    if (svm.moves >= svc.context.next_attrib_check) {
        debugpline1("exerchk: ready to test. multi = %ld.", gm.multi);
    }
    /*  Are we ready for a test? */
    if (svm.moves >= svc.context.next_attrib_check && !gm.multi) {
        debugpline0("exerchk: testing.");
        /*
         *      Law of diminishing returns (Part II):
         *
         *      The effects of "exercise" and "abuse" wear
         *      off over time.  Even if you *don't* get an
         *      increase/decrease, you lose some of the
         *      accumulated effects.
         */
        for (i = 0; i < A_MAX; ++i) {
            ax = AEXE(i);
            /* nothing to do here if no exercise or abuse has occurred
               (Int and Cha always fall into this category) */
            if (!ax)
                continue; /* ok to skip nextattrib */

            mod_val = sgn(ax); /* +1 or -1; used below */
            /* no further effect for exercise if at max or abuse if at min;
               can't exceed 18 via exercise even if actual max is higher */
            lolim = ATTRMIN(i); /* usually 3; might be higher */
            hilim = ATTRMAX(i); /* usually 18; maybe lower or higher */
            if (hilim > 18)
                hilim = 18;
            if ((ax < 0) ? (ABASE(i) <= lolim) : (ABASE(i) >= hilim))
                goto nextattrib;
            /* can't exercise non-Wisdom while polymorphed; previous
               exercise/abuse gradually wears off without impact then */
            if (Upolyd && i != A_WIS)
                goto nextattrib;

            debugpline2("exerchk: testing %s (%d).",
                        (i == A_STR) ? "Str"
                        : (i == A_INT) ? "Int?"
                          : (i == A_WIS) ? "Wis"
                            : (i == A_DEX) ? "Dex"
                              : (i == A_CON) ? "Con"
                                : (i == A_CHA) ? "Cha?"
                                  : "???",
                        ax);
            /*
             *  Law of diminishing returns (Part III):
             *
             *  You don't *always* gain by exercising.
             *  [MRS 92/10/28 - Treat Wisdom specially for balance.]
             */
            if (rn2(AVAL) > ((i != A_WIS) ? (abs(ax) * 2 / 3) : abs(ax)))
                goto nextattrib;

            debugpline1("exerchk: changing %d.", i);
            if (adjattrib(i, mod_val, -1)) {
                debugpline1("exerchk: changed %d.", i);
                /* if you actually changed an attrib - zero accumulation */
                AEXE(i) = ax = 0;
                /* then print an explanation */
                You("%s %s.",
                    (mod_val > 0) ? "must have been" : "haven't been",
                    exertext[i][(mod_val > 0) ? 0 : 1]);
            }
 nextattrib:
            /* this used to be ``AEXE(i) /= 2'' but that would produce
               platform-dependent rounding/truncation for negative vals */
            AEXE(i) = (abs(ax) / 2) * mod_val;
        }
        svc.context.next_attrib_check += rn1(200, 800);
        debugpline1("exerchk: next check at %ld.",
                    svc.context.next_attrib_check);
    }
}

/* return random hero attribute (by role's attr distribution).
   returns A_MAX if failed. */
staticfn int
rnd_attr(void)
{
    int i, x = rn2(100);

    /* 3.7: the x -= ... calculation used to have an off by 1 error that
       resulted in the values being biased toward Str and away from Cha */
    for (i = 0; i < A_MAX; ++i)
        if ((x -= gu.urole.attrdist[i]) < 0)
            break;
    return i;
}

/* add or subtract np points from random attributes,
   adjusting the base and maximum values of the attributes.
   if subtracting, np must be negative.
   returns the left over points. */
staticfn int
init_attr_role_redist(int np, boolean addition)
{
    int tryct = 0;
    int adj = addition ? 1 : -1;

    while ((addition ? (np > 0) : (np < 0)) && tryct < 100) {
        int i = rnd_attr();

        if (i >= A_MAX || ABASE(i) >= ATTRMAX(i)) {
            tryct++;
            continue;
        }
        tryct = 0;
        ABASE(i) += adj;
        AMAX(i) += adj;
        np -= adj;
    }
    return np;
}

/* allocate hero's initial characteristics */
void
init_attr(int np)
{
    int i;

    for (i = 0; i < A_MAX; i++) {
        ABASE(i) = AMAX(i) = gu.urole.attrbase[i];
        ATEMP(i) = ATIME(i) = 0;
        np -= gu.urole.attrbase[i];
    }

    /* distribute leftover points */
    np = init_attr_role_redist(np, TRUE);
    /* if we went over, remove points */
    np = init_attr_role_redist(np, FALSE);
}

void
redist_attr(void)
{
    int i, tmp;

    for (i = 0; i < A_MAX; i++) {
        if (i == A_INT || i == A_WIS)
            continue;
        /* Polymorphing doesn't change your mind */
        tmp = AMAX(i);
        AMAX(i) += (rn2(5) - 2);
        if (AMAX(i) > ATTRMAX(i))
            AMAX(i) = ATTRMAX(i);
        if (AMAX(i) < ATTRMIN(i))
            AMAX(i) = ATTRMIN(i);
        ABASE(i) = ABASE(i) * AMAX(i) / tmp;
        /* ABASE(i) > ATTRMAX(i) is impossible */
        if (ABASE(i) < ATTRMIN(i))
            ABASE(i) = ATTRMIN(i);
    }
    /* (void) encumber_msg(); -- caller needs to do this */
}

/* apply minor variation to attributes */
void
vary_init_attr(void)
{
    int i;

    for (i = 0; i < A_MAX; i++)
        if (!rn2(20)) {
            int xd = rn2(7) - 2; /* biased variation */

            (void) adjattrib(i, xd, TRUE);
            if (ABASE(i) < AMAX(i))
                AMAX(i) = ABASE(i);
        }
}

staticfn
void
postadjabil(long *ability)
{
    if (!ability)
        return;
    if (ability == &(HWarning) || ability == &(HSee_invisible))
        see_monsters();
}

staticfn const struct innate *
role_abil(int r)
{
    const struct {
        short role;
        const struct innate *abil;
    } roleabils[] = {
        { PM_ARCHEOLOGIST, arc_abil },
        { PM_BARBARIAN, bar_abil },
	    { PM_CARTOMANCER, car_abil },
        { PM_CAVE_DWELLER, cav_abil },
        { PM_HEALER, hea_abil },
        { PM_KNIGHT, kni_abil },
        { PM_MONK, mon_abil },
        { PM_CLERIC, pri_abil },
        { PM_RANGER, ran_abil },
        { PM_ROGUE, rog_abil },
        { PM_SAMURAI, sam_abil },
        { PM_TOURIST, tou_abil },
        { PM_UNDEAD_SLAYER, und_abil },
        { PM_VALKYRIE, val_abil },
        { PM_WIZARD, wiz_abil },
        { 0, 0 }
    };
    int i;

    for (i = 0; roleabils[i].abil && roleabils[i].role != r; i++)
        continue;
    return roleabils[i].abil;
}

staticfn const struct innate *
check_innate_abil(long *ability, long frommask)
{
    const struct innate *abil = 0;

    if (frommask == FROMEXPER)
        abil = role_abil(Role_switch);
    else if (frommask == FROMRACE)
        switch (Race_switch) {
        case PM_DWARF:
            abil = dwa_abil;
            break;
        case PM_ELF:
            abil = elf_abil;
            break;
        case PM_GNOME:
            abil = gno_abil;
            break;
        case PM_ORC:
            abil = orc_abil;
            break;
        case PM_HUMAN:
            abil = hum_abil;
            break;
        case PM_DHAMPIR:
            abil = vam_abil;
            break;
        case PM_GRUNG:
            abil = gru_abil;
            break;
        default:
            break;
        }

    while (abil && abil->ability) {
        if ((abil->ability == ability) && (u.ulevel >= abil->ulevel))
            return abil;
        abil++;
    }
    return (struct innate *) 0;
}

/* reasons for innate ability */
#define FROM_NONE 0
#define FROM_ROLE 1 /* from experience at level 1 */
#define FROM_RACE 2
#define FROM_INTR 3 /* intrinsically (eating some corpse or prayer reward) */
#define FROM_EXP  4 /* from experience for some level > 1 */
#define FROM_FORM 5
#define FROM_LYCN 6

/* check whether particular ability has been obtained via innate attribute */
staticfn int
innately(long *ability)
{
    const struct innate *iptr;

    if ((iptr = check_innate_abil(ability, FROMEXPER)) != 0)
        return (iptr->ulevel == 1) ? FROM_ROLE : FROM_EXP;
    if ((iptr = check_innate_abil(ability, FROMRACE)) != 0)
        return FROM_RACE;
    if ((*ability & FROMOUTSIDE) != 0L)
        return FROM_INTR;
    if ((*ability & FROMFORM) != 0L)
        return FROM_FORM;
    return FROM_NONE;
}

int
is_innate(int propidx)
{
    int innateness;

    /* innately() would report FROM_FORM for this; caller wants specificity */
    if (propidx == DRAIN_RES && ismnum(u.ulycn))
        return FROM_LYCN;
    if (propidx == FAST && Very_fast)
        return FROM_NONE; /* can't become very fast innately */
    if ((innateness = innately(&u.uprops[propidx].intrinsic)) != FROM_NONE)
        return innateness;
    if (propidx == JUMPING && Role_if(PM_KNIGHT)
        /* knight has intrinsic jumping, but extrinsic is more versatile so
           ignore innateness if equipment is going to claim responsibility */
        && !u.uprops[propidx].extrinsic)
        return FROM_ROLE;
    if ((propidx == BLINDED && !haseyes(gy.youmonst.data))
        || (propidx == BLND_RES && (HBlnd_resist & FROMFORM) != 0))
        return FROM_FORM;
    return FROM_NONE;
}

DISABLE_WARNING_FORMAT_NONLITERAL

char *
from_what(
    int propidx) /* special cases can have negative values */
{
    static char buf[BUFSZ];

    buf[0] = '\0';
    /*
     * Restrict the source of the attributes just to debug mode for now
     */
    if (wizard) {
        static NEARDATA const char because_of[] = " because of %s";

        if (propidx >= 0) {
            char *p;
            struct obj *obj = (struct obj *) 0;
            int innateness = is_innate(propidx);

            /*
             * Properties can be obtained from multiple sources and we
             * try to pick the most significant one.  Classification
             * priority is not set in stone; current precedence is:
             * "from the start" (from role or race at level 1),
             * "from outside" (eating corpse, divine reward, blessed potion),
             * "from experience" (from role or race at level 2+),
             * "from current form" (while polymorphed),
             * "from timed effect" (potion or spell),
             * "from worn/wielded equipment" (Firebrand, elven boots, &c),
             * "from carried equipment" (mainly quest artifacts).
             * There are exceptions.  Versatile jumping from spell or boots
             * takes priority over knight's innate but limited jumping.
             */
            if ((propidx == BLINDED && u.uroleplay.blind)
                || (propidx == DEAF && u.uroleplay.deaf))
                Sprintf(buf, " from birth");
            else if (innateness == FROM_ROLE || innateness == FROM_RACE)
                Strcpy(buf, " innately");
            else if (innateness == FROM_INTR) /* [].intrinsic & FROMOUTSIDE */
                Strcpy(buf, " intrinsically");
            else if (innateness == FROM_EXP)
                Strcpy(buf, " because of your experience");
            else if (innateness == FROM_LYCN)
                Strcpy(buf, " due to your lycanthropy");
            else if (innateness == FROM_FORM)
                Strcpy(buf, " from your creature form");
            else if (propidx == FAST && Very_fast)
                Sprintf(buf, because_of,
                        ((HFast & TIMEOUT) != 0L) ? "a potion or spell"
                          : ((EFast & W_ARMF) != 0L && uarmf->dknown
                             && objects[uarmf->otyp].oc_name_known)
                              ? ysimple_name(uarmf) /* speed boots */
                                : (u_wield_art(ART_QUICK_BLADE)
                                   || u_offhand_art(ART_QUICK_BLADE))
                                    ? "Quick Blade"
                                    : EFast
                                        ? "worn equipment"
                                        : something);
            else if ((obj = what_gives(&u.uprops[propidx].extrinsic)) != 0)
                Sprintf(buf, because_of, obj->oartifact
                                             ? bare_artifactname(obj)
                                             : ysimple_name(obj));
            else if (propidx == BLINDED && Blindfolded_only)
                Sprintf(buf, because_of, ysimple_name(ublindf));
            else if (propidx == BLINDED && u.ucreamed
                     && BlindedTimeout == (long) u.ucreamed
                     && !EBlinded && !(HBlinded & ~TIMEOUT))
                Sprintf(buf, " due to goop covering your %s",
                        body_part(FACE));
            else if (propidx == FUMBLING && (HFumbling & I_SPECIAL))
                Sprintf(buf, " due to goop covering your %s",
                        makeplural(body_part(FOOT)));

            /* remove some verbosity and/or redundancy */
            if ((p = strstri(buf, " pair of ")) != 0)
                copynchars(p + 1, p + 9, BUFSZ); /* overlapping buffers ok */
            else if (propidx == STRANGLED
                     && (p = strstri(buf, " of strangulation")) != 0)
                *p = '\0';

        } else { /* negative property index */
            /* if more blocking capabilities get implemented we'll need to
               replace this with what_blocks() comparable to what_gives() */
            switch (-propidx) {
            case BLINDED:
                /* wearing the Eyes of the Overworld overrides blindness */
                if (BBlinded && is_art(ublindf, ART_EYES_OF_THE_OVERWORLD))
                    Sprintf(buf, because_of, bare_artifactname(ublindf));
                break;
            case INVIS:
                if (u.uprops[INVIS].blocked & W_ARMC)
                    Sprintf(buf, because_of,
                            ysimple_name(uarmc)); /* mummy wrapping */
                break;
            case CLAIRVOYANT:
                if (wizard && (u.uprops[CLAIRVOYANT].blocked & W_ARMH))
                    Sprintf(buf, because_of,
                            ysimple_name(uarmh)); /* cornuthaum */
                break;
            }
        }

    } /*wizard*/
    return buf;
}

RESTORE_WARNING_FORMAT_NONLITERAL

void
adjabil(int oldlevel, int newlevel)
{
    const struct innate *abil, *rabil;
    long prevabil, mask = FROMEXPER;

    abil = role_abil(Role_switch);

    switch (Race_switch) {
    case PM_ELF:
        rabil = elf_abil;
        break;
    case PM_ORC:
        rabil = orc_abil;
        break;
    case PM_DHAMPIR:
        rabil = vam_abil;
        break;
    case PM_GRUNG:
        rabil = gru_abil;
        break;
    case PM_HUMAN:
    case PM_DWARF:
    case PM_GNOME:
    default:
        rabil = 0;
        break;
    }

    while (abil || rabil) {
        /* Have we finished with the intrinsics list? */
        if (!abil || !abil->ability) {
            /* Try the race intrinsics */
            if (!rabil || !rabil->ability)
                break;
            abil = rabil;
            rabil = 0;
            mask = FROMRACE;
        }
        prevabil = *(abil->ability);
        if (oldlevel < abil->ulevel && newlevel >= abil->ulevel) {
            /* Abilities gained at level 1 can never be lost
             * via level loss, only via means that remove _any_
             * sort of ability.  A "gain" of such an ability from
             * an outside source is devoid of meaning, so we set
             * FROMOUTSIDE to avoid such gains.
             */
            if (abil->ulevel == 1)
                *(abil->ability) |= (mask | FROMOUTSIDE);
            else {
                *(abil->ability) |= mask;
                if (!(*(abil->ability) & INTRINSIC & ~HAVEPARTIAL & ~mask)
                    && (!(*(abil->ability) & HAVEPARTIAL & ~mask)
                        || (*(abil->ability) & TIMEOUT) < 100)) {
                    if (*(abil->gainstr))
                        You_feel("%s!", abil->gainstr);
                }
            }
        } else if (oldlevel >= abil->ulevel && newlevel < abil->ulevel) {
            *(abil->ability) &= ~mask;
            if (!(*(abil->ability) & INTRINSIC & ~HAVEPARTIAL)
                && (!(*(abil->ability) & HAVEPARTIAL)
                    || (*(abil->ability) & TIMEOUT) < 100)) {
                if (*(abil->losestr))
                    You_feel("%s!", abil->losestr);
                else if (*(abil->gainstr))
                    You_feel("less %s!", abil->gainstr);
            }
        }
        if (prevabil != *(abil->ability)) /* it changed */
            postadjabil(abil->ability);
        abil++;
    }

    if (oldlevel > 0) {
        if (newlevel > oldlevel)
            add_weapon_skill(newlevel - oldlevel);
        else
            lose_weapon_skill(oldlevel - newlevel);
    }
    /* Inform cartomancers of their combo ability */
    if (Role_if(PM_CARTOMANCER)) {
        if (oldlevel < 5 && newlevel >= 5) {
            if (newlevel > oldlevel)
                Your("powers grow! (Use Z to play multiple card combos!)");
            else
                Your("powers diminish!");
        }
    }

    /* ALI -- update Warn_of_mon */
    HWarn_of_mon = HUndead_warning;
    if (HUndead_warning)
        svc.context.warntype.intrins |= MH_UNDEAD;
    else
        svc.context.warntype.intrins &= ~MH_UNDEAD;
}

/* called when gaining a level (before u.ulevel gets incremented);
   also called with u.ulevel==0 during hero initialization or for
   re-init if hero turns into a "new man/woman/elf/&c" */
int
newhp(void)
{
    int hp, conplus, tempnum;

    if (u.ulevel == 0) {
        /* Initialize hit points */
        hp = gu.urole.hpadv.infix + gu.urace.hpadv.infix;
        if (gu.urole.hpadv.inrnd > 0)
            hp += rnd(gu.urole.hpadv.inrnd);
        if (gu.urace.hpadv.inrnd > 0)
            hp += rnd(gu.urace.hpadv.inrnd);
        if (svm.moves == 0) { /* initial hero; skip for polyself to new man */
            /* Initialize alignment stuff */
            u.ualign.type = aligns[flags.initalign].value;
            u.ualign.record = gu.urole.initrecord;
        }
        /* no Con adjustment for initial hit points */
    } else {
        if (u.ulevel < gu.urole.xlev) {
            hp = gu.urole.hpadv.lofix + gu.urace.hpadv.lofix;
            if (gu.urole.hpadv.lornd > 0)
                hp += rnd(gu.urole.hpadv.lornd);
            if (gu.urace.hpadv.lornd > 0)
                hp += rnd(gu.urace.hpadv.lornd);
        } else {
            hp = gu.urole.hpadv.hifix + gu.urace.hpadv.hifix;
            if (gu.urole.hpadv.hirnd > 0)
                hp += rnd(gu.urole.hpadv.hirnd);
            if (gu.urace.hpadv.hirnd > 0)
                hp += rnd(gu.urace.hpadv.hirnd);
        }

        /* Cavemen get a tiny HP boost if they've remained illiterate,
         * as well as a tiny wisdom boost.  The longer they remain
         * illiterate, the bigger the HP boost gets (capped at d5)
         */
        if (Role_if(PM_CAVE_DWELLER)) {
            tempnum = 0;
            if (u.uconduct.literate < 1) {
                if (u.ulevel < 3) {
                    /* 1st rank */
                    tempnum += rn1(3, 2);
                } else if (u.ulevel < 10) {
                    /* 2nd and 3rd ranks */
                    tempnum += rn1(4, 3);
                } else if (u.ulevel < 18) {
                    /* 4th and 5th ranks */
                    tempnum += rn1(6, 3);
                } else {
                    /* 6th through 9th ranks */
                    tempnum += rn1(8, 5);
                }
                exercise(A_WIS, TRUE);
            }
            hp += tempnum;
        }

        if (ACURR(A_CON) <= 3)
            conplus = -2;
        else if (ACURR(A_CON) <= 6)
            conplus = -1;
        else if (ACURR(A_CON) <= 14)
            conplus = 0;
        else if (ACURR(A_CON) <= 16)
            conplus = 1;
        else if (ACURR(A_CON) == 17)
            conplus = 2;
        else if (ACURR(A_CON) == 18)
            conplus = 3;
        else
            conplus = 4;
        hp += conplus;
    }
    if (hp <= 0)
        hp = 1;
    if (u.ulevel < MAXULEV) {
        /* remember increment; future level drain could take it away again */
        u.uhpinc[u.ulevel] = (xint16) hp;
    } else {
        /* after level 30, throttle hit point gains from extra experience;
           once max reaches 1200, further increments will be just 1 more */
        char lim = 5 - u.uhpmax / 300;

        lim = max(lim, 1);
        if (hp > lim)
            hp = lim;
    }
    return hp;
}

/* minimum value for uhpmax is ulevel but for life-saving it is always at
   least 10 if ulevel is less than that */
int
minuhpmax(int altmin)
{
    if (altmin < 1)
        altmin = 1;
    return max(u.ulevel, altmin);
}

/* update u.uhpmax or u.mhmax and values of other things that depend upon
   whichever of them is relevant */
void
setuhpmax(int newmax, boolean even_when_polyd)
{
    if (!Upolyd || even_when_polyd) {
        if (newmax != u.uhpmax) {
            u.uhpmax = newmax;
            if (u.uhpmax > u.uhppeak)
                u.uhppeak = u.uhpmax;
            disp.botl = TRUE;
        }
        if (u.uhp > u.uhpmax)
            u.uhp = u.uhpmax, disp.botl = TRUE;
    } else { /* Upolyd */
        if (newmax != u.mhmax) {
            u.mhmax = newmax;
            disp.botl = TRUE;
        }
        if (u.mh > u.mhmax)
            u.mh = u.mhmax, disp.botl = TRUE;
    }
}

/* called after setuhpmax() when damage is pending;
   if uhpmax (or mhmax) has been reduced, it might have caused uhp (or mh)
   to be reduced too; if so, recalculate pending loss to account for that */
int
adjuhploss(
    int loss, /* pending hp loss */
    int olduhp) /* does double duty as oldmh when Upolyd */
{
    if (!Upolyd) {
        if (u.uhp < olduhp)
            loss -= (olduhp - u.uhp);
    } else {
        if (u.mh < olduhp)
            loss -= (olduhp - u.mh);
    }
    return max(loss, 1);
}

/* return the current effective value of a specific characteristic
   (the 'a' in 'acurr()' comes from outdated use of "attribute" for the
   six Str/Dex/&c characteristics; likewise for u.abon, u.atemp, u.acurr) */
schar
acurr(int chridx)
{
    int tmp, result = 0; /* 'result' will always be reset to positive value */

    assert(chridx >= 0 && chridx < A_MAX);
    tmp = u.abon.a[chridx] + u.atemp.a[chridx] + u.acurr.a[chridx];

    /* for Strength:  3 <= result <= 125;
       for all others:  3 <= result <= 25 */
    if (chridx == A_STR) {
        if (Race_if(PM_ORC)) {
            tmp += u.ulevel / 3;
            tmp = (tmp > 18) ? STR19(tmp) : tmp;
        }
        /* strength value is encoded:  3..18 normal, 19..118 for 18/xx (with
           1 <= xx <= 100), and 119..125 for other characteristics' 19..25;
           STR18(x) yields 18 + x (intended for 0 <= x <= 100; not used here);
           STR19(y) yields 100 + y (intended for 19 <= y <= 25) */
        if (tmp >= STR19(25) || (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER))
            result = STR19(25); /* 125 */
        else if (u_wield_art(ART_GIANTSLAYER) || u_offhand_art(ART_GIANTSLAYER))
            result = (tmp < 118 ? 118 : tmp);
        else
            /* need non-zero here to avoid 'if(result==0)' below because
               that doesn't deal with Str encoding; the cap of 25 applied
               there would limit Str to 18/07 [18 + 7] */
            result = max(tmp, 3);
    } else if (chridx == A_CHA) {
        if (tmp < 18 && (gy.youmonst.data->mlet == S_NYMPH
                         || u.umonnum == PM_AMOROUS_DEMON))
            result = 18;
        else if (Role_if(PM_CARTOMANCER)
            && uwep && uwep->otyp == CRYSTAL_BALL)
            result = 25;
    } else if (chridx == A_CON) {
        if (u_wield_art(ART_OGRESMASHER) || u_offhand_art(ART_OGRESMASHER))
            result = 25;
        else if (Race_if(PM_ORC)) {
            tmp += u.ulevel / 3;
            result = (tmp > 25) ? 25 : tmp;
        }
    } else if (chridx == A_INT || chridx == A_WIS) {
        /* Yes, this may raise Int and/or Wis if hero is sufficiently
           stupid.  There are lower levels of cognition than "dunce". */
        if (uarmh && uarmh->otyp == DUNCE_CAP)
            result = 6;
    } else if (chridx == A_DEX) {
        if (Race_if(PM_ORC)) {
            tmp += u.ulevel / 3;
            result = (tmp > 25) ? 25 : tmp;
        }
    }

    if (result == 0) /* none of the special cases applied */
        result = (tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp;

    return (schar) result;
}

/* condense clumsy ACURR(A_STR) value into value that fits into formulas */
schar
acurrstr(void)
{
    int str = ACURR(A_STR), /* 3..125 after massaging by acurr() */
        result; /* 3..25 */

    if (str <= STR18(0)) /* <= 18; max(,3) here is redundant */
        result = max(str, 3); /* 3..18 */
    else if (str <= STR19(21)) /* <= 121 */
        /* this converts
           18/01..18/31 into 19,
           18/32..18/81 into 20,
           18/82..18/100 and 19..21 into 21 */
        result = 19 + str / 50; /* map to 19..21 */
    else /* convert 122..125; min(,125) here is redundant */
        result = min(str, 125) - 100; /* 22..25 */

    return (schar) result;
}

/* when wearing (or taking off) an unID'd item, this routine is used
   to distinguish between observable +0 result and no-visible-effect
   due to an attribute not being able to exceed maximum or minimum */
boolean
extremeattr(
    int attrindx) /* does attrindx's value match its max or min? */
{
    /* Fixed_abil and racial MINATTR/MAXATTR aren't relevant here */
    int lolimit = 3, hilimit = 25, curval = ACURR(attrindx);

    /* upper limit for Str is 25 but its value is encoded differently */
    if (attrindx == A_STR) {
        hilimit = STR19(25); /* 125 */
        /* lower limit for Str can also be 25 */
        if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER)
            lolimit = hilimit;
        else if (u_wield_art(ART_GIANTSLAYER) || u_offhand_art(ART_GIANTSLAYER))
            lolimit = hilimit;
    } else if (attrindx == A_CON) {
        if (u_wield_art(ART_OGRESMASHER) || u_offhand_art(ART_OGRESMASHER))
            lolimit = hilimit;
    } else if (attrindx == A_CHA) {
         if (Role_if(PM_CARTOMANCER)
            && uwep && uwep->otyp == CRYSTAL_BALL)
            lolimit = hilimit;
    }
    /* this exception is hypothetical; the only other worn item affecting
       Int or Wis is another helmet so can't be in use at the same time */
    if (attrindx == A_INT || attrindx == A_WIS) {
        if (uarmh && uarmh->otyp == DUNCE_CAP)
            hilimit = lolimit = 6;
    }

    /* are we currently at either limit? */
    return (curval == lolimit || curval == hilimit) ? TRUE : FALSE;
}

/* avoid possible problems with alignment overflow, and provide a centralized
   location for any future alignment limits */
void
adjalign(int n)
{
    int newalign = u.ualign.record + n;

    if (n < 0) {
        unsigned newabuse = u.ualign.abuse - n;

        if (newalign < u.ualign.record)
            u.ualign.record = newalign;
        if (newabuse > u.ualign.abuse) {
            u.ualign.abuse = newabuse;
            adj_erinys(newabuse);
        }
    } else if (newalign > u.ualign.record) {
        u.ualign.record = newalign;
        if (u.ualign.record > ALIGNLIM)
            u.ualign.record = (int)ALIGNLIM;
    }
}

/* change hero's alignment type, possibly losing use of artifacts */
void
uchangealign(
    int newalign,
    int reason) /* A_CG_CONVERT, A_CG_HELM_ON, or A_CG_HELM_OFF */
{
    aligntyp oldalign = u.ualign.type;

    u.ublessed = 0; /* lose divine protection */
    /* You/Your/pline message with call flush_screen(), triggering bot(),
       so the actual data change needs to come before the message */
    disp.botl = TRUE; /* status line needs updating */
    if (reason == A_CG_CONVERT) {
        /* conversion via altar */
        livelog_printf(LL_ALIGNMENT, "permanently converted to %s",
                       aligns[1 - newalign].adj);
        u.ualignbase[A_CURRENT] = (aligntyp) newalign;
        /* worn helm of opposite alignment might block change */
        if (!uarmh || uarmh->otyp != HELM_OF_OPPOSITE_ALIGNMENT)
            u.ualign.type = u.ualignbase[A_CURRENT];
        You("have a %ssense of a new direction.",
            (u.ualign.type != oldalign) ? "sudden " : "");
    } else {
        /* putting on or taking off a helm of opposite alignment */
        u.ualign.type = (aligntyp) newalign;
        if (reason == A_CG_HELM_ON) {
            adjalign(-7); /* for abuse -- record will be cleared shortly */
            Your("mind oscillates %s.", Hallucination ? "wildly" : "briefly");
            make_confused(rn1(2, 3), FALSE);
            if (Is_astralevel(&u.uz) || ((unsigned) rn2(50) < u.ualign.abuse))
                summon_furies(Is_astralevel(&u.uz) ? 0 : 1);
            /* don't livelog taking it back off */
            livelog_printf(LL_ALIGNMENT, "used a helm to turn %s",
                           aligns[1 - newalign].adj);
        } else if (reason == A_CG_HELM_OFF) {
            Your("mind is %s.", Hallucination
                                    ? "much of a muchness"
                                    : "back in sync with your body");
        }
    }
    if (u.ualign.type != oldalign) {
        u.ualign.record = 0; /* slate is wiped clean */
        retouch_equipment(0);
    }
}

/** Returns the hitpoints of your current form. */
int
uhp(void)
{
    return (Upolyd ? u.mh : u.uhp);
}

/** Returns the maximal hitpoints of your current form. */
int
uhpmax(void)
{
    return (Upolyd ? u.mhmax : u.uhpmax);
}

/*attrib.c*/
