/* NetHack 3.7	read.c	$NHDT-Date: 1715889745 2024/05/16 20:02:25 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.308 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2012. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#define Your_Own_Role(mndx)  ((mndx) == gu.urole.mnum)
#define Your_Own_Race(mndx)  ((mndx) == gu.urace.mnum)

staticfn boolean learnscrolltyp(short);
staticfn void cap_spe(struct obj *);
staticfn char *erode_obj_text(struct obj *, char *);
staticfn char *hawaiian_design(struct obj *, char *);
staticfn int read_ok(struct obj *);
staticfn void stripspe(struct obj *);
staticfn void p_glow1(struct obj *);
staticfn void p_glow2(struct obj *, const char *);
staticfn void flood_space(coordxy, coordxy, genericptr);
staticfn void unflood_space(coordxy, coordxy, genericptr);
staticfn boolean can_center_cloud(coordxy, coordxy);
staticfn void display_stinking_cloud_positions(boolean);
staticfn void seffect_enchant_armor(struct obj **);
staticfn void seffect_destroy_armor(struct obj **);
staticfn void seffect_confuse_monster(struct obj **);
staticfn void seffect_scare_monster(struct obj **);
staticfn void seffect_remove_curse(struct obj **);
staticfn boolean seffect_create_monster(struct obj **);
staticfn boolean seffect_zapping(struct obj **);
staticfn void seffect_enchant_weapon(struct obj **);
staticfn int wep_enchant_range(int);
staticfn void seffect_taming(struct obj **);
staticfn void seffect_genocide(struct obj **);
staticfn void seffect_light(struct obj **);
staticfn void seffect_cloning(struct obj **);
staticfn void seffect_charging(struct obj **);
staticfn void seffect_amnesia(struct obj **);
staticfn void seffect_fire(struct obj **);
staticfn void seffect_earth(struct obj **);
staticfn void seffect_punishment(struct obj **);
staticfn void seffect_stinking_cloud(struct obj **);
staticfn void seffect_blank_paper(struct obj **);
staticfn void seffect_teleportation(struct obj **);
staticfn void seffect_gold_detection(struct obj **);
staticfn void seffect_food_detection(struct obj **);
staticfn void seffect_identify(struct obj **);
staticfn void seffect_knowledge(struct obj **);
staticfn void seffect_magic_mapping(struct obj **);
#ifdef MAIL_STRUCTURES
staticfn void seffect_mail(struct obj **);
#endif /* MAIL_STRUCTURES */
staticfn void set_lit(coordxy, coordxy, genericptr);
staticfn void do_stinking_cloud(struct obj *, boolean);
staticfn boolean create_particular_parse(char *,
                                       struct _create_particular_data *);
staticfn boolean create_particular_creation(struct _create_particular_data *);
staticfn void specified_id(void);

staticfn boolean
learnscrolltyp(short scrolltyp)
{
    if (!objects[scrolltyp].oc_name_known) {
        makeknown(scrolltyp);
        more_experienced(0, 10);
        return TRUE;
    } else
        return FALSE;
}

/* also called from teleport.c for scroll of teleportation */
void
learnscroll(struct obj *sobj)
{
    /* it's implied that sobj->dknown is set;
       we couldn't be reading this scroll otherwise */
    if (sobj->oclass != SPBOOK_CLASS)
        (void) learnscrolltyp(sobj->otyp);
}

/* max spe is +99, min is -99 */
staticfn void
cap_spe(struct obj *obj)
{
    if (obj) {
        if (abs(obj->spe) > SPE_LIM)
            obj->spe = sgn(obj->spe) * SPE_LIM;
    }
}

staticfn char *
erode_obj_text(struct obj *otmp, char *buf)
{
    int erosion = greatest_erosion(otmp);

    if (erosion)
        wipeout_text(buf, (int) (strlen(buf) * erosion / (2 * MAX_ERODE)),
                     otmp->o_id ^ (unsigned) ubirthday);
    return buf;
}

char *
tshirt_text(struct obj *tshirt, char *buf)
{
    static const char *const shirt_msgs[] = {
        /* Scott Bigham */
      "I explored the Dungeons of Doom and all I got was this lousy T-shirt!",
        "Is that Mjollnir in your pocket or are you just happy to see me?",
      "It's not the size of your sword, it's how #enhance'd you are with it.",
        "Madame Elvira's House O' Succubi Lifetime Customer",
        "Madame Elvira's House O' Succubi Employee of the Month",
        "Ludios Vault Guards Do It In Small, Dark Rooms",
        "Yendor Military Soldiers Do It In Large Groups",
        "I survived NerfHack Military Boot Camp",
        "Ludios Accounting School Intra-Mural Lacrosse Team",
        "Oracle(TM) Fountains 10th Annual Wet T-Shirt Contest",
        "Hey, black dragon!  Disintegrate THIS!",
        "I'm With Stupid -->",
        "Don't blame me, I voted for Izchak!",
        "Don't Panic", /* HHGTTG */
        "Furinkan High School Athletic Dept.",                /* Ranma 1/2 */
        "Hel-LOOO, Nurse!",                                   /* Animaniacs */
        "=^.^=",
        "100% goblin hair - do not wash",
        "Aberzombie and Fitch",
        "cK -- Cockatrice touches the Kop",
        "Don't ask me, I only adventure here",
        "Down with pants!",
        "d, your dog or a killer?",
        "FREE PUG AND NEWT!",
        "Go team ant!",
        "Got newt?",
        "Hello, my darlings!", /* Charlie Drake */
        "Hey!  Nymphs!  Steal This T-Shirt!",
        "I <3 Dungeon of Doom",
        "I <3 Maud",
        /* note: there is a similarly worded apron (alchemy smock) slogan */
        "I am a Valkyrie.  If you see me running, try to keep up.",
        "I am not a pack rat - I am a collector",
        "I bounced off a rubber tree",         /* Monkey Island */
        "Plunder Island Brimstone Beach Club", /* Monkey Island */
        "If you can read this, I can hit you with my polearm",
        "I'm confused!",
        "I scored with the princess",
        "I want to live forever or die in the attempt.",
        "Lichen Park",
        "LOST IN THOUGHT - please send search party",
        "Meat is Mordor",
        "Minetown Better Business Bureau",
        "Minetown Watch",
        /* Discworld riff; unfortunately long */
        ("Ms. Palm's House of Negotiable Affection--A Very Reputable"
            " House Of Disrepute"),
        "Protection Racketeer",
        "Real men love Crom",
        "Somebody stole my Mojo!",
        "The Hellhound Gang",
        "The Werewolves",
        "They Might Be Storm Giants",
        "Weapons don't kill people, I kill people",
        "White Zombie",
        "You're killing me!",
        "Anhur State University - Home of the Fighting Fire Ants!",
        "FREE HUGS",
        "Serial Ascender",
        "Real men are valkyries",
        "Young Men's Cavedigging Association",
        "Occupy Fort Ludios",
        "I couldn't afford this T-shirt so I stole it!",
        "Mind flayers suck",
        "I'm not wearing any pants",
        "Down with the living!",
        "Pudding farmer",
        "Vegetarian",
        "Hello, I'm War!",
        "It is better to light a candle than to curse the darkness",
        "It is easier to curse the darkness than to light a candle",
        /* expanded "rock--paper--scissors" featured in TV show "Big Bang
           Theory" although they didn't create it (and an actual T-shirt
           with pentagonal diagram showing which choices defeat which) */
        "rock--paper--scissors--lizard--Spock!",
        /* "All men must die -- all men must serve" challenge and response
           from book series _A_Song_of_Ice_and_Fire_ by George R.R. Martin,
           TV show "Game of Thrones" (probably an actual T-shirt too...) */
        "/Valar morghulis/ -- /Valar dohaeris/",
    };

    Strcpy(buf, shirt_msgs[tshirt->o_id % SIZE(shirt_msgs)]);
    return erode_obj_text(tshirt, buf);
}

char *
hawaiian_motif(struct obj *shirt, char *buf)
{
    static const char *const hawaiian_motifs[] = {
        /* birds */
        "flamingo",
        "parrot",
        "toucan",
        "bird of paradise", /* could be a bird or a flower */
        /* sea creatures */
        "sea turtle",
        "tropical fish",
        "jellyfish",
        "giant eel",
        "water nymph",
        /* plants */
        "plumeria",
        "orchid",
        "hibiscus flower",
        "palm tree",
        /* other */
        "hula dancer",
        "sailboat",
        "ukulele",
    };

    /* a tourist's starting shirt always has the same o_id; we need some
       additional randomness or else its design will never differ */
    unsigned motif = shirt->o_id ^ (unsigned) ubirthday;

    Strcpy(buf, hawaiian_motifs[motif % SIZE(hawaiian_motifs)]);
    return buf;
}

staticfn char *
hawaiian_design(struct obj *shirt, char *buf)
{
    static const char *const hawaiian_bgs[] = {
        /* solid colors */
        "purple",
        "yellow",
        "red",
        "blue",
        "orange",
        "black",
        "green",
        /* adjectives */
        "abstract",
        "geometric",
        "patterned",
        "naturalistic",
    };

    /* This hash method is slightly different than the one in hawaiian_motif;
       using the same formula in both cases may lead to some shirt combos
       never appearing, if the sizes of the two lists have common factors. */
    unsigned bg = shirt->o_id ^ (unsigned) ~ubirthday;

    Sprintf(buf, "%s on %s background",
            makeplural(hawaiian_motif(shirt, buf)),
            an(hawaiian_bgs[bg % SIZE(hawaiian_bgs)]));
    return buf;
}

char *
apron_text(struct obj *apron, char *buf)
{
    static const char *const apron_msgs[] = {
        "Kiss the cook",
        "I'm making SCIENCE!",
        "Don't mess with the chef",
        "Don't make me poison you",
        "Gehennom's Kitchen",
        "Rat: The other white meat",
        "If you can't stand the heat, get out of Gehennom!",
        "If we weren't meant to eat animals, why are they made out of meat?",
        "If you don't like the food, I'll stab you",
        /* In the movie "The Sum of All Fears", a Russian worker in a weapons
           facility wears a T-shirt that a translator says reads, "I am a
           bomb technician, if you see me running ... try to catch up."
           In nethack, the quote is far more suitable to an alchemy smock
           (particularly since so many of these others are about cooking)
           than a T-shirt and is paraphrased to simplify/shorten it.
           [later... turns out that this is already a T-shirt message:
            "I am a Valkyrie.  If you see me running, try to keep up."
           so this one has been revised a little:  added alchemist prefix,
           changed "keep up" to original source's "catch up"] */
        "I am an alchemist; if you see me running, try to catch up...",
    };

    Strcpy(buf, apron_msgs[apron->o_id % SIZE(apron_msgs)]);
    return erode_obj_text(apron, buf);
}

static const char *const candy_wrappers[] = {
    "",                         /* (none -- should never happen) */
    "Apollo",                   /* Lost */
    "Moon Crunchy",             /* South Park */
    "Snacky Cake",    "Chocolate Nuggie", "The Small Bar",
    "Crispy Yum Yum", "Nilla Crunchie",   "Berry Bar",
    "Choco Nummer",   "Om-nom", /* Cat Macro */
    "Fruity Oaty",              /* Serenity */
    "Wonka Bar",                /* Charlie and the Chocolate Factory */
};

/* return the text of a candy bar's wrapper */
const char *
candy_wrapper_text(struct obj *obj)
{
    /* modulo operation is just bullet proofing; 'spe' is already in range */
    return candy_wrappers[obj->spe % SIZE(candy_wrappers)];
}

/* assign a wrapper to a candy bar stack */
void
assign_candy_wrapper(struct obj *obj)
{
    if (obj->otyp == CANDY_BAR) {
        /* skips candy_wrappers[0] */
        obj->spe = 1 + rn2(SIZE(candy_wrappers) - 1);
    }
    return;
}

/* getobj callback for object to read */
staticfn int
read_ok(struct obj *obj)
{
    if (!obj)
        return GETOBJ_EXCLUDE;

    if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS)
        return GETOBJ_SUGGEST;

    return GETOBJ_DOWNPLAY;
}

/* getobj callback for object to enchant */
static int
enchant_ok(struct obj* obj)
{
    if (!obj)
        return GETOBJ_EXCLUDE;

    if (obj->oclass == ARMOR_CLASS)
        return GETOBJ_SUGGEST;

    return GETOBJ_DOWNPLAY;
}

DISABLE_WARNING_FORMAT_NONLITERAL

/* the #read command; read a scroll or spell book or various other things */
int
doread(void)
{
    static const char find_any_braille[] = "feel any Braille writing.";
    struct obj *scroll;
    boolean confused, nodisappear;
    boolean carto = Role_if(PM_CARTOMANCER);
    int otyp;


    /*
     * Reading while blind is allowed in most cases, including the
     * Book of the Dead but not regular spellbooks.  For scrolls, the
     * description has to have been seen or magically learned (so only
     * when scroll->dknown is true):  hero recites the label while
     * holding the unfurled scroll.  We deliberately don't require
     * free hands because that would cripple scroll of remove curse,
     * but we ought to be requiring hands or at least limbs.  The
     * recitation could be sub-vocal; actual speech isn't required.
     *
     * Reading while confused is allowed and can produce alternate
     * outcome.
     *
     * Reading while stunned is currently allowed but probably should
     * be prevented....
     */

    gk.known = FALSE;
    if (check_capacity((char *) 0))
        return ECMD_OK;

    scroll = getobj("read", read_ok, GETOBJ_PROMPT);
    if (!scroll)
        return ECMD_CANCEL;

    /* really low intelligence can prevent reading things.
       exceptions to this rule are scrolls of mail (server
       messages), hawaiian shirts (pattern, not text), and
       the book of the dead (must have to win) */
    if (ACURR(A_INT) < 6
#ifdef MAIL
        && scroll->otyp != SCR_MAIL
#endif
        && scroll->otyp != HAWAIIAN_SHIRT
        && scroll->otyp != SPE_BOOK_OF_THE_DEAD) {
        You_cant("seem to make out what all these %s on the %s mean.",
                 rn2(2) ? "weird scribbles" : "confusing scratches",
                 simple_typename(scroll->otyp));
        return 0;
    }

    otyp = scroll->otyp;
    scroll->pickup_prev = 0; /* no longer 'just picked up' */

    /* outrumor has its own blindness check */
    if (otyp == FORTUNE_COOKIE) {
        if (flags.verbose)
            You("break up the cookie and throw away the pieces.");
        outrumor(bcsign(scroll), BY_COOKIE);
        if (!Blind)
            if (!u.uconduct.literate++)
                livelog_printf(LL_CONDUCT,
                               "became literate by reading a fortune cookie");
        useup(scroll);
        return ECMD_TIME;
    } else if (otyp == T_SHIRT || otyp == ALCHEMY_SMOCK
               || otyp == HAWAIIAN_SHIRT) {
        char buf[BUFSZ], *mesg;
        const char *endpunct;

        if (Blind) {
            You_cant(find_any_braille);
            return ECMD_OK;
        }
        /* can't read shirt worn under suit (under cloak is ok though) */
        if ((otyp == T_SHIRT || otyp == HAWAIIAN_SHIRT) && uarm
            && scroll == uarmu) {
            pline("%s shirt is obscured by %s%s.",
                  scroll->unpaid ? "That" : "Your", shk_your(buf, uarm),
                  suit_simple_name(uarm));
            return ECMD_OK;
        }
        if (otyp == HAWAIIAN_SHIRT) {
            pline("%s features %s.", flags.verbose ? "The design" : "It",
                  hawaiian_design(scroll, buf));
            return ECMD_TIME;
        }
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT, "became literate by reading %s",
                           (scroll->otyp == T_SHIRT) ? "a T-shirt"
                           : "an apron");

        /* populate 'buf[]' */
        mesg = (otyp == T_SHIRT) ? tshirt_text(scroll, buf)
                                 : apron_text(scroll, buf);
        endpunct = "";
        if (flags.verbose) {
            int ln = (int) strlen(mesg);

            /* we will be displaying a sentence; need ending punctuation */
            if (ln > 0 && !strchr(".!?", mesg[ln - 1]))
                endpunct = ".";
            pline("It reads:");
        }
        pline("\"%s\"%s", mesg, endpunct);
        return ECMD_TIME;
    } else if ((otyp == DUNCE_CAP || otyp == CORNUTHAUM)
        /* note: "DUNCE" isn't directly connected to tourists but
           if everyone could read it, they would always be able to
           trivially distinguish between the two types of conical hat;
           limiting this to tourists is better than rejecting it */
               && Role_if(PM_TOURIST)) {
        /* another note: the misspelling, "wizzard", is correct;
           that's what is written on Rincewind's pointy hat from
           Pratchett's Discworld series, along with a lot of stars;
           rather than inked on or painted on, treat them as stitched
           or even separate pieces of fabric which have been attached
           (don't recall whether the books mention anything like that...) */
        const char *cap_text = (otyp == DUNCE_CAP) ? "DUNCE" : "WIZZARD";

        if (scroll->o_id % 3) {
            /* no need to vary this when blind; "on this ___" is important
               because it suggests that there might be something on others */
            You_cant("find anything to read on this %s.",
                     simpleonames(scroll));
            return ECMD_OK;
        }
        pline("%s on the %s.  It reads:  %s.",
              !Blind ? "There is writing" : "You feel lettering",
              simpleonames(scroll), cap_text);
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT, "became literate by reading %s",
                           (otyp == DUNCE_CAP) ? "a dunce cap"
                                               : "a cornuthaum");

        /* yet another note: despite the fact that player will recognize
           the object type, don't make it become a discovery for hero */
        trycall(scroll);
        return ECMD_TIME;
    } else if (otyp == CREDIT_CARD) {
        static const char *const card_msgs[] = {
            "Leprechaun Gold Tru$t - Shamrock Card",
            "Magic Memory Vault Charge Card",
            "Larn National Bank",                /* Larn */
            "First Bank of Omega",               /* Omega */
            "Bank of Zork - Frobozz Magic Card", /* Zork */
            "Ankh-Morpork Merchant's Guild Barter Card",
            "Ankh-Morpork Thieves' Guild Unlimited Transaction Card",
            "Ransmannsby Moneylenders Association",
            "Bank of Gehennom - 99% Interest Card",
            "Yendorian Express - Copper Card",
            "Yendorian Express - Silver Card",
            "Yendorian Express - Gold Card",
            "Yendorian Express - Mithril Card",
            "Yendorian Express - Platinum Card", /* must be last */
        };
	/* Takeoffs of banned or broken from tcgs. */
        static const char *tcg_msgs[] = {
            /* MTG */
            "Draw three cards, or force a monster to draw three cards.",
            "Turn any monster into an 3/3 newt.",
            "Opponent loses next turn.",
            /* YGO */
            "Select and control one hostile monster.",
            "Draw two cards from your deck.",
            /* Pkmn */
            "Birthday surprise!",
            /* HVL */
            "You gain 3 energy."
        };

        if (Blind) {
            You("feel the embossed numbers:");
        } else {
            if (flags.verbose)
                pline("It reads:");
            if (carto) {
                pline("\"%s\"",
                    scroll->oartifact
                    ? tcg_msgs[SIZE(tcg_msgs) - 1]
                    : tcg_msgs[scroll->o_id % (SIZE(tcg_msgs) - 1)]);
            } else {
                pline("\"%s\"",
                    scroll->oartifact
                    ? card_msgs[SIZE(card_msgs) - 1]
                    : card_msgs[scroll->o_id % (SIZE(card_msgs) - 1)]);
            }
        }

        /* Make a credit card number */
        pline("\"%d0%d %ld%d1 0%d%d0\"%s",
              (((int) scroll->o_id % 89) + 10),
              ((int) scroll->o_id % 4),
              ((((long) scroll->o_id * 499L) % 899999L) + 100000L),
              ((int) scroll->o_id % 10),
              (!((int) scroll->o_id % 3)),
              (((int) scroll->o_id * 7) % 10),
              (flags.verbose || Blind) ? "." : "");
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT,
                           "became literate by reading a credit card");

        return ECMD_TIME;
    } else if (otyp == CAN_OF_GREASE) {
        pline("This %s has no label.", singular(scroll, xname));
        return ECMD_OK;
    } else if (otyp == MAGIC_MARKER) {
        static const int red_mons[] = {
            PM_FIRE_ANT, PM_PYROLISK, PM_HELL_HOUND, PM_IMP,
            PM_LARGE_MIMIC, PM_LEOCROTTA, PM_SCORPION, PM_XAN,
            PM_GIANT_BAT, PM_WATER_MOCCASIN, PM_FLESH_GOLEM,
            PM_BARBED_DEVIL, PM_MARILITH, PM_PIRANHA
        };
        char buf[BUFSZ];
        struct permonst *pm = &mons[red_mons[scroll->o_id % SIZE(red_mons)]];

        if (Blind) {
            You_cant(find_any_braille);
            return ECMD_OK;
        }
        if (flags.verbose)
            pline("It reads:");
        Sprintf(buf, "%s", pmname(pm, NEUTRAL));
        pline("\"Magic Marker(TM) %s Red Ink Marker Pen.  Water Soluble.\"",
              upwords(buf));
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT,
                           "became literate by reading a magic marker");

        return ECMD_TIME;
    } else if (scroll->oclass == COIN_CLASS) {
        if (Blind)
            You("feel the embossed words:");
        else if (flags.verbose)
            You("read:");
        pline("\"1 Zorkmid.  857 GUE.  In Frobs We Trust.\"");
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT,
                           "became literate by reading a coin's engravings");

        return ECMD_TIME;
    } else if (is_art(scroll, ART_ORB_OF_FATE)) {
        if (Blind)
            You("feel the engraved signature:");
        else
            pline("It is signed:");
        pline("\"Odin.\"");
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT,
                   "became literate by reading the divine signature of Odin");

        return ECMD_TIME;
    } else if (otyp == CANDY_BAR) {
        const char *wrapper = candy_wrapper_text(scroll);

        if (Blind) {
            You_cant(find_any_braille);
            return ECMD_OK;
        }
        if (!*wrapper) {
            pline("The candy bar's wrapper is blank.");
            return ECMD_OK;
        }
        pline("The wrapper reads: \"%s\".", wrapper);
        if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT,
                           "became literate by reading a candy bar wrapper");

        return ECMD_TIME;
    } else if (scroll->oclass != SCROLL_CLASS
               && scroll->oclass != SPBOOK_CLASS) {
        pline(silly_thing_to, "read");
        return ECMD_OK;
    } else if (Blind && otyp != SPE_BOOK_OF_THE_DEAD
        /* Cartomancers are allowed to "read" rulebooks.
         * Assume they have braille. */
        && !Role_if(PM_CARTOMANCER)) {
        const char *what = 0;

        if (otyp == SPE_NOVEL)
            /* unseen novels are already distinguishable from unseen
               spellbooks so this isn't revealing any extra information */
            what = "words";
        else if (scroll->oclass == SPBOOK_CLASS)
            what = "mystic runes";
        else if (!scroll->dknown)
            what = "formula on the scroll";
        if (what) {
            pline("Being blind, you cannot read the %s.", what);
            return ECMD_OK;
        }
    }

    confused = (Confusion != 0);
#ifdef MAIL_STRUCTURES
    if (otyp == SCR_MAIL) {
        confused = FALSE; /* override */
        /* reading mail is a convenience for the player and takes
           place outside the game, so shouldn't affect gameplay;
           on the other hand, it starts by explicitly making the
           hero actively read something, which is pretty hard
           to simply ignore; as a compromise, if the player has
           maintained illiterate conduct so far, and this mail
           scroll didn't come from bones, ask for confirmation */
        if (!u.uconduct.literate) {
            if (!scroll->spe && y_n(
             "Reading mail will violate \"illiterate\" conduct.  Read anyway?"
                                   ) != 'y')
                return ECMD_OK;
        }
    }
#endif

    /* Actions required to win the game aren't counted towards conduct */
    /* Novel conduct is handled in read_tribute so exclude it too */
    if (otyp != SPE_BOOK_OF_THE_DEAD
        && otyp != SPE_NOVEL
        && otyp != SPE_BLANK_PAPER
        && otyp != SCR_BLANK_PAPER) {
        /* Cartomancers *play* cards, they don't read them. */
        if (carto) {
            if (scroll->oclass != SCROLL_CLASS && !u.uconduct.literate++)
                livelog_printf(LL_CONDUCT, "became literate by reading %s",
                            (scroll->oclass == SPBOOK_CLASS) ? "a rulebook"
                            : (scroll->oclass == SCROLL_CLASS) ? "a card"
                                : something);
        } else if (!u.uconduct.literate++)
            livelog_printf(LL_CONDUCT, "became literate by reading %s",
                           (scroll->oclass == SPBOOK_CLASS) ? "a book"
                           : (scroll->oclass == SCROLL_CLASS) ? "a scroll"
                             : something);
    }
    if (scroll->oclass == SPBOOK_CLASS) {
        if (Role_if(PM_CARTOMANCER)
                && otyp != SPE_NOVEL
                && otyp != SPE_BOOK_OF_THE_DEAD)
            return cast_from_book(scroll) ? ECMD_TIME : ECMD_OK;
        else
            return study_book(scroll) ? ECMD_TIME : ECMD_OK;
    }
    scroll->in_use = TRUE; /* scroll, not spellbook, now being read */
    if (otyp != SCR_BLANK_PAPER) {
        boolean silently = !can_chant(&gy.youmonst);

        /* a few scroll feedback messages describe something happening
           to the scroll itself, so avoid "it disappears" for those */
        nodisappear = (otyp == SCR_FIRE
                       || (otyp == SCR_REMOVE_CURSE && scroll->cursed)
                       || otyp == SCR_ZAPPING
                       || is_moncard(scroll));
        if (Blind)
            pline(nodisappear
                      ? "You %s the formula on the scroll."
                      : "As you %s the formula on it, the scroll disappears.",
                  silently ? "cogitate" : "pronounce");
        else
            pline(nodisappear ? "You read the scroll."
                              : "As you read the scroll, it disappears.");
        if (confused) {
            if (Hallucination)
                pline("Being so trippy, you screw up...");
            else {
                if (Role_if(PM_CARTOMANCER))
                    pline("Being confused, you %s the rules text...",
                        silently ? "misunderstand" : "misread");
                else
                    pline("Being confused, you %s the magic words...",
                        silently ? "misunderstand" : "mispronounce");
            }
        }
    }
    if (!seffects(scroll)) {
        if (!objects[otyp].oc_name_known) {
            if (gk.known)
                learnscroll(scroll);
            else
                trycall(scroll);
        }
        scroll->in_use = FALSE;
        if (otyp != SCR_BLANK_PAPER)
            useup(scroll);
    }
    return ECMD_TIME;
}

RESTORE_WARNING_FORMAT_NONLITERAL

staticfn void
stripspe(struct obj *obj)
{
    if (obj->blessed || obj->spe <= 0) {
        pline1(nothing_happens);
    } else {
        /* order matters: message, shop handling, actual transformation */
        pline("%s briefly.", Yobjnam2(obj, "vibrate"));
        costly_alteration(obj, COST_UNCHRG);
        obj->spe = 0;
        if (obj->otyp == OIL_LAMP || obj->otyp == BRASS_LANTERN)
            obj->age = 0;
    }
}

staticfn void
p_glow1(struct obj *otmp)
{
    pline("%s briefly.", Yobjnam2(otmp, Blind ? "vibrate" : "glow"));
}

staticfn void
p_glow2(struct obj *otmp, const char *color)
{
    pline("%s%s%s for a moment.", Yobjnam2(otmp, Blind ? "vibrate" : "glow"),
          Blind ? "" : " ", Blind ? "" : hcolor(color));
}

/* getobj callback for object to charge */
int
charge_ok(struct obj *obj)
{
    if (!obj)
        return GETOBJ_EXCLUDE;

    if (obj->oclass == WAND_CLASS)
        return GETOBJ_SUGGEST;

    if (obj->oclass == RING_CLASS && objects[obj->otyp].oc_charged
        && obj->dknown && objects[obj->otyp].oc_name_known)
        return GETOBJ_SUGGEST;

    if (is_weptool(obj)) /* specific check before general tools */
        return GETOBJ_EXCLUDE;

    if (obj->oclass == TOOL_CLASS) {
        /* suggest tools that aren't oc_charged but can still be recharged */
        if (obj->otyp == BRASS_LANTERN
            || (obj->otyp == OIL_LAMP)
            /* only list magic lamps if they are not identified yet */
            || (obj->otyp == MAGIC_LAMP
                && !objects[MAGIC_LAMP].oc_name_known)) {
            return GETOBJ_SUGGEST;
        }
        /* suggest chargeable tools only if discovered, to prevent leaking
           info (e.g. revealing if an unidentified 'flute' is magic or not) */
        if (objects[obj->otyp].oc_charged) {
            return (obj->dknown && objects[obj->otyp].oc_name_known)
                     ? GETOBJ_SUGGEST : GETOBJ_DOWNPLAY;
        }
        return GETOBJ_EXCLUDE;
    }
    /* why are weapons/armor considered charged anyway?
     * make them selectable even so for "feeling of loss" message */
    return GETOBJ_EXCLUDE_SELECTABLE;
}

/* recharge an object; curse_bless is -1 if the recharging implement
   was cursed, +1 if blessed, 0 otherwise. */
void
recharge(struct obj *obj, int curse_bless)
{
    int n;
    boolean is_cursed, is_blessed;

    is_cursed = curse_bless < 0;
    is_blessed = curse_bless > 0;

    if (obj->oclass == WAND_CLASS) {
        int lim = (obj->otyp == WAN_WISHING)
                      ? 3
                      : (objects[obj->otyp].oc_dir != NODIR) ? 8 : 15;

        /* undo any prior cancellation, even when is_cursed */
        if (obj->spe == -1)
            obj->spe = 0;

        /*
         * Recharging might cause wands to explode.
         *      v = number of previous recharges
         *            v = percentage chance to explode on this attempt
         *                    v = cumulative odds for exploding
         *      0 :   0       0
         *      1 :   0.29    0.29
         *      2 :   2.33    2.62
         *      3 :   7.87   10.28
         *      4 :  18.66   27.02
         *      5 :  36.44   53.62
         *      6 :  62.97   82.83
         *      7 : 100     100
         */
        n = (int) obj->recharged;
        if (n > 0 && (obj->otyp == WAN_WISHING
                      || (n * n * n > rn2(7 * 7 * 7)))) { /* recharge_limit */
            pline_The("%s suddenly detonates!", xname(obj));
            wand_explode(obj, rnd(lim), &gy.youmonst);
            return;
        }
        /* didn't explode, so increment the recharge count */
        obj->recharged = (unsigned) (n + 1);

        /* now handle the actual recharging */
        if (is_cursed) {
            stripspe(obj);
        } else {
            n = (lim == 3) ? 3 : rn1(5, lim + 1 - 5);
            if (!is_blessed)
                n = rnd(n);

            if (obj->spe < n)
                obj->spe = n;
            else
                obj->spe++;
            if (obj->otyp == WAN_WISHING && obj->spe > 3) {
                pline_The("%s suddenly detonates!", xname(obj));
                wand_explode(obj, 1, &gy.youmonst);
                return;
            }
            if (obj->spe >= lim)
                p_glow2(obj, NH_BLUE);
            else
                p_glow1(obj);
#if 0 /*[shop price doesn't vary by charge count]*/
            /* update shop bill to reflect new higher price */
            if (obj->unpaid)
                alter_cost(obj, 0L);
#endif
        }

    } else if ((obj->oclass == RING_CLASS && objects[obj->otyp].oc_charged)
            || obj->otyp == AMULET_OF_GUARDING) {
        /* charging does not affect ring's curse/bless status */
        int s = is_blessed ? rnd(3) : is_cursed ? -rnd(2) : 1;
        boolean isring = obj->oclass == RING_CLASS;
        boolean is_on = (obj == uleft || obj == uright || obj == uamul);

        /* destruction depends on current state, not adjustment */
        if (obj->spe > rn2(6) + 3 || (is_cursed && obj->spe <= -5)) {
            pline("%s momentarily, then %s!", Yobjnam2(obj, "pulsate"),
                  otense(obj, "explode"));
            if (is_on) {
                if (isring)
                    Ring_gone(obj);
                else
                    Amulet_off();
            }

            s = rnd(3 * abs(obj->spe)); /* amount of damage */
            useup(obj), obj = 0;
            losehp(Maybe_Half_Phys(s), "exploding jewelery", KILLED_BY_AN);
        } else {
            long mask = is_on ? (obj == uleft ? LEFT_RING
                                 : obj == uright ? RIGHT_RING : W_AMUL) : 0L;

            pline("%s spins %sclockwise for a moment.", Yname2(obj),
                  s < 0 ? "counter" : "");
            if (s < 0)
                costly_alteration(obj, COST_DECHNT);
            /* cause attributes and/or properties to be updated */
            if (is_on) {
                if (isring)
                    Ring_off(obj);
                else
                    Amulet_off();
            }
            obj->spe += s; /* update the accessory while it's off */
            if (is_on)
                setworn(obj, mask), isring ? Ring_on(obj) : Amulet_on(obj);
            /* oartifact: if a touch-sensitive artifact ring is
               ever created the above will need to be revised  */
            /* update shop bill to reflect new higher price */
            if (s > 0 && obj->unpaid)
                alter_cost(obj, 0L);
        }
    } else if (obj->oclass == TOOL_CLASS) {
        n = (int) obj->recharged;


        /* Use same recharging calculation as for wands. */
        if (obj->obroken) {
            pline_The("%s crumbles away!", xname(obj));
            useup(obj);
            return;
        }
        if (n > 0 && !obj->oartifact && obj->otyp != BELL_OF_OPENING
                && (n * n * n > rn2(7 * 7 * 7))) { /* recharge_limit */
            obj->obroken = 1;
            Your("%s suddenly vibrates!", xname(obj));
            return;
        }
        if (objects[obj->otyp].oc_charged) {
            /* tools don't have a limit, but the counter used does */
            if (n < 7) /* recharge_limit */
                obj->recharged++;
        }

        switch (obj->otyp) {
        case BELL_OF_OPENING:
            if (is_cursed)
                stripspe(obj);
            else if (is_blessed)
                obj->spe += rnd(3);
            else
                obj->spe += 1;
            if (obj->spe > 5)
                obj->spe = 5;
            break;
        case MAGIC_MARKER:
        case TINNING_KIT:
        case EXPENSIVE_CAMERA:
            if (is_cursed) {
                stripspe(obj);
            } else if (n && obj->otyp == MAGIC_MARKER) {
                /* previously recharged */
                obj->recharged = 1; /* override increment done above */
                if (obj->spe < 3)
                    Your("marker seems permanently dried out.");
                else
                    pline1(nothing_happens);
            } else if (is_blessed) {
                n = rn1(16, 15); /* 15..30 */
                if (obj->spe + n <= 50)
                    obj->spe = 50;
                else if (obj->spe + n <= 75)
                    obj->spe = 75;
                else {
                    int chrg = (int) obj->spe;
                    if ((chrg + n) > 127)
                        obj->spe = 127;
                    else
                        obj->spe += n;
                }
                p_glow2(obj, NH_BLUE);
            } else {
                n = rn1(11, 10); /* 10..20 */
                if (obj->spe + n <= 50)
                    obj->spe = 50;
                else {
                    int chrg = (int) obj->spe;

                    if (chrg + n > SPE_LIM)
                        obj->spe = SPE_LIM;
                    else
                        obj->spe += n;
                }
                p_glow2(obj, NH_WHITE);
            }
            break;
        case OIL_LAMP:
        case BRASS_LANTERN:
            if (is_cursed) {
                stripspe(obj);
                if (obj->lamplit) {
                    if (!Blind)
                        pline("%s out!", Tobjnam(obj, "go"));
                    end_burn(obj, TRUE);
                }
            } else if (is_blessed) {
                obj->spe = 1;
                obj->age = 1500;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe = 1;
                obj->age += 750;
                if (obj->age > 1500)
                    obj->age = 1500;
                p_glow1(obj);
            }
            break;
        case CRYSTAL_BALL:
            if (obj->spe == -1) /* like wands, first uncancel */
                obj->spe = 0;

            if (is_cursed) {
                /* cursed scroll removes charges and curses ball */
                /*stripspe(obj); -- doesn't do quite what we want...*/
                if (!obj->cursed) {
                    p_glow2(obj, NH_BLACK);
                    curse(obj);
                } else {
                    pline("%s briefly.", Yobjnam2(obj, "vibrate"));
                }
                if (obj->spe > 0)
                    costly_alteration(obj, COST_UNCHRG);
                obj->spe = 0;
            } else if (is_blessed) {
                /* blessed scroll sets charges to max and blesses ball */
                obj->spe = 7;
                p_glow2(obj, !obj->blessed ? NH_LIGHT_BLUE : NH_BLUE);
                if (!obj->blessed)
                    bless(obj);
                /* [shop price stays the same regardless of charges or BUC] */
            } else {
                /* uncursed scroll increments charges and uncurses ball */
                if (obj->spe < 7 || obj->cursed) {
                    n = rnd(2);
                    obj->spe = min(obj->spe + n, 7);
                    if (!obj->cursed) {
                        p_glow1(obj);
                    } else {
                        p_glow2(obj, NH_AMBER);
                        uncurse(obj);
                    }
                } else {
                    /* charges at max and ball not being uncursed */
                    pline1(nothing_happens);
                }
            }
            break;
        case HORN_OF_PLENTY:
        case BAG_OF_TRICKS:
        case CAN_OF_GREASE:
            if (is_cursed) {
                stripspe(obj);
            } else if (is_blessed) {
                if (obj->spe <= 10)
                    obj->spe += rn1(10, 6);
                else
                    obj->spe += rn1(5, 6);
                if (obj->spe > 50)
                    obj->spe = 50;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe += rn1(5, 2);
                if (obj->spe > 50)
                    obj->spe = 50;
                p_glow1(obj);
            }
            break;
        case MAGIC_FLUTE:
        case MAGIC_HARP:
        case FROST_HORN:
        case FIRE_HORN:
        case DRUM_OF_EARTHQUAKE:
            if (is_cursed) {
                stripspe(obj);
            } else if (is_blessed) {
                obj->spe += d(2, 4);
                if (obj->spe > 20)
                    obj->spe = 20;
                p_glow2(obj, NH_BLUE);
            } else {
                obj->spe += rnd(4);
                if (obj->spe > 20)
                    obj->spe = 20;
                p_glow1(obj);
            }
            break;
        default:
            goto not_chargable;
            /*NOTREACHED*/
            break;
        } /* switch */

    } else {
 not_chargable:
        You("have a feeling of loss.");
    }

    /* prevent enchantment from getting out of range */
    cap_spe(obj);
}

/*
 * Forget some things (e.g. after reading a scroll of amnesia).  When called,
 * the following are always forgotten:
 *      - felt ball & chain
 *      - skill training
 *
 * Other things are subject to flags:
 *      howmuch & ALL_SPELLS    = forget all spells
 */
void
forget(int howmuch)
{
    struct monst *mtmp;

    if (u_wield_art(ART_ORIGIN))
        return;

    if (Punished)
        u.bc_felt = 0; /* forget felt ball&chain */

    if (howmuch & ALL_SPELLS)
        losespells();

    /* Can lose telepathy to amnesia attacks */
    if (HTelepat & INTRINSIC && rn2(3)) {
        HTelepat &= ~INTRINSIC;
        if (Blind && !Blind_telepat)
            see_monsters(); /* Can't sense mons anymore! */
        Your("senses fail!");
    }

    /* Forget some skills. */
    drain_weapon_skill(rnd(howmuch ? 5 : 3));

    /* forget having seen monsts (affects recognizing unseen ones by sound) */
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
        if (mtmp != u.usteed && mtmp != u.ustuck)
            mtmp->meverseen = 0;
    /* [perhaps ought to forget having seen every monster on every level] */
    for (mtmp = gm.migrating_mons; mtmp; mtmp = mtmp->nmon)
        mtmp->meverseen = 0;
}

/* monster is hit by scroll of taming's effect */
int
maybe_tame(struct monst *mtmp, struct obj *sobj)
{
    int was_tame = mtmp->mtame;
    unsigned was_peaceful = mtmp->mpeaceful;

    if (sobj->cursed) {
        setmangry(mtmp, FALSE);
        if (was_peaceful && !mtmp->mpeaceful)
            return -1;
    } else {
        /* for a shopkeeper, tamedog() will call make_happy_shk() but
           not tame the target, so call it even if taming gets resisted */
        if (!resist(mtmp, sobj->oclass, 0, NOTELL) || mtmp->isshk)
            (void) tamedog(mtmp, sobj, FALSE);

        if ((!was_peaceful && mtmp->mpeaceful) || was_tame != mtmp->mtame)
            return 1;
    }
    return 0;
}

/* Can a stinking cloud physically exist at a certain position?
 * NOT the same thing as can_center_cloud.
 */
boolean
valid_cloud_pos(coordxy x, coordxy y)
{
    if (!isok(x,y))
        return FALSE;
    return ACCESSIBLE(levl[x][y].typ) || is_pool(x, y) || is_lava(x, y);
}

/* Callback for getpos_sethilite, also used in determining whether a scroll
 * should have its regular effects, or not because it was out of range.
 */
staticfn boolean
can_center_cloud(coordxy x, coordxy y)
{
    if (!valid_cloud_pos(x, y))
        return FALSE;
    return (cansee(x, y) && distu(x, y) < 32);
}

staticfn void
display_stinking_cloud_positions(boolean on_off)
{
    coordxy x, y, dx, dy;
    int dist = 6;

    if (on_off) {
        /* on */
        tmp_at(DISP_BEAM, cmap_to_glyph(S_goodpos));
        for (dx = -dist; dx <= dist; dx++)
            for (dy = -dist; dy <= dist; dy++) {
                x = u.ux + dx;
                y = u.uy + dy;
                /* hero's location is allowed but highlighting the hero's
                   spot makes map harder to read (if using '$' rather than
                   by changing background color) */
                if (u_at(x, y))
                    continue;
                if (can_center_cloud(x, y))
                    tmp_at(x, y);
            }
    } else {
        /* off */
        tmp_at(DISP_END, 0);
    }
}

/* Flood a space. This is a callback function. */
staticfn void
flood_space(coordxy x, coordxy y, genericptr_t poolcnt)
{
    struct monst *mtmp;
    struct trap *ttmp;
    schar ltyp = rn2(3) ? POOL : PUDDLE;

    /* Don't create on the user's space unless poolcnt is -1. */
    if ((* (int*)poolcnt) != -1) {
        if (x == u.ux && y == u.uy)
            return;
    } else {
        ltyp = POOL;
    }

    if (nexttodoor(x, y) || rn2(1 + distmin(u.ux, u.uy, x, y))
        || sobj_at(BOULDER, x, y))
        return;

    if (levl[x][y].typ != ROOM      && levl[x][y].typ != GRASS
        && levl[x][y].typ != TREE   && levl[x][y].typ != GRAVE
        && levl[x][y].typ != LAVAPOOL)
        return;

    /* Never create if there's an immovable trap here. */
    ttmp = t_at(x, y);
    if (ttmp && !delfloortrap(ttmp))
        return;

    if (levl[x][y].typ == TREE) {
        if (ltyp == PUDDLE) /* This won't destroy a tree */
            return;
        if (cansee(x, y))
            pline("A tree sinks into deep water!");
    }
    set_levltyp(x, y, ltyp);
    del_engr_at(x, y);
    water_damage_chain(svl.level.objects[x][y], FALSE);
    maybe_unhide_at(x, y);

    if (!does_block(x, y, &levl[x][y]))
        unblock_point(x, y); /* vision */
    vision_recalc(0);

    if (ltyp == POOL && (mtmp = m_at(x, y)) != 0)
        (void) minliquid(mtmp);
    else
        newsym(x, y);
    (* (int*)poolcnt)++;
}

/* Unflood (dry up) a space. This is a callback function. */
staticfn void
unflood_space(coordxy x, coordxy y, genericptr_t drycnt)
{
    if (levl[x][y].typ != POOL
        && levl[x][y].typ != PUDDLE
        && levl[x][y].typ != MOAT
        && levl[x][y].typ != WATER
        && levl[x][y].typ != FOUNTAIN)
        return;

    set_levltyp(x, y, ROOM);
    maybe_unhide_at(x, y);
    newsym(x, y);
    (* (int*)drycnt)++;
}

staticfn void
seffect_enchant_armor(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    schar s;
    int i;
    boolean special_armor;
    boolean same_color;
    boolean draconic = (uarmc && Is_dragon_scales(uarmc));
    struct obj *otmp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean old_erodeproof, new_erodeproof;
    boolean already_known = objects[sobj->otyp].oc_name_known;
    boolean resists_magic;

    if (already_known) {
        for (i = 0; i < 5; i++) {
            otmp = getobj("enchant", enchant_ok, GETOBJ_NOFLAGS);
            if (!otmp) {
                if (y_n("Really forfeit this scroll?") == 'y')
                    break;
                else
                    continue;
            } else if (otmp && otmp->oclass != ARMOR_CLASS) {
                You("must select armor to enchant.");
                otmp = (struct obj *) 0;
            } else if (otmp && !(otmp->owornmask & W_ARMOR)) {
                You("cannot enchant armor that is not worn.");
                otmp = (struct obj *) 0;
            } else
                break; /* Success! */
        }
     } else {
         otmp = some_armor(&gy.youmonst);
         if (draconic) {
             /* if player is trying to enchant scales onto armor,
                override random armor selection */
             otmp = uarmc;
         }
     }

    if (!otmp) {
        strange_feeling(sobj, !Blind
                        ? "Your skin glows then fades."
                        : "Your skin feels warm for a moment.");
        *sobjp = 0; /* useup() in strange_feeling() */
        exercise(A_CON, !scursed);
        exercise(A_STR, !scursed);
        return;
    }
    if (confused && !draconic) {
        old_erodeproof = (otmp->oerodeproof != 0);
        new_erodeproof = !scursed;
        otmp->oerodeproof = 0; /* for messages */
        if (Blind) {
            otmp->rknown = sobj->bknown;
            pline("%s warm for a moment.", Yobjnam2(otmp, "feel"));
        } else {
            otmp->rknown = TRUE;
            pline("%s covered by a %s %s %s!", Yobjnam2(otmp, "are"),
                  scursed ? "mottled" : "shimmering",
                  hcolor(scursed ? NH_BLACK : NH_GOLDEN),
                  scursed ? "glow"
                  : (is_shield(otmp) ? "layer" : "shield"));
        }
        if (new_erodeproof && (otmp->oeroded || otmp->oeroded2)) {
            otmp->oeroded = otmp->oeroded2 = 0;
            pline("%s as good as new!",
                  Yobjnam2(otmp, Blind ? "feel" : "look"));
        }
        if (old_erodeproof && !new_erodeproof) {
            /* restore old_erodeproof before shop charges */
            otmp->oerodeproof = 1;
            costly_alteration(otmp, COST_DEGRD);
        }
        otmp->oerodeproof = new_erodeproof ? 1 : 0;
        makeknown(SCR_ENCHANT_ARMOR);
        return;
    }
    /* sometimes armor can be enchanted to a higher limit than usual */
    special_armor = (is_elven_armor(otmp->otyp) && Race_if(PM_ELF))
        || (Role_if(PM_WIZARD) && otmp->otyp == CORNUTHAUM)
        || (Role_if(PM_ARCHEOLOGIST) && otmp->otyp == FEDORA);
    if (scursed)
        same_color = (otmp->otyp == BLACK_DRAGON_SCALES);
    else
        same_color = (otmp->otyp == SILVER_DRAGON_SCALES
                      || otmp->otyp == SHIELD_OF_REFLECTION);
    if (Blind)
        same_color = FALSE;

    /* KMH -- catch underflow */
    s = scursed ? -otmp->spe : otmp->spe;

    /* Dragon scales that are worn over body armor will cause the armor to
     * become scaled. */
    if (draconic) {
        if (!maybe_merge_scales(sobj, otmp))
            return;
    }

    if (s > (special_armor ? 5 : 3) && rn2(s)) {
        otmp->in_use = TRUE;
        pline("%s violently %s%s%s for a while, then %s.", Yname2(otmp),
              otense(otmp, Blind ? "vibrate" : "glow"),
              (!Blind && !same_color) ? " " : "",
              (Blind || same_color) ? "" : hcolor(scursed ? NH_BLACK
                                                  : NH_SILVER),
              otense(otmp, "evaporate"));
        remove_worn_item(otmp, FALSE);
        useup(otmp);
        exercise(A_WIS, FALSE);
        return;
    }

    s = scursed ? -1
        : (otmp->spe >= 9)
        ? (rn2(otmp->spe) == 0)
        : sblessed
        ? rnd(3 - otmp->spe / 3)
        : 1;


    /* Items which provide magic resistance also can resist enchanting.
     * They don't resist when their enchantment is zero or negative, that is
     * "un"-enchanting a bad enchantment. But for anything starting at
     * +1 has a x in 7 chance of failure. */
    resists_magic = objects[otmp->otyp].oc_oprop == ANTIMAGIC
        || defends(AD_MAGM, otmp);;
    if (resists_magic && otmp->spe > rn2(7) + 1) {
        pline("%s vibrates and resists!", Yname2(otmp));
        return;
    }

    pline("%s %s%s%s%s for a %s.", Yname2(otmp),
          (s == 0) ? "violently " : "",
          otense(otmp, Blind ? "vibrate" : "glow"),
          (!Blind && !same_color) ? " " : "",
          (Blind || same_color)
          ? "" : hcolor(scursed ? NH_BLACK : NH_SILVER),
          (s * s > 1) ? "while" : "moment");
    /* [this cost handling will need updating if shop pricing is
       ever changed to care about curse/bless status of armor] */
    if (s < 0)
        costly_alteration(otmp, COST_DECHNT);
    if (scursed && !otmp->cursed)
        curse(otmp);
    else if (sblessed && !otmp->blessed)
        bless(otmp);
    else if (!scursed && otmp->cursed)
        uncurse(otmp);
    if (s) {
        int oldspe = otmp->spe;
        /* RDSM act as a ring of increase damage, so we need to remove them
         * and put them back on to recalculate the damage bonus. */
        if (otmp->dragonscales == RED_DRAGON_SCALES)
            Armor_off();

        /* despite being schar, it shouldn't be possible for spe to wrap
           here because it has been capped at 99 and s is quite small;
           however, might need to change s if it takes spe past 99 */

        otmp->spe += s;
        cap_spe(otmp); /* make sure that it doesn't exceed SPE_LIM */
        s = otmp->spe - oldspe; /* cap_spe() might have throttled 's' */

        if (otmp->dragonscales == RED_DRAGON_SCALES) {
            setworn(otmp, W_ARM);
            Armor_on();
        }

        if (s) /* skip if it got changed to 0 */
            adj_abon(otmp, s); /* adjust armor bonus for Dex or Int+Wis */
        gk.known = otmp->known;
        /* update shop bill to reflect new higher price */
        if (s > 0 && otmp->unpaid)
            alter_cost(otmp, 0L);
    }

    /* armor vibrates warningly when enchanted beyond a limit */
    if (otmp->spe > (special_armor ? 5 : 3))
        pline("%s %s.", Yobjnam2(otmp, "suddenly vibrate"),
              Blind ? "again" : "unexpectedly");
}

staticfn void
seffect_destroy_armor(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    struct obj *otmp = some_armor(&gy.youmonst);
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean old_erodeproof, new_erodeproof;
    boolean already_known = objects[sobj->otyp].oc_name_known;

    /* player is prompted to choose what to destroy when:
     * - the scroll is blessed
     * - the scroll is cursed, player is confused, and have it id'd
     * - they must also actually be wearing armor */
    boolean gets_choice = (otmp && (sblessed
                               || (scursed && confused && already_known)));

    if (confused) {
        if (gets_choice) {
            pline("This is a scroll of destroy armor.");
            otmp = getobj("proof", enchant_ok, GETOBJ_NOFLAGS);
        }
        if (!otmp) {
            strange_feeling(sobj, "Your bones itch.");
            *sobjp = 0; /* useup() in strange_feeling() */
            exercise(A_STR, FALSE);
            exercise(A_CON, FALSE);
            return;
        }
        old_erodeproof = (otmp->oerodeproof != 0);
        new_erodeproof = scursed;
        otmp->oerodeproof = 0; /* for messages */
        p_glow2(otmp, NH_PURPLE);
        if (old_erodeproof && !new_erodeproof) {
            /* restore old_erodeproof before shop charges */
            otmp->oerodeproof = 1;
            costly_alteration(otmp, COST_DEGRD);
        }
        otmp->oerodeproof = new_erodeproof ? 1 : 0;
        return;
    }
    if (!scursed || !otmp || !otmp->cursed) {
        if (gets_choice) {
            pline("This is a scroll of destroy armor.");
        }
        if (!destroy_arm(otmp, gets_choice, TRUE)) {
            strange_feeling(sobj, "Your skin itches.");
            *sobjp = 0; /* useup() in strange_feeling() */
            exercise(A_STR, FALSE);
            exercise(A_CON, FALSE);
            return;
        } else
            gk.known = TRUE;
    } else { /* armor and scroll both cursed */
        pline("%s.", Yobjnam2(otmp, "vibrate"));
        if (otmp->spe >= -6) {
            otmp->spe += -1;
            adj_abon(otmp, -1);
        }
        make_stunned((HStun & TIMEOUT) + (long) rn1(10, 10), TRUE);
    }
}

staticfn void
seffect_confuse_monster(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed,
            scursed = sobj->cursed,
            confused = (Confusion != 0),
            altfeedback = (Blind || Invisible);
    const char *const hands = makeplural(body_part(HAND));

    if (gy.youmonst.data->mlet != S_HUMAN || scursed) {
        if (!HConfusion)
            You_feel("confused.");
        make_confused(HConfusion + rnd(100), FALSE);
    } else if (confused) {
        if (!sblessed) {
            Your("%s begin to %s%s.", hands,
                 altfeedback ? "tingle" : "glow ",
                 altfeedback ? "" : hcolor(NH_PURPLE));
            make_confused(HConfusion + rnd(100), FALSE);
        } else {
            pline("A %s%s surrounds your %s.",
                  altfeedback ? "" : hcolor(NH_RED),
                  altfeedback ? "faint buzz" : " glow", body_part(HEAD));
            make_confused(0L, TRUE);
        }
        gk.known = TRUE;
    } else {
        /* scroll vs spell */
        int incr = (sobj->oclass == SCROLL_CLASS) ? 3 : 0;

        if (!sblessed) {
            if (altfeedback)
                Your("%s tingle%s.", hands, u.umconf ? " even more" : "");
            else if (!u.umconf)
                Your("%s begin to glow %s.", hands, hcolor(NH_RED));
            else
                pline_The("%s glow of your %s intensifies.", hcolor(NH_RED),
                          hands);
            incr += rnd(2);
        } else {
            if (altfeedback)
                Your("%s tingle %s sharply.", hands,
                     u.umconf ? "even more" : "very");
            else
                Your("%s glow %s brilliant %s.", hands,
                     u.umconf ? "an even more" : "a", hcolor(NH_RED));
            incr += rn1(8, 2);
        }
        /* after a while, repeated uses become less effective */
        if (u.umconf >= 40)
            incr = 1;
        u.umconf += (unsigned) incr;
        gk.known = TRUE;
    }
}

staticfn void
seffect_scare_monster(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    int ct = 0;
    struct monst *mtmp;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (DEADMONSTER(mtmp))
            continue;
        if (cansee(mtmp->mx, mtmp->my)) {
            if (confused || scursed) {
                mtmp->mflee = mtmp->mfrozen = mtmp->msleeping = 0;
                maybe_moncanmove(mtmp);
            } else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
                monflee(mtmp, 0, FALSE, FALSE);
            if (!mtmp->mtame)
                ct++; /* pets don't laugh at you */
        }
    }
    if (otyp == SCR_SCARE_MONSTER || !ct) {
        if (confused || scursed) {
            Soundeffect(se_sad_wailing, 50);
        } else {
            Soundeffect(se_maniacal_laughter, 50);
        }
        You_hear("%s %s.", (confused || scursed) ? "sad wailing"
                 : "maniacal laughter",
                 !ct ? "in the distance" : "close by");
        gk.known = TRUE;
    }
}

staticfn void
seffect_remove_curse(struct obj **sobjp)
{
    struct obj *sobj = *sobjp; /* scroll or fake spellbook */
    int otyp = sobj->otyp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    struct obj *obj, *nxto;
    long wornmask;
    gk.known = TRUE;

    You_feel(!Hallucination
             ? (!confused ? "like someone is helping you."
                : "like you need some help.")
             : (!confused ? "in touch with the Universal Oneness."
                : "the power of the Force against you!"));

    if (scursed) {
        rndcurse();
        pline_The("scroll disintegrates.");
    } else {
        /* 3.7: this used to use a straight
               for (obj = invent; obj; obj = obj->nobj) {}
           traversal, but for the confused case, secondary weapon might
           become cursed and be dropped, moving it from the invent chain
           to the floor chain at hero's spot, so we have to remember the
           next object prior to processing the current one */
        for (obj = gi.invent; obj; obj = nxto) {
            nxto = obj->nobj;
            /* gold isn't subject to cursing and blessing */
            if (obj->oclass == COIN_CLASS)
                continue;
            /* hide current scroll from itself so that perm_invent won't
               show known blessed scroll losing bknown when confused */
            if (obj == sobj && obj->quan == 1L)
                continue;
            wornmask = (obj->owornmask & ~(W_BALL | W_ART | W_ARTI));
            if (wornmask && !sblessed) {
                /* handle a couple of special cases; we don't
                   allow auxiliary weapon slots to be used to
                   artificially increase number of worn items */
                if (obj == uswapwep) {
                    if (!u.twoweap)
                        wornmask = 0L;
                } else if (obj == uquiver) {
                    if (obj->oclass == WEAPON_CLASS) {
                        /* mergeable weapon test covers ammo,
                           missiles, spears, daggers & knives */
                        if (!objects[obj->otyp].oc_merge)
                            wornmask = 0L;
                    } else if (obj->oclass == GEM_CLASS) {
                        /* possibly ought to check whether
                           alternate weapon is a sling... */
                        if (!uslinging())
                            wornmask = 0L;
                    } else {
                        /* weptools don't merge and aren't
                           reasonable quivered weapons */
                        wornmask = 0L;
                    }
                }
            }
            if (sblessed || wornmask || obj->otyp == LOADSTONE
                || obj->otyp == HEALTHSTONE || obj->otyp == FOULSTONE
                /* this treats an in-use leash as a worn item but does not
                   do the same for lit lamp/candle [seems inconsistent] */
                || (obj->otyp == LEASH && obj->leashmon)) {
                /* water price varies by curse/bless status */
                boolean shop_h2o = (obj->unpaid && obj->otyp == POT_WATER);

                if (confused) {
                    blessorcurse(obj, 2);
                    /* lose knowledge of this object's curse/bless
                       state (even if it didn't actually change) */
                    obj->bknown = 0;
                    /* blessorcurse() only affects uncursed items
                       so no need to worry about price of water
                       going down (hence no costly_alteration) */
                    if (shop_h2o && (obj->cursed || obj->blessed))
                        alter_cost(obj, 0L); /* price goes up */
                } else if (obj->cursed) {
                    if (shop_h2o)
                        costly_alteration(obj, COST_UNCURS);
                    uncurse(obj);
                    /* if the object was known to be cursed and is now
                       known not to be, make the scroll known; it's
                       trivial to identify anyway by comparing inventory
                       before and after */
                    if (obj->bknown && otyp == SCR_REMOVE_CURSE)
                        learnscrolltyp(SCR_REMOVE_CURSE);
                }
            }
        }
        /* if riding, treat steed's saddle as if part of hero's invent */
        if (u.usteed && (obj = which_armor(u.usteed, W_SADDLE)) != 0) {
            if (confused) {
                blessorcurse(obj, 2);
                obj->bknown = 0; /* skip set_bknown() */
            } else if (obj->cursed) {
                uncurse(obj);
                /* like rndcurse(sit.c), effect on regular inventory
                   doesn't show things glowing but saddle does */
                if (!Blind) {
                    pline("%s %s.", Yobjnam2(obj, "glow"),
                              hcolor("amber"));
                    obj->bknown = Hallucination ? 0 : 1;
                } else {
                    obj->bknown = 0; /* skip set_bknown() */
                }
            }
        }
    }
    if (Punished && !confused)
        unpunish();
    if (u.utrap && u.utraptype == TT_BURIEDBALL) {
        buried_ball_to_freedom();
        pline_The("clasp on your %s vanishes.", body_part(LEG));
    }
    update_inventory();
}

staticfn boolean
seffect_create_monster(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean yours = !svc.context.mon_moving;

    if (is_moncard(sobj)) {
        if (yours && u.uen < CARD_COST) {
            pline1(nothing_happens);
            sobj->in_use = FALSE;
            return FALSE;
        }
        use_moncard(sobj, u.ux, u.uy);
        gk.known = TRUE;
        return TRUE;
    }
    if (create_critters(1 + ((confused || scursed) ? 12 : 0)
                        + ((sblessed || rn2(73)) ? 0 : rnd(4)),
                        confused ? &mons[PM_ACID_BLOB]
                        : (struct permonst *) 0,
                        FALSE))
        gk.known = TRUE;
    /* no need to flush monsters; we ask for identification only if the
     * monsters are not visible
     */
    return TRUE;
}

staticfn boolean
seffect_zapping(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    struct obj pseudo;
    pseudo = cg.zeroobj;

    if (sobj->corpsenm == NON_PM) {
	    impossible("seffects: SCR_WAND_ZAP has no zap type!");
        return FALSE;
    }
    if (u.uen < CARD_COST) {
        pline1(nothing_happens);
        sobj->in_use = FALSE;
        return FALSE;
    }
    pseudo.otyp = sobj->corpsenm;
    pseudo.oclass = WAND_CLASS;
    pseudo.dknown = pseudo.obroken = 1; /* Don't id it */

    if (!(objects[pseudo.otyp].oc_dir == NODIR) && !getdir((char *) 0)) {
        if (!Blind)
            pline("%s glows and fades.", The(xname(sobj)));
    } else {
        gc.current_wand = &pseudo;
        weffects(&pseudo);
        gc.current_wand = 0;
    }
    return TRUE;
}

staticfn void
seffect_enchant_weapon(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean old_erodeproof, new_erodeproof;
    boolean resists_magic;

    /* [What about twoweapon mode?  Proofing/repairing/enchanting both
       would be too powerful, but shouldn't we choose randomly between
       primary and secondary instead of always acting on primary?] */
    if (confused && uwep && erosion_matters(uwep)
        && uwep->oclass != ARMOR_CLASS) {
        old_erodeproof = (uwep->oerodeproof != 0);
        new_erodeproof = !scursed;
        uwep->oerodeproof = 0; /* for messages */

        if (is_supermaterial(uwep)) {
            if (!Blind)
                pline("%s for a moment.", Yobjnam2(uwep, "shimmer"));
            return;
        }

        if (Blind) {
            uwep->rknown = sobj->bknown;
            Your("weapon feels warm for a moment.");
        } else {
            uwep->rknown = TRUE;
            pline("%s covered by a %s %s %s!", Yobjnam2(uwep, "are"),
                  scursed ? "mottled" : "shimmering",
                  hcolor(scursed ? NH_PURPLE : NH_GOLDEN),
                  scursed ? "glow" : "shield");
        }
        if (new_erodeproof && erosion_matters(uwep)
            && (uwep->oeroded || uwep->oeroded2)) {
            uwep->oeroded = uwep->oeroded2 = 0;
            pline("%s as good as new!",
                  Yobjnam2(uwep, Blind ? "feel" : "look"));
        }
        if (old_erodeproof && !new_erodeproof) {
            /* restore old_erodeproof before shop charges */
            uwep->oerodeproof = 1;
            costly_alteration(uwep, COST_DEGRD);
        }
        uwep->oerodeproof = new_erodeproof ? 1 : 0;
        return;
    }

     /* Items that grant magic resistance themselves resist enchantment. */
    resists_magic = uwep && (objects[uwep->otyp].oc_oprop == ANTIMAGIC
        || defends(AD_MAGM, uwep));
    if (uwep && resists_magic && uwep->spe > rn2(13) + 1) {
        pline("%s vibrates and resists!", Yname2(uwep));
        return;
    }

    /* Adjusted max weapon enchantment. For more exciting hackin 'n
       slashin we will allow the new 'soft' limit to be 12. This
       allows weapons to be safely enchanted up to 13.
       After +5, scrolls only grant +1 so this will still require
       many scrolls to achieve. */
    if (!chwepon(sobj, scursed ? -1
                 : !uwep ? 1
                 : (uwep->spe >= (WEP_ENCHANT_MAX + 1)) ? !rn2(uwep->spe)
                 : sblessed ? rnd(wep_enchant_range(uwep->spe))
                 : 1))
        *sobjp = 0; /* nothing enchanted: strange_feeling -> useup */
    if (uwep)
        cap_spe(uwep);
}

/* Helper function to regular how enchant weapon bonuses are distributed.
 * I couldn't figure out a simple formula to calculate this without giving
 * too much enchantment to the player. My goal was to allow the player to roll
 * a d2 from 11, so that max 13 is possible, but difficult to reach.
 *
 * There is a long stretch from 4 to 11 where the player can only get +d2
 * enchantment. This is by design. If we let them roll d3 or even d4, that
 * destroys the item scarcity.
*/
staticfn int
wep_enchant_range(int wspe) {
    if (wspe < 4)
        return 3;
    else if (wspe < 6)
        return 2;
    return 1;
}

staticfn void
seffect_taming(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean confused = (Confusion != 0);
    int candidates, res, results, vis_results;

    if (u.uswallow) {
        candidates = 1;
        results = vis_results = maybe_tame(u.ustuck, sobj);
    } else {
        int i, j, bd = confused ? 5 : 1;
        struct monst *mtmp;

        /* note: maybe_tame() can return either positive or
           negative values, but not both for the same scroll */
        candidates = results = vis_results = 0;
        for (i = -bd; i <= bd; i++)
            for (j = -bd; j <= bd; j++) {
                if (!isok(u.ux + i, u.uy + j))
                    continue;
                if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0
                    || (!i && !j && (mtmp = u.usteed) != 0)) {
                    ++candidates;
                    res = maybe_tame(mtmp, sobj);
                    results += res;
                    if (canspotmon(mtmp))
                        vis_results += res;
                }
            }
    }
    if (!results) {
        pline("Nothing interesting %s.",
              !candidates ? "happens" : "seems to happen");
    } else {
        pline_The("neighborhood %s %sfriendlier.",
                  vis_results ? "is" : "seems",
                  (results < 0) ? "un" : "");
        if (vis_results > 0)
            gk.known = TRUE;
    }
}

staticfn void
seffect_genocide(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                             || objects[otyp].oc_name_known);

    if (!already_known)
        You("have found a scroll of exile!");
    gk.known = TRUE;

    do_genocide((!scursed) | (2 * !!Confusion), !sblessed);
}

staticfn void
seffect_light(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);

    if (!confused) {
        if (!Blind)
            gk.known = TRUE;
        litroom(!scursed, sobj);
        if (!scursed) {
            if (lightdamage(sobj, TRUE, 5))
                gk.known = TRUE;
        }
    } else {
        int pm = scursed ? PM_BLACK_LIGHT : PM_YELLOW_LIGHT;

        if ((svm.mvitals[pm].mvflags & G_GONE)) {
            pline("Tiny lights sparkle in the air momentarily.");
        } else {
            /* surround with cancelled tame lights which won't explode */
            struct monst *mon;
            boolean sawlights = FALSE;
            int i, numlights = rn1(2, 3) + (sblessed * 2);

            for (i = 0; i < numlights; ++i) {
                mon = makemon(&mons[pm], u.ux, u.uy,
                              MM_EDOG | NO_MINVENT | MM_NOMSG);
                if (mon) {
                    if (Role_if(PM_CARTOMANCER))
                        mon->mpeaceful = 1;
                    else
                        initedog(mon, TRUE);
                    mon->msleeping = 0;
                    mon->mcan = TRUE;
                    if (canspotmon(mon))
                        sawlights = TRUE;
                    newsym(mon->mx, mon->my);
                }
            }
            if (sawlights) {
                pline("Lights appear all around you!");
                gk.known = TRUE;
            }
        }
    }
}

staticfn void
seffect_cloning(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    struct obj *otmp, *otmp2;
    struct monst *mtmp;
    int otyp = sobj->otyp, otyp2;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean already_known = objects[otyp].oc_name_known;

    if (!already_known)
        learnscroll(sobj);

    if (confused) {
        if (!Hallucination) {
            You("clone yourself!");
        } else {
            You("realize that you have been a clone all along!");
        }
        int mndx = monsndx(gy.youmonst.data);

        /* No permapets for cartomancers */
        if (Role_if(PM_CARTOMANCER)) {
            get_particular_moncard(mndx, sobj);
            return;
        }

        if (Upolyd)
            mtmp = cloneu();
        else {
            if (svm.mvitals[mndx].mvflags & G_EXTINCT) {
                You("momentarily feel like your kind has no future.");
                return;
            }

            mtmp = makemon(&mons[mndx], u.ux, u.uy,
                           sblessed ? (NO_MINVENT | MM_EDOG)
                                    : (scursed ? (NO_MINVENT | MM_ANGRY) : NO_MINVENT));
            if (!mtmp) {
                pline("Never mind.");
                return;
            }
            if (sblessed) {
                initedog(mtmp, TRUE);
                u.uconduct.pets++;
            } else if (!scursed && !mtmp->mrabid) {
                mtmp->mpeaceful = 1;
            }

            mtmp->mcloned = 1;
            mtmp = christen_monst(mtmp, svp.plname);
            mtmp->m_lev = u.ulevel;
            mtmp->mhpmax = u.uhpmax;
            mtmp->mhp = u.uhp;
            newsym(mtmp->mx, mtmp->my);
            disp.botl = 1;
        }
    } else {
        if (!already_known)
            You("have found a scroll of cloning!");
        otmp = getobj("clone", any_obj_ok, GETOBJ_PROMPT);
        if (!otmp) {
            pline("Never mind.");
            return;
        }
        /* Unique/abusable items */
        if (otmp->otyp == BELL_OF_OPENING) {
            otyp2 = BELL;
        } else if (otmp->otyp == CANDELABRUM_OF_INVOCATION) {
            otyp2 = WAX_CANDLE;
        } else if (otmp->otyp == AMULET_OF_YENDOR) {
            otyp2 = FAKE_AMULET_OF_YENDOR;
        } else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
            otyp2 = SPE_BLANK_PAPER;
        } else {
            otyp2 = otmp->otyp;
        }
        otmp2 = mksobj_at(otyp2, u.ux, u.uy, FALSE, FALSE);

        if (!otmp2)
            impossible("Invalid cloned object?");

        /* beatitude */
        if (scursed)
            curse(otmp2);
        else {
            otmp2->blessed = otmp->blessed;
            otmp2->cursed = otmp->cursed;
        }

        /* charge / enchantment */
        if (sblessed)
            otmp2->spe = otmp->spe;
        else
            otmp2->spe = min(otmp->spe, 0);

        /* other properties */
        otmp2->oeroded = otmp->oeroded;
        otmp2->oeroded2 = otmp->oeroded2;
        otmp2->oerodeproof = otmp->oerodeproof;
        otmp2->olocked = otmp->olocked;
        otmp2->obroken = otmp->obroken;
        otmp2->otrapped = otmp->otrapped;
        otmp2->recharged = otmp->recharged;
        otmp2->lamplit = otmp->lamplit;
        otmp2->globby = otmp->globby;
        otmp2->greased = otmp->greased;
        otmp2->corpsenm = otmp->corpsenm;
        otmp2->oeaten = otmp->oeaten;
        otmp2->opoisoned = otmp->opoisoned;


        /* Copy the name over. Artifact names will not be copied.  */
        if (otmp->oextra)
            oname(otmp2, otmp->oextra->oname, ONAME_VIA_NAMING);

        /* but suppressing fruit details leads to "bad fruit #0" */
        if (otmp2->otyp == SLIME_MOLD)
            otmp2->spe = svc.context.current_fruit;

        /* Prevent wishing exploits */
        if (otmp2->otyp == WAN_WISHING) {
            otmp2->otyp = WAN_WONDER;
            otmp2->spe = rn1(10, 15);
        } else if (otmp2->otyp == SCR_ZAPPING && otmp2->corpsenm == WAN_WISHING) {
            otmp2->corpsenm = WAN_WONDER;
        } else if (otmp2->otyp == MAGIC_LAMP) {
            otmp2->otyp = OIL_LAMP;
        } else if (otmp2->otyp == SCR_CLONING) {
            otmp2->otyp = SCR_BLANK_PAPER;
        } else if (otmp2->otyp == DECK_OF_FATE) {
            otmp2->otyp = PLAYING_CARD_DECK;
        } else if (otmp2->otyp == MAGIC_MARKER) {
            otmp2->otyp = ATHAME;
            if (sblessed)
                otmp2->spe = rnd(otmp->spe / 10);
        }

        /* Prevent any weird class conversion errors */
        otmp2->oclass = objects[otmp2->otyp].oc_class;
        otmp2->quan = 1;

        /* Weight could change due to material/type */
        otmp2->owt = weight(otmp2);

        /* cartomancers feel guilty for counterfeiting */
        if (Role_if(PM_CARTOMANCER) && otmp2->oclass == SCROLL_CLASS) {
            You("feel incredibly guilty about forging a card!");
            adjalign(-20);
            change_luck(-5);
        }

        obj_extract_self(otmp2);

        /* If an unpaid item is cloned, that item also inherits the cost
         * and unpaid status. */
        /* You clone it, you buy it! */
        if (otmp->unpaid)
            addtobill(otmp2, FALSE, FALSE, FALSE);

        (void) hold_another_object(otmp2, "Whoops! %s out of your grasp.",
                                The(aobjnam(otmp2, "tumbles")),
                                (const char *) 0);
    }
}

staticfn void
seffect_charging(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                             || objects[otyp].oc_name_known);
    struct obj *otmp;

    if (confused) {
        if (scursed) {
            You_feel("discharged.");
            u.uen = 0;
        } else {
            You_feel("charged up!");
            u.uen += d(sblessed ? 6 : 4, 4);
            if (u.uen > u.uenmax) /* if current energy is already at   */
                u.uenmax = u.uen; /* or near maximum, increase maximum */
            else
                u.uen = u.uenmax; /* otherwise restore current to max  */
        }
        disp.botl = TRUE;
        return;
    }
    /* known = TRUE; -- handled inline here */
    if (!already_known) {
        pline("This is a charging scroll.");
        learnscroll(sobj);
    }
    /* use it up now to prevent it from showing in the
       getobj picklist because the "disappears" message
       was already delivered */
    useup(sobj);
    *sobjp = 0; /* it's gone */
    otmp = getobj("charge", charge_ok, GETOBJ_PROMPT | GETOBJ_ALLOWCNT);
    if (otmp)
        recharge(otmp, scursed ? -1 : sblessed ? 1 : 0);
}

staticfn void
seffect_amnesia(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;

    gk.known = TRUE;
    forget((!sblessed ? ALL_SPELLS : 0));
    if (Hallucination) /* Ommmmmm! */
        Your("mind releases itself from mundane concerns.");
    else if (!strncmpi(svp.plname, "Maud", 4))
        pline("As your mind turns inward on itself,"
              " you forget everything else.");
    else if (rn2(2))
        pline("Who was that Maud person anyway?");
    else
        pline("Thinking of Maud you forget everything else.");
    exercise(A_WIS, FALSE);
}

staticfn void
seffect_fire(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean sblessed = sobj->blessed;
    boolean confused = (Confusion != 0);
    boolean already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                             || objects[otyp].oc_name_known);
    coord cc;
    int dam, cval;

    cc.x = u.ux;
    cc.y = u.uy;
    cval = bcsign(sobj);
    dam = (2 * (rn1(3, 3) + 2 * cval) + 1) / 3;
    useup(sobj);
    *sobjp = 0; /* it's gone */
    if (!already_known)
        (void) learnscrolltyp(SCR_FIRE);
    if (confused) {
        if (Underwater) {
            pline("A little %s around you vaporizes.", hliquid("water"));
        } else if (fully_resistant(FIRE_RES)) {
            shieldeff(u.ux, u.uy);
            monstseesu(M_SEEN_FIRE);
            if (!Blind)
                pline("Oh, look, what a pretty fire in your %s.",
                      makeplural(body_part(HAND)));
            else
                You_feel("a pleasant warmth in your %s.",
                         makeplural(body_part(HAND)));
        } else {
            monstunseesu(M_SEEN_FIRE);
            pline_The("scroll catches fire and you burn your %s.",
                      makeplural(body_part(HAND)));
            losehp(1, "scroll of fire", KILLED_BY_AN);
        }
        return;
    }
    if (Underwater) {
        pline_The("%s around you vaporizes violently!", hliquid("water"));
    } else {
        if (sblessed) {
            if (!already_known)
                pline("This is a scroll of fire!");
            dam *= 5;
            pline("Where do you want to center the explosion?");
            getpos_sethilite(display_stinking_cloud_positions,
                             can_center_cloud);
            (void) getpos(&cc, TRUE, "the desired position");
            if (!can_center_cloud(cc.x, cc.y)) {
                /* try to reach too far, get burned */
                cc.x = u.ux;
                cc.y = u.uy;
            }
        }
        if (u_at(cc.x, cc.y)) {
            pline_The("scroll erupts in a tower of flame!");
            iflags.last_msg = PLNMSG_TOWER_OF_FLAME; /* for explode() */
            burn_away_slime();
        }
    }
    explode(cc.x, cc.y, ZT_SPELL(ZT_FIRE), dam, SCROLL_CLASS, EXPL_FIERY);
}

staticfn void
seffect_earth(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);

    /* TODO: handle steeds */
    if (!Is_rogue_level(&u.uz) && has_ceiling(&u.uz)
        && (!In_endgame(&u.uz) || Is_earthlevel(&u.uz))) {
        coordxy x, y;
        int nboulders = 0;

        /* Identify the scroll */
        if (u.uswallow) {
            You_hear("rumbling.");
        } else {
            if (!avoid_ceiling(&u.uz)) {
                pline_The("%s rumbles %s you!", ceiling(u.ux, u.uy),
                          sblessed ? "around" : "above");
            } else {
                char matbuf[BUFSZ];
                const char *const avalanche = "avalanche";

                Sprintf(matbuf, "%s",
                        sblessed ? makeplural(avalanche) : an(avalanche));
                pline("%s of boulders %s %s you!",
                      upstart(matbuf), vtense(matbuf, "materialize"),
                      sblessed ? "around" : "above");
            }
        }
        gk.known = 1;
        sokoban_guilt();

        /* Loop through the surrounding squares */
        if (!scursed)
            for (x = u.ux - 1; x <= u.ux + 1; x++) {
                for (y = u.uy - 1; y <= u.uy + 1; y++) {
                    /* Is this a suitable spot? */
                    if (isok(x, y) && !closed_door(x, y)
                        && !IS_OBSTRUCTED(levl[x][y].typ)
                        && !IS_AIR(levl[x][y].typ)
                        && (x != u.ux || y != u.uy)) {
                        nboulders +=
                            drop_boulder_on_monster(x, y, confused, TRUE);
                    }
                }
            }
        /* Attack the player */
        if (!sblessed) {
            drop_boulder_on_player(confused, !scursed, TRUE, FALSE);
        } else if (!nboulders)
            pline("But nothing else happens.");
    }
}

staticfn void
seffect_punishment(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);

    gk.known = TRUE;
    if (u.ugangr && confused && scursed) {
        int saved_anger = u.ugangr;
        u.ugangr -= 1;
        if (u.ugangr < 0)
            u.ugangr = 0;
        if (u.ugangr != saved_anger) {
            if (u.ugangr) {
                pline("%s seems %s.", u_gname(),
                        Hallucination ? "groovy" : "slightly mollified");
            } else {
                pline("%s seems %s.", u_gname(),
                        Hallucination ? "cosmic (not a new fact)"
                                    : "mollified");
                u.reconciled = REC_MOL;
            }
        }
        return;
    }
    if (confused || sblessed) {
        You_feel("guilty.");
        return;
    }
    punish(sobj);
}

staticfn void
seffect_stinking_cloud(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                             || objects[otyp].oc_name_known);

    if (!already_known)
        You("have found a scroll of stinking cloud!");
    gk.known = TRUE;
    do_stinking_cloud(sobj, already_known);
}

void
seffect_flood(struct obj **sobjp, struct monst *mtmp)
{
    struct obj *sobj = *sobjp;
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean isyou = (mtmp == &gy.youmonst);
    boolean confused = isyou ? (Confusion != 0) : mtmp->mconf;
    int wx = u.ux, wy = u.uy, range = 4 + (2 * bcsign(sobj));

    if (!isyou)
        extract_from_minvent(mtmp, sobj, FALSE, TRUE);

    if (confused) {
        int dried_up = 0;
        do_clear_area(wx, wy, range, unflood_space, (genericptr_t) &dried_up);
        if (dried_up) {
            gk.known = TRUE;
            if (Hallucination)
                pline("Oh no! Dehydration!");
            else {
                pline("You are suddenly very dry!");
                /* TODO: Dry out towels, etc */
            }
        } else {
            pline("The air around you suddenly feels very dry.");
        }
        if (maybe_polyd(is_grung(gy.youmonst.data), Race_if(PM_GRUNG)))
            dehydrate(u.hydration - (u.hydration / 5));
    } else {
        int madepools = 0;
        int stilldry = -1;
        int x, y, safe_pos = 0;
        do_clear_area(wx, wy, range, flood_space, (genericptr_t) &madepools);

        /* check if there are safe tiles around the user */
        for (x = wx - 1; x <= wx + 1; x++) {
            for (y = wy - 1; y <= wy + 1; y++) {
                if (x != wx && y != wy &&
                    goodpos(x, y, mtmp, 0)) {
                    safe_pos++;
                }
            }
        }

        /* cursed and uncursed might put a water tile on
         * user's position */
        if (!sblessed && safe_pos > 0) {
            if (scursed || rnl(10) > 5)
                flood_space(wx, wy, &stilldry);
        }

        if (madepools || !stilldry) {
            if (Hallucination)
                pline("A totally gnarly wave comes in!");
            else
                pline("A flood surges through the area!");
        } else {
            pline("The air around you suddenly feels very humid.");
        }
        gk.known = TRUE;
        rehydrate(rn1(1500, 4500));
        /* Cleanup when used in muse.c */
        if (!isyou)
            delobj(sobj);
    }
}

staticfn void
seffect_blank_paper(struct obj **sobjp UNUSED)
{
    if (Blind)
        You("don't remember there being any magic words on this scroll.");
    else {
        if (Role_if(PM_CARTOMANCER))
            pline("This card is useless!");
        else
            pline("This scroll seems to be blank.");
    }
    gk.known = TRUE;
}

staticfn void
seffect_teleportation(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);

    if (confused || scursed) {
        level_tele();
        /* gives "materialize on different/same level!" message, must
           be a teleport scroll */
        gk.known = TRUE;
    } else {
        scrolltele(sobj);
        /* this will call learnscroll() as appropriate, and has results
           which maybe shouldn't result in the scroll becoming known;
           either way, no need to set gk.known here */
    }
}

staticfn void
seffect_gold_detection(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);

    if ((confused || scursed) ? trap_detect(sobj) : gold_detect(sobj))
        *sobjp = 0; /* failure: strange_feeling() -> useup() */
}

staticfn void
seffect_food_detection(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;

    if (food_detect(sobj))
        *sobjp = 0; /* nothing detected: strange_feeling -> useup */
    else if (!objects[sobj->otyp].oc_name_known)
        (void) learnscrolltyp(SCR_FOOD_DETECTION);
}

staticfn void
seffect_knowledge(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp, learnabout, i;

    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean already_known = (objects[otyp].oc_name_known);
    boolean learned_something = FALSE;

    i = (sblessed && rnl(5) == 0) ? 2 : 1;

    /* known = TRUE; -- handled inline here */
    /* use up the scroll first, before learnscrolltyp() -> makeknown()
        performs perm_invent update; also simplifies empty invent check */
    useup(sobj);
    *sobjp = 0; /* it's gone */

    if (confused) {
        if (!already_known)
            You("know this to be a knowledge scroll.");
        return;
    } else if (scursed) {
        You("start studying the scroll, but it confounds you...");
        make_confused(HConfusion + rnd(50), FALSE);
        return;
    }
    /* Archeologists are great at research. */
    if (Role_if(PM_ARCHEOLOGIST) && !scursed)
        specified_id();

    if (!already_known)
        (void) learnscrolltyp(SCR_KNOWLEDGE);

    for (; i>0;i--) {
        if ((learnabout = learnme())) {
            pline_The("image of %s %s.",
                an(simple_typename(learnabout)),
                !rn2(3) ? "etches itself into your mind"
                      : rn2(2) ? "imprints itself on your mind"
                               : "sears into your memory");
            learned_something = TRUE;
        } else {
            break;
        }
    }
    if (!learned_something)
        You("don't learn anything new...");
}

staticfn void
seffect_identify(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    int otyp = sobj->otyp;
    boolean is_scroll = (sobj->oclass == SCROLL_CLASS);
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    boolean already_known = (sobj->oclass == SPBOOK_CLASS /* spell */
                             || objects[otyp].oc_name_known);

    if (is_scroll) { /* scroll of identify */
        /* known = TRUE; -- handled inline here */
        /* use up the scroll first, before learnscrolltyp() -> makeknown()
           performs perm_invent update; also simplifies empty invent check */
        useup(sobj);
        *sobjp = 0; /* it's gone */
        if (confused) {
            You("identify yourself...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
        } else if (scursed && !already_known)
            You("identify this as an identify scroll.");
        else if (!already_known)
            pline("This is an identify scroll.");

        if (!already_known)
            (void) learnscrolltyp(SCR_IDENTIFY);
        if (confused || (scursed && !already_known))
            return;
    }

    if (gi.invent) {
        int cval = 1;
        if (sblessed || (!scursed && !rn2(5))) {
            cval = rn2(5);
            /* note: if cval==0, identify all items */
            if (cval == 1 && sblessed && Luck > 0)
                ++cval;
        }
        /* Archeologists are great at research. */
        if (Role_if(PM_ARCHEOLOGIST) && !scursed && cval > 0)
            ++cval;

        /* Cavemen are not so great at knowledge... */
        if (Role_if(PM_CAVE_DWELLER) && cval == 0)
            cval = 1;

        identify_pack(cval, !already_known);
    } else {
        /* spell cast with inventory empty or scroll read when it's
           the only item leaving empty inventory after being used up */
        pline("You're not carrying anything%s to be identified.",
              (is_scroll) ? " else" : "");
    }
}

staticfn void
seffect_magic_mapping(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean is_scroll = (sobj->oclass == SCROLL_CLASS);
    boolean sblessed = sobj->blessed;
    boolean scursed = sobj->cursed;
    boolean confused = (Confusion != 0);
    int cval;

    if (is_scroll) {
        if (svl.level.flags.nommap) {
            Your("mind is filled with crazy lines!");
            if (Hallucination)
                pline("Wow!  Modern art.");
            else
                Your("%s spins in bewilderment.", body_part(HEAD));
            make_confused(HConfusion + rnd(30), FALSE);
            return;
        }
        if (sblessed) {
            coordxy x, y;

            for (x = 1; x < COLNO; x++)
                for (y = 0; y < ROWNO; y++)
                    if (levl[x][y].typ == SDOOR) {
                        cvt_sdoor_to_door(&levl[x][y]);
                        if (Is_rogue_level(&u.uz))
                            unblock_point(x, y);
                    }
            /* do_mapping() already reveals secret passages */
        }
        gk.known = TRUE;
    }

    if (svl.level.flags.nommap) {
        Your("%s spins as %s blocks the spell!", body_part(HEAD),
             something);
        make_confused(HConfusion + rnd(30), FALSE);
        return;
    }
    pline("A map coalesces in your mind!");
    cval = (scursed && !confused);
    if (cval)
        HConfusion = 1; /* to screw up map */
    notice_mon_off();
    do_mapping(is_scroll ? 0 : P_SKILL(P_ENCHANTMENT_SPELL));
    notice_mon_on();
    if (cval) {
        HConfusion = 0; /* restore */
        pline("Unfortunately, you can't grasp the details.");
    }
}

#ifdef MAIL_STRUCTURES
staticfn void
seffect_mail(struct obj **sobjp)
{
    struct obj *sobj = *sobjp;
    boolean odd = (sobj->o_id % 2) == 1;

    gk.known = TRUE;
    switch (sobj->spe) {
    case 2:
        /* "stamped scroll" created via magic marker--without a stamp */
        if (Role_if(PM_CARTOMANCER))
            pline("The rules on this card read \"Discard upon use\".");
        else
            pline("This scroll is marked \"%s\".",
                odd ? "Postage Due" : "Return to Sender");
        break;
    case 1:
        /* scroll of mail obtained from bones file or from wishing;
           note to the puzzled: the game Larn actually sends you junk
           mail if you win! */
        pline("This seems to be %s.",
              odd ? "a chain letter threatening your luck"
              : "junk mail addressed to the finder of the Eye of Larn");
        break;
    default:
#ifdef MAIL
        readmail(sobj);
#else
        /* unreachable since with MAIL undefined, sobj->spe won't be 0;
           as a precaution, be prepared to give arbitrary feedback;
           caller has already reported that it disappears upon reading */
        pline("That was a scroll of mail?");
#endif
        break;
    }
}
#endif /* MAIL_STRUCTURES */

/* scroll effects; return 1 if we use up the scroll and possibly make it
   become discovered, 0 if caller should take care of those side-effects */
int
seffects(
    struct obj *sobj) /* sobj - scroll or fake spellbook for spell */
{
    int otyp = sobj->otyp;

    if (objects[otyp].oc_magic)
        exercise(A_WIS, TRUE);                       /* just for trying */

    switch (otyp) {
#ifdef MAIL_STRUCTURES
    case SCR_MAIL:
        seffect_mail(&sobj);
        break;
#endif
    case SCR_ENCHANT_ARMOR:
        seffect_enchant_armor(&sobj);
        break;
    case SCR_DESTROY_ARMOR:
        seffect_destroy_armor(&sobj);
        break;
    case SCR_CONFUSE_MONSTER:
    case SPE_CONFUSE_MONSTER:
        seffect_confuse_monster(&sobj);
        break;
    case SCR_SCARE_MONSTER:
    case SPE_CAUSE_FEAR:
        seffect_scare_monster(&sobj);
        break;
    case SCR_BLANK_PAPER:
        seffect_blank_paper(&sobj);
        break;
    case SCR_REMOVE_CURSE:
    case SPE_REMOVE_CURSE:
        seffect_remove_curse(&sobj);
        break;
    case SCR_CREATE_MONSTER:
    case SPE_CREATE_MONSTER:
        if (!seffect_create_monster(&sobj))
            return 1;
        break;
    case SCR_ZAPPING:
        if (!seffect_zapping(&sobj))
            return 1;
        break;
    case SCR_ENCHANT_WEAPON:
        seffect_enchant_weapon(&sobj);
        break;
    case SCR_TAMING:
        seffect_taming(&sobj);
        break;
    case SCR_EXILE:
        seffect_genocide(&sobj);
        break;
    case SCR_LIGHT:
        seffect_light(&sobj);
        break;
    case SCR_TELEPORTATION:
        seffect_teleportation(&sobj);
        break;
    case SCR_GOLD_DETECTION:
        seffect_gold_detection(&sobj);
        break;
    case SCR_FOOD_DETECTION:
    case SPE_DETECT_FOOD:
        seffect_food_detection(&sobj);
        break;
    case SCR_KNOWLEDGE:
        seffect_knowledge(&sobj);
        break;
    case SCR_IDENTIFY:
        seffect_identify(&sobj);
        break;
    case SCR_CLONING:
        seffect_cloning(&sobj);
        break;
    case SCR_CHARGING:
        seffect_charging(&sobj);
        break;
    case SCR_MAGIC_MAPPING:
    case SPE_MAGIC_MAPPING:
        seffect_magic_mapping(&sobj);
        break;
    case SCR_AMNESIA:
        seffect_amnesia(&sobj);
        break;
    case SCR_FIRE:
        seffect_fire(&sobj);
        break;
    case SCR_EARTH:
        seffect_earth(&sobj);
        break;
    case SCR_PUNISHMENT:
        seffect_punishment(&sobj);
        break;
    case SCR_STINKING_CLOUD:
        seffect_stinking_cloud(&sobj);
        break;
    case SCR_FLOOD:
        seffect_flood(&sobj, &gy.youmonst);
        break;
    default:
        impossible("What weird effect is this? (%u)", otyp);
    }
    /* if sobj is gone, we've already called useup() above and the
       update_inventory() that it performs might have come too soon
       (before charging an item, for instance) */
    if (!sobj)
        update_inventory();
    return sobj ? 0 : 1;
}

void
drop_boulder_on_player(
    boolean confused,
    boolean helmet_protects,
    boolean byu,
    boolean skip_uswallow)
{
    int dmg;
    struct obj *otmp2;

    /* hit monster if swallowed */
    if (u.uswallow && !skip_uswallow) {
        drop_boulder_on_monster(u.ux, u.uy, confused, byu);
        return;
    }

    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE);
    if (!otmp2)
        return;
    otmp2->quan = confused ? rn1(5, 2) : 1;
    otmp2->owt = weight(otmp2);
    if (!amorphous(gy.youmonst.data) && !Passes_walls
        && !noncorporeal(gy.youmonst.data) && !unsolid(gy.youmonst.data)) {
        You("are hit by %s!", doname(otmp2));
        dmg = (int) (dmgval(otmp2, &gy.youmonst) * otmp2->quan);
        if (uarmh && helmet_protects) {
            if (hard_helmet(uarmh)) {
                if (otmp2->owt >= 400) {
                    if (dmg > 2)
                        dmg -= 2;
                    Your("helmet only slightly protects you.");
                } else {
                    if (dmg > 2)
                        dmg = 2;
                    pline("Fortunately, you are wearing a hard helmet.");
                }
            } else if (flags.verbose) {
                pline("%s does not protect you.", Yname2(uarmh));
            }
        }
    } else
        dmg = 0;
    wake_nearto(u.ux, u.uy, 4 * 4);
    /* Must be before the losehp(), for bones files */
    if (!flooreffects(otmp2, u.ux, u.uy, "fall")) {
        place_object(otmp2, u.ux, u.uy);
        stackobj(otmp2);
        newsym(u.ux, u.uy);
    }
    if (dmg)
        losehp(Maybe_Half_Phys(dmg), "scroll of earth", KILLED_BY_AN);
    if (dmg > 6)
        make_stunned((HStun & TIMEOUT) + (long) d(dmg / 6 + 1, 3), TRUE);
}

boolean
drop_boulder_on_monster(coordxy x, coordxy y, boolean confused, boolean byu)
{
    struct obj *otmp2;
    struct monst *mtmp;

    /* Make the object(s) */
    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE);
    if (!otmp2)
        return FALSE; /* Shouldn't happen */
    otmp2->quan = confused ? rn1(5, 2) : 1;
    otmp2->owt = weight(otmp2);

    /* Find the monster here (won't be player) */
    mtmp = m_at(x, y);
    if (mtmp && !amorphous(mtmp->data) && !passes_walls(mtmp->data)
        && !noncorporeal(mtmp->data) && !unsolid(mtmp->data)) {
        struct obj *helmet = which_armor(mtmp, W_ARMH);
        long mdmg;

        if (cansee(mtmp->mx, mtmp->my)) {
            pline("%s is hit by %s!", Monnam(mtmp), doname(otmp2));
            if (mtmp->minvis && !canspotmon(mtmp))
                map_invisible(mtmp->mx, mtmp->my);
        } else if (engulfing_u(mtmp))
            You_hear("something hit %s %s over your %s!",
                     s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH),
                     body_part(HEAD));

        mdmg = dmgval(otmp2, mtmp) * otmp2->quan;
        if (helmet) {
            if (hard_helmet(helmet)) {
                if (otmp2->owt >= 400) {
                    if (mdmg > 2)
                        mdmg -= 2;
                    if (canspotmon(mtmp)) {
                        pline("%s helmet is only slightly protective.",
                              s_suffix(Monnam(mtmp)));
                    }
                } else {
                    if (mdmg > 2)
                        mdmg = 2;
                    if (canspotmon(mtmp))
                        pline("Fortunately, %s is wearing a hard helmet.",
                              mon_nam(mtmp));
                    else if (!Deaf)
                        You_hear("a clanging sound.");
                }
            } else {
                if (canspotmon(mtmp))
                    pline("%s's %s does not protect %s.", Monnam(mtmp),
                          xname(helmet), mhim(mtmp));
            }
        }
        mtmp->mhp -= mdmg;
        if (DEADMONSTER(mtmp)) {
            if (byu) {
                killed(mtmp);
            } else {
                pline("%s is killed.", Monnam(mtmp));
                mondied(mtmp);
            }
        } else {
            wakeup(mtmp, byu);
            /* Stun if damage was over 6. */
            if (mdmg > 6 && !mtmp->mstun) {
                if (canseemon(mtmp))
                    pline("%s %s for a moment.", Monnam(mtmp),
                            makeplural(stagger(mtmp->data, "stagger")));
                mtmp->mstun = 1;
            }
        }
        wake_nearto(x, y, 4 * 4);
    } else if (engulfing_u(mtmp)) {
        obfree(otmp2, (struct obj *) 0);
        /* fall through to player */
        drop_boulder_on_player(confused, TRUE, FALSE, TRUE);
        return 1;
    }
    /* Drop the rock/boulder to the floor */
    if (!flooreffects(otmp2, x, y, "fall")) {
        place_object(otmp2, x, y);
        stackobj(otmp2);
        newsym(x, y); /* map the rock */
    }
    return TRUE;
}

/* user-specified 'applying' as well as wands exploding by accident
 * during use (called by backfire() in zap.c)
 *
 * If the effect is directly recognisable as pertaining to a
 * specific wand, the wand should be makeknown()
 * Otherwise, if there is an ambiguous or indirect but visible effect
 * the wand should be allowed to be named by the user.
 *
 * If there is no obvious effect,  do nothing. (Should this be changed
 * to letting the user call that type of wand?)
 *
 * hero_broke is nonzero if the user initiated the action that caused
 * the wand to explode (zapping or applying).
 */
void
wand_explode(struct obj *obj, int chg /* recharging */, struct monst *mon)
{
    int dmg, charges, dmg_multiplier, expltype = EXPL_MAGICAL;
    boolean hero_broke = (mon == &gy.youmonst);

    /* number of damage dice */
    if (!chg)
        chg = 2; /* zap/engrave adjustment */
    charges = obj->spe + chg;
    if (charges < 2)
        charges = 2; /* arbitrary minimum */

    /* size of damage dice */
    switch (obj->otyp) {
    case WAN_WISHING:
        dmg_multiplier = 12;
        break;
    case WAN_CANCELLATION:
    case WAN_DEATH:
    case WAN_DRAINING:
    case WAN_POLYMORPH:
    case WAN_UNDEAD_TURNING:
        dmg_multiplier = 10;
        break;
    case WAN_COLD:
        expltype = EXPL_FROSTY;
        dmg_multiplier = 8;
        break;
    case WAN_FIRE:
        expltype = EXPL_FIERY;
        dmg_multiplier = 8;
        break;
    case WAN_POISON_GAS:
        expltype = EXPL_NOXIOUS;
        dmg_multiplier = 8;
        break;
    case WAN_CORROSION:
        expltype = EXPL_WET;
        dmg_multiplier = 8;
        break;
    case WAN_LIGHTNING:
    case WAN_MAGIC_MISSILE:
        dmg_multiplier = 8;
        break;
    default:
        dmg_multiplier = 6;
        break;
    }

    if (obj->otyp == WAN_STRIKING) {
        /* we want this before the explosion instead of at the very end */
        if (hero_broke) {
            Soundeffect(se_wall_of_force, 65);
            pline("A wall of force smashes down around you!");
        }
        dmg = d(1 + obj->spe, 6); /* normally 2d12 */
    } else if (obj->otyp == WAN_NOTHING) {
        dmg = 0;
    } else {
        dmg = d(charges, dmg_multiplier);
    }

    /* inflict damage and destroy the wand */
    if (hero_broke) {
        freeinv(obj);       /* hide it from destroy_items instead... */
        setnotworn(obj);    /* so we need to do this ourselves */
        explode(u.ux, u.uy, -(obj->otyp), dmg * 2, WAND_CLASS, expltype);
        exploding_wand_efx(obj);
        makeknown(obj->otyp); /* explode describes the effect */
        obj->in_use = FALSE;
        discard_broken_wand();
    } else {
        int otyp = obj->otyp;
        /* Useup before monster is possibly killed. */
        m_useup(mon, obj);
        explode(mon->mx, mon->my, -(otyp), dmg * 2, WAND_CLASS, expltype);

        exploding_wand_efx(obj);
        makeknown(obj->otyp); /* explode describes the effect */
    }

    /* Couple janky exceptions */
    switch (obj->otyp) {
    case WAN_NOTHING:
        if (hero_broke)
            pline("Nothing happens.");
        break;
    case WAN_OPENING:
        if (hero_broke && u.ustuck) {
            release_hold();
            if (obj->dknown)
                makeknown(WAN_OPENING);
        }
        break;
    case WAN_SECRET_DOOR_DETECTION:
        /* Detects portals: We'll use the same odds UnNetHack has for
         * creating traps for breaking the other wands. */
        if (hero_broke && (obj->spe > 2) && rn2(obj->spe - 2)) {
            trap_detect((struct obj *) 0);
            makeknown(obj->otyp);
        }
        break;
    }

    /* obscure side-effect */
    if (hero_broke)
        exercise(A_STR, FALSE);
}

/* used to collect gremlins being hit by light so that they can be processed
   after vision for the entire lit area has been brought up to date */
struct litmon {
    struct monst *mon;
    struct litmon *nxt;
};
static struct litmon *gremlins = 0;

/*
 * Low-level lit-field update routine.
 */
staticfn void
set_lit(coordxy x, coordxy y, genericptr_t val)
{
    struct monst *mtmp;
    struct litmon *gremlin;

    if (val) {
        levl[x][y].lit = 1;
        if ((mtmp = m_at(x, y)) != 0 && mtmp->data == &mons[PM_GREMLIN]) {
            gremlin = (struct litmon *) alloc(sizeof *gremlin);
            gremlin->mon = mtmp;
            gremlin->nxt = gremlins;
            gremlins = gremlin;
        }
    } else {
        levl[x][y].lit = 0;
        snuff_light_source(x, y);
    }
}

void
litroom(
    boolean on,      /* True: make nearby area lit; False: cursed scroll */
    struct obj *obj) /* scroll, spellbook (for spell), or wand of light */
{
    struct obj *otmp, *nextobj;
    boolean blessed_effect = (obj && obj->oclass == SCROLL_CLASS
                              && obj->blessed);
    boolean no_op = (u.uswallow || Underwater || Is_waterlevel(&u.uz));
    char is_lit = 0; /* value is irrelevant but assign something anyway; its
                      * address is used as a 'not null' flag for set_lit() */

    /* update object lights and produce message (provided you're not blind) */
    if (!on) {
        int still_lit = 0;

        /*
         * The magic douses lamps,&c too and might curse artifact lights.
         *
         * FIXME?
         *  Shouldn't this affect all lit objects in the area of effect
         *  rather than just those carried by the hero?
         */
        for (otmp = gi.invent; otmp; otmp = nextobj) {
            nextobj = otmp->nobj;
            if (otmp->lamplit) {
                if (!artifact_light(otmp))
                    (void) snuff_lit(otmp);
                else
                    /* wielded Sunsword or worn gold dragon scales/mail;
                       maybe lower its BUC state if not already cursed */
                    impact_arti_light(otmp, TRUE, (boolean) !Blind);

                if (otmp->lamplit)
                    ++still_lit;
            }
        }
        /* scroll of light becomes discovered when not blind, so some
           message to justify that is needed */
        if (!Blind) {
            /* for the still_lit case, we don't know at this point whether
               anything currently visibly lit is going to go dark; if this
               message came after the darkening, we could count visibly
               lit squares before and after to know; we do know that being
               swallowed won't be affected--the interior is still lit */
            if (still_lit)
                pline_The("ambient light seems dimmer.");
            else if (u.uswallow)
                pline("It seems even darker in here than before.");
            else
                You("are surrounded by darkness!");
        }
    } else { /* on */
        if (blessed_effect) {
            /* might bless artifact lights; no effect on ordinary lights */
            for (otmp = gi.invent; otmp; otmp = nextobj) {
                nextobj = otmp->nobj;
                if (otmp->lamplit && artifact_light(otmp))
                    /* wielded Sunsword or worn gold dragon scales/mail;
                       maybe raise its BUC state if not already blessed */
                    impact_arti_light(otmp, FALSE, (boolean) !Blind);
            }
        }
        if (u.uswallow) {
            if (Blind)
                ; /* no feedback */
            else if (digests(u.ustuck->data))
                pline("%s %s is lit.", s_suffix(Monnam(u.ustuck)),
                      mbodypart(u.ustuck, STOMACH));
            else if (is_whirly(u.ustuck->data))
                pline("%s shines briefly.", Monnam(u.ustuck));
            else
                pline("%s glistens.", Monnam(u.ustuck));
        } else if (!Blind && (!Is_rogue_level(&u.uz)
                              || levl[u.ux][u.uy].typ != CORR)) {
            pline("A lit field %ssurrounds you!", no_op ? "briefly " : "");
        }
    }

    /* No-op when swallowed or in water */
    if (no_op)
        return;
    /*
     *  If we are darkening the room and the hero is punished but not
     *  blind, then we have to pick up and replace the ball and chain so
     *  that we don't remember them if they are out of sight.
     */
    if (Punished && !on && !Blind)
        move_bc(1, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);

    if (Is_rogue_level(&u.uz)) {
        /* Can't use do_clear_area because MAX_RADIUS is too small */
        /* rogue lighting must light the entire room */
        int rnum = levl[u.ux][u.uy].roomno - ROOMOFFSET;
        int rx, ry;

        if (rnum >= 0) {
            for (rx = svr.rooms[rnum].lx - 1; rx <= svr.rooms[rnum].hx + 1;
                 rx++)
                for (ry = svr.rooms[rnum].ly - 1;
                     ry <= svr.rooms[rnum].hy + 1; ry++)
                    set_lit(rx, ry,
                            (genericptr_t) (on ? &is_lit : (char *) 0));
            svr.rooms[rnum].rlit = on;
        }
        /* hallways remain dark on the rogue level */
    } else if (is_art(obj, ART_SUNSWORD)) {
        /* Sunsword's #invoke power directed up or down lights hero's spot
           (do_clear_area() rejects radius 0 so call set_lit() directly) */
        set_lit(u.ux, u.uy, (genericptr_t) &is_lit);
    } else {
        int radius = 5; /* default radius */
        if (obj && obj->oclass == SCROLL_CLASS) {
            /* blessed scroll lights up entire level */
            if (obj->blessed) {
                int x, y;
                radius = -1;
                for (x = 1; x < COLNO; x++) {
                    for (y = 1; y < ROWNO; y++) {
                        set_lit(x, y, (on ? &is_lit : NULL));
                    }
                }
            } else { /* uncursed gets a much larger area than previously */
                radius = 11;
            }
        } else if (obj && obj->oclass == SPBOOK_CLASS) {
            /* Spell scales with ability; scroll is still superior. */
            if (Role_if(PM_CARTOMANCER))
                radius = 7;
            else {
                switch (P_SKILL(P_DIVINATION_SPELL)) {
                case P_ISRESTRICTED:
                case P_UNSKILLED:
                    radius = 1;
                    break;
                case P_BASIC:
                    radius = 3;
                    break;
                case P_SKILLED:
                    radius = 5;
                    break;
                case P_EXPERT:
                    radius = 7;
                    break;
                }
            }
        }

        if (radius > 0) {
            do_clear_area(u.ux, u.uy, radius, set_lit,
                          (genericptr_t) (on ? &is_lit : (char *) 0));
        }
    }

    /*
     *  If we are not blind, then force a redraw on all positions in sight
     *  by temporarily blinding the hero.  The vision recalculation will
     *  correctly update all previously seen positions *and* correctly
     *  set the waslit bit [could be messed up from above].
     */
    if (!Blind) {
        vision_recalc(2);

        /* replace ball&chain */
        if (Punished && !on)
            move_bc(0, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);
    }

    gv.vision_full_recalc = 1; /* delayed vision recalculation */
    if (gremlins) {
        struct litmon *gremlin;

        /* can't delay vision recalc after all */
        vision_recalc(0);
        /* after vision has been updated, monsters who are affected
           when hit by light can now be hit by it */
        do {
            gremlin = gremlins;
            gremlins = gremlin->nxt;
            light_hits_gremlin(gremlin->mon, rnd(5));
            free((genericptr_t) gremlin);
        } while (gremlins);
    }
    return;
}

#define REALLY 1
#define PLAYER 2
#define ONTHRONE 4
void
do_genocide(
    int how, /* 0 = no exile; create monsters (cursed scroll)
              * 1 = normal exile
              * 3 = forced exile of player
              * 5 (4 | 1) = normal exile from throne */
    boolean only_close)
{
    char buf[BUFSZ], promptbuf[QBUFSZ];
    int i, killplayer = 0;
    int mndx;
    struct permonst *ptr;
    const char *which;

    if (how & PLAYER) {
        mndx = u.umonster; /* non-polymorphed mon num */
        ptr = &mons[mndx];
        Strcpy(buf, pmname(ptr, Ugender));
        killplayer++;
    } else {
        buf[0] = '\0'; /* init for EDIT_GETLIN */
        for (i = 0; ; i++) {
            if (i >= 5) {
                /* cursed effect => no free pass (unless rndmonst() fails) */
                if (!(how & REALLY) && (ptr = rndmonst()) != 0)
                    break;

                pline1(thats_enough_tries);
                return;
            }
            Strcpy(promptbuf,
                   "What type of monster do you want to exile?");
            if (i > 0)
                Snprintf(eos(promptbuf), sizeof promptbuf - strlen(promptbuf),
                         " [enter %s]",
                         iflags.cmdassist
                           ? "the name of a type of monster, or '?'"
                           : "'?' to see previous exiles");
            getlin(promptbuf, buf);
            (void) mungspaces(buf);
            /* avoid 'such creatures do not exist' for empty input */
            if (!*buf) {
                pline("%s.", (i + 1 < 5)
                             ? "Type the name of a type of monster or 'none'"
                             /* next iteration gives "that's enough tries"
                                so don't suggest typing anything this time */
                             : "No type of monster specified");
                continue; /* try again */
            }
            /* choosing "none" preserves exileless conduct */
            if (*buf == '\033' || !strcmpi(buf, "none")
                || !strcmpi(buf, "'none'") || !strcmpi(buf, "nothing")) {
                /* ... but no free pass if cursed */
                if (!(how & REALLY) && (ptr = rndmonst()) != 0)
                    break; /* remaining checks don't apply */

                livelog_printf(LL_GENOCIDE, "declined to perform exile");
                return;
            }
            /* "?" or "'?'" runs #exiled to show existing exiles */
            if (!strcmp(buf, "?") || !strcmp(buf, "'?'")) {
                list_genocided('g', FALSE);
                --i; /* don't count this iteration as one of the tries */
                continue;
            }

#ifdef WIZARD	/* to aid in topology testing; remove pesky monsters */
            /* copy from do_class_genocide */
            if (wizard && buf[0] == '*') {
                register struct monst *mtmp, *mtmp2;

                int gonecnt = 0;
                for (mtmp = fmon; mtmp; mtmp = mtmp2) {
                    mtmp2 = mtmp->nmon;
                    if (DEADMONSTER(mtmp))
                        continue;
                    mongone(mtmp);
                    gonecnt++;
                }
                pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));
                return;
            }
#endif
            mndx = name_to_mon(buf, (int *) 0);
            if (mndx == NON_PM || (svm.mvitals[mndx].mvflags & G_GENOD)) {
                pline("Such creatures %s exist in this world.",
                      (mndx == NON_PM) ? "do not" : "no longer");
                continue;
            }
            ptr = &mons[mndx];
            /* first revert if current shifted form or base vampire form */
            if (Upolyd && vampshifted(&gy.youmonst)
                && (mndx == u.umonnum || mndx == gy.youmonst.cham))
                polyself(POLY_REVERT); /* vampshifter (bat, &c) to vampire */
            /* Although "genus" is Latin for race, the hero benefits
             * from both race and role; thus exile affects either.
             */
            if (Your_Own_Role(mndx) || Your_Own_Race(mndx)) {
                killplayer++;
                break;
            }
            if (is_human(ptr))
                adjalign(-sgn(u.ualign.type));
            if (is_demon(ptr))
                adjalign(sgn(u.ualign.type));

            if (!(ptr->geno & G_GENO)) {
                if (!Deaf) {
                    /* FIXME: unconditional "caverns" will be silly in some
                     * circumstances.  Who's speaking?  Divine pronouncements
                     * aren't supposed to be hampered by deafness....
                     */
                    if (flags.verbose)
                        pline("A thunderous voice booms"
                              " through the caverns:");
                    SetVoice((struct monst *) 0, 0, 80, voice_deity);
                    /* FIXME? shouldn't this override deafness? */
                    verbalize("No, mortal!  That will not be done.");
                }
                continue;
            }
            /* KMH -- Unchanging prevents rehumanization */
            if (Unchanging && ptr == gy.youmonst.data)
                killplayer++;
            break;
        }
        mndx = monsndx(ptr); /* needed for the 'no free pass' cases */
    }

    which = "some ";
    if (Hallucination) {
        if (Upolyd)
            Strcpy(buf, pmname(gy.youmonst.data,
                               flags.female ? FEMALE : MALE));
        else {
            Strcpy(buf, (flags.female && gu.urole.name.f) ? gu.urole.name.f
                                                       : gu.urole.name.m);
            buf[0] = lowc(buf[0]);
        }
    } else {
        Strcpy(buf, ptr->pmnames[NEUTRAL]); /* standard singular */
        if ((ptr->geno & G_UNIQ) && ptr != &mons[PM_HIGH_CLERIC])
            which = !type_is_pname(ptr) ? "the " : "";
    }
    if (how & REALLY) {
        if (!u.uconduct.exiles)
            livelog_printf(LL_CONDUCT | LL_GENOCIDE,
                           "performed %s first exile (%s)",
                           uhis(), makeplural(buf));
        livelog_printf(LL_GENOCIDE, "exiled %s in %s",
                       makeplural(buf), svd.dungeons[u.uz.dnum].dname);
        pline("Wiped out %s%s on this level.", which, makeplural(buf));

        if (killplayer) {
            u.uhp = -1;
            if (how & PLAYER) {
                svk.killer.format = KILLED_BY;
                Strcpy(svk.killer.name, "exiling confusion");
            } else if (how & ONTHRONE) {
                /* player selected while on a throne */
                svk.killer.format = KILLED_BY_AN;
                Strcpy(svk.killer.name, "imperious order");
            } else { /* selected player deliberately, not confused */
                svk.killer.format = KILLED_BY_AN;
                Strcpy(svk.killer.name, "scroll of exile");
            }

            /* Polymorphed characters will die as soon as they're rehumanized.
             */
            /* KMH -- Unchanging prevents rehumanization */
            if (Upolyd && ptr != gy.youmonst.data) {
                delayed_killer(POLYMORPH, svk.killer.format, svk.killer.name);
                You_feel("%s inside.", udeadinside());
            } else
                done(GENOCIDED);
        } else if (ptr == gy.youmonst.data) {
            rehumanize();
        }
        kill_monster_on_level(mndx, only_close);
    } else {
        int cnt = 0, census = monster_census(FALSE);

        if (!(mons[mndx].geno & G_UNIQ)
            && !(svm.mvitals[mndx].mvflags & (G_GENOD | G_EXTINCT)))
            for (i = rn1(3, 4); i > 0; i--) {
                if (!makemon(ptr, u.ux, u.uy, NO_MINVENT | MM_NOMSG))
                    break; /* couldn't make one */
                ++cnt;
                if (svm.mvitals[mndx].mvflags & G_EXTINCT)
                    break; /* just made last one */
            }
        if (cnt) {
            /* accumulated 'cnt' doesn't take groups into account;
               assume bringing in new mon(s) didn't remove any old ones */
            cnt = monster_census(FALSE) - census;
            if (!Hallucination && mons[mndx].mlet == S_TROLL) {
                pline("S3n7 1n %s!!!",
                      (cnt > 1) ? "s0m3 7r0llz" : "4 7r0ll");
            } else  {
                pline("Sent in %s%s.", (cnt > 1) ? "some " : "",
                    (cnt > 1) ? makeplural(buf) : an(buf));
            }
        } else
            pline1(nothing_happens);
    }
}

void
punish(struct obj *sobj)
{
    /* angrygods() calls this with NULL sobj arg */
    struct obj *reuse_ball = (sobj && sobj->otyp == HEAVY_IRON_BALL)
                                ? sobj : (struct obj *) 0;
    /* analyzer doesn't know that the one caller that passes a NULL
     * sobj (angrygods) checks !Punished first, so add a guard */
    int cursed_levy = (sobj && sobj->cursed) ? 1 : 0;

    /* KMH -- Punishment is still okay when you are riding */
    if (!reuse_ball)
        You("are being punished for your misbehavior!");
    if (Punished) {
        Your("iron ball gets heavier.");
        uball->owt += WT_IRON_BALL_INCR * (1 + cursed_levy);
        return;
    }
    if (amorphous(gy.youmonst.data) || is_whirly(gy.youmonst.data)
        || unsolid(gy.youmonst.data)) {
        if (!reuse_ball) {
            pline("A ball and chain appears, then falls away.");
            dropy(mkobj(BALL_CLASS, TRUE));
        } else {
            dropy(reuse_ball);
        }
        return;
    }
    setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN);
    if (!reuse_ball)
        setworn(mkobj(BALL_CLASS, TRUE), W_BALL);
    else
        setworn(reuse_ball, W_BALL);

    /*
     *  Place ball & chain if not swallowed.  If swallowed, the ball & chain
     *  variables will be set at the next call to placebc().
     */
    if (!u.uswallow) {
        placebc();
        if (Blind)
            set_bc(1);      /* set up ball and chain variables */
        newsym(u.ux, u.uy); /* see ball&chain if can't see self */
    }
}

/* remove the ball and chain */
void
unpunish(void)
{
    struct obj *savechain = uchain;

    /* chain goes away */
    setworn((struct obj *) 0, W_CHAIN); /* sets 'uchain' to Null */
    /* for floor, unhides monster hidden under chain, calls newsym() */
    delobj(savechain);

    /* the chain is gone but the no longer attached ball persists */
    setworn((struct obj *) 0, W_BALL); /* sets 'uball' to Null */
}

/* prompt the player to create a stinking cloud and then create it if they
   give a location */
staticfn void
do_stinking_cloud(struct obj *sobj, boolean mention_stinking)
{
    coord cc;

    pline("Where do you want to center the %scloud?",
          mention_stinking ? "stinking " : "");
    cc.x = u.ux;
    cc.y = u.uy;
    getpos_sethilite(display_stinking_cloud_positions, can_center_cloud);
    if (getpos(&cc, TRUE, "the desired position") < 0) {
        pline1(Never_mind);
        return;
    } else if (!can_center_cloud(cc.x, cc.y)) {
        if (Hallucination)
            pline("Ugh... someone cut the cheese.");
        else
            pline("%s a whiff of rotten eggs.",
                  sobj->oclass == SCROLL_CLASS ? "The scroll crumbles with"
                                               : "You smell");
        return;
    }
    (void) create_gas_cloud(cc.x, cc.y, 15 + 10 * bcsign(sobj),
                            8 + 4 * bcsign(sobj));
}

/* some creatures have special data structures that only make sense in their
 * normal locations -- if the player tries to create one elsewhere, or to
 * revive one, the disoriented creature becomes a zombie
 */
boolean
cant_revive(
    int *mtype,
    boolean revival,
    struct obj *from_obj)
{
    /* SHOPKEEPERS can be revived now */
    if (*mtype == PM_GUARD || (*mtype == PM_SHOPKEEPER && !revival)
        || *mtype == PM_HIGH_CLERIC || *mtype == PM_ALIGNED_CLERIC
        || *mtype == PM_ANGEL) {
        *mtype = PM_HUMAN_ZOMBIE;
        return TRUE;
    } else if (*mtype == PM_LONG_WORM_TAIL) { /* for create_particular() */
        *mtype = PM_LONG_WORM;
        return TRUE;
    } else if (unique_corpstat(&mons[*mtype])
               && (!from_obj || !has_omonst(from_obj))) {
        /* unique corpses (from bones or wizard mode wish) or
           statues (bones or any wish) end up as shapechangers */
        *mtype = PM_DOPPELGANGER;
        return TRUE;
    }
    return FALSE;
}

staticfn boolean
create_particular_parse(
    char *str,
    struct _create_particular_data *d)
{
    int gender_name_var = NEUTRAL;
    char *bufp = str;
    char *tmpp;

    d->quan = 1 + ((gm.multi > 0) ? (int) gm.multi : 0);
    d->monclass = MAXMCLASSES;
    d->which = gu.urole.mnum; /* an arbitrary index into mons[] */
    d->fem = -1;     /* gender not specified */
    d->genderconf = -1;  /* no confusion on which gender to assign */
    d->randmonst = FALSE;
    d->maketame = d->makepeaceful = d->makehostile = d->makesummon = FALSE;
    d->sleeping = d->saddled = d->invisible = FALSE;
    d->rabid = d->diseased = FALSE;
    /* quantity */
    if (digit(*bufp)) {
        d->quan = atoi(bufp);
        while (digit(*bufp))
            bufp++;
        while (*bufp == ' ')
            bufp++;
    }
#define QUAN_LIMIT (ROWNO * (COLNO - 1))
    /* maximum possible quantity is one per cell: (0..ROWNO-1) x (1..COLNO-1)
       [21*79==1659 for default map size; could subtract 1 for hero's spot] */
    if (d->quan < 1 || d->quan > QUAN_LIMIT)
        d->quan = QUAN_LIMIT - monster_census(FALSE);
#undef QUAN_LIMIT
    /* gear -- extremely limited number of possibilities supported */
    if ((tmpp = strstri(bufp, "saddled ")) != 0) {
        d->saddled = TRUE;
        (void) memset(tmpp, ' ', sizeof "saddled " - 1);
    }
    /* state -- limited number of possibilities supported */
    if ((tmpp = strstri(bufp, "sleeping ")) != 0) {
        d->sleeping = TRUE;
        (void) memset(tmpp, ' ', sizeof "sleeping " - 1);
    }
    if ((tmpp = strstri(bufp, "invisible ")) != 0) {
        d->invisible = TRUE;
        (void) memset(tmpp, ' ', sizeof "invisible " - 1);
    }
    if ((tmpp = strstri(bufp, "hidden ")) != 0) {
        d->hidden = TRUE;
        (void) memset(tmpp, ' ', sizeof "hidden " - 1);
    }
    /* check "female" before "male" to avoid false hit mid-word */
    if ((tmpp = strstri(bufp, "female ")) != 0) {
        d->fem = 1;
        (void) memset(tmpp, ' ', sizeof "female " - 1);
    }
    if ((tmpp = strstri(bufp, "male ")) != 0) {
        d->fem = 0;
        (void) memset(tmpp, ' ', sizeof "male " - 1);
    }
    bufp = mungspaces(bufp); /* after potential memset(' ') */
    /* allow the initial disposition to be specified */
    if (!strncmpi(bufp, "tame ", 5)) {
        bufp += 5;
        d->maketame = TRUE;
    } else if (!strncmpi(bufp, "peaceful ", 9)) {
        bufp += 9;
        d->makepeaceful = TRUE;
    } else if (!strncmpi(bufp, "hostile ", 8)) {
        bufp += 8;
        d->makehostile = TRUE;
    }

    if (!strncmpi(bufp, "summoned ", 9)) {
        bufp += 9;
        d->makesummon = TRUE;
    }

    if (!strncmpi(bufp, "rabid ", 6)) {
        bufp += 6;
        d->rabid = TRUE;
    }

    if (!strncmpi(bufp, "diseased ", 9)) {
        bufp += 9;
        d->diseased = TRUE;
    }
    /* decide whether a valid monster was chosen */
    if (wizard && (!strcmp(bufp, "*") || !strcmp(bufp, "random"))) {
        d->randmonst = TRUE;
        return TRUE;
    }
    d->which = name_to_mon(bufp, &gender_name_var);
    /*
     * With the introduction of male and female monster names
     * in 3.7, preserve that detail.
     *
     * If d->fem is already set to MALE or FEMALE at this juncture, it means
     * one of those terms was explicitly specified.
     */
    if (d->fem == MALE || d->fem == FEMALE) {     /* explicitly expressed */
        if ((gender_name_var != NEUTRAL) && (d->fem != gender_name_var)) {
            /* apparent selection incompatibility */
            d->genderconf = gender_name_var;        /* resolve later */
        }
        /* otherwise keep the value of d->fem, as it's okay */
    } else {  /* no explicit gender term was specified */
        d->fem = gender_name_var;
    }
    if (ismnum(d->which))
        return TRUE; /* got one */
    d->monclass = name_to_monclass(bufp, &d->which);

    if (ismnum(d->which)) {
        d->monclass = MAXMCLASSES; /* matters below */
        return TRUE;
    } else if (d->monclass == S_invisible) { /* not an actual monster class */
        d->which = PM_STALKER;
        d->monclass = MAXMCLASSES;
        return TRUE;
    } else if (d->monclass == S_WORM_TAIL) { /* empty monster class */
        d->which = PM_LONG_WORM;
        d->monclass = MAXMCLASSES;
        return TRUE;
    } else if (d->monclass > 0) {
        d->which = gu.urole.mnum; /* reset from NON_PM */
        return TRUE;
    }
    return FALSE;
}

staticfn boolean
create_particular_creation(
    struct _create_particular_data *d)
{
    struct permonst *whichpm = NULL;
    int i, mx, my, firstchoice = NON_PM;
    struct monst *mtmp;
    boolean madeany = FALSE;

    if (!d->randmonst) {
        firstchoice = d->which;
        if (cant_revive(&d->which, FALSE, (struct obj *) 0)
            && firstchoice != PM_LONG_WORM_TAIL) {
            /* wizard mode can override handling of special monsters */
            char buf[BUFSZ];

            Sprintf(buf, "Creating %s instead; force %s?",
                    mons[d->which].pmnames[NEUTRAL],
                    mons[firstchoice].pmnames[NEUTRAL]);
            if (y_n(buf) == 'y')
                d->which = firstchoice;
        }
        whichpm = &mons[d->which];
    }
    for (i = 0; i < d->quan; i++) {
        mmflags_nht mmflags = NO_MM_FLAGS;

        if (d->monclass != MAXMCLASSES)
            whichpm = mkclass(d->monclass, 0);
        else if (d->randmonst)
            whichpm = rndmonst();
        if (d->genderconf == -1) {
            /* no conflict exists between explicit gender term and
               the specified monster name */
            if (d->fem != -1 && (!whichpm || (!is_male(whichpm)
                                              && !is_female(whichpm))))
                mmflags |= (d->fem == FEMALE) ? MM_FEMALE
                               : (d->fem == MALE) ? MM_MALE : 0;
            /* no surprise; "<mon> appears." rather than "<mon> appears!" */
            mmflags |= MM_NOEXCLAM;
        } else {
            /* conundrum alert: an explicit gender term conflicts with an
               explicit gender-tied naming term (i.e. male cavewoman) */

            /* option not gone with: name overrides the explicit gender as
               commented out here */
            /*  d->fem = d->genderconf; */

            /* option chosen: let the explicit gender term (male or female)
               override the gender-tied naming term, so leave d->fem as-is */

            mmflags |= (d->fem == FEMALE) ? MM_FEMALE
                           : (d->fem == MALE) ? MM_MALE : 0;

            /* another option would be to consider it a faulty specification
               and reject the request completely and produce a random monster
               with a gender matching that specified instead (i.e. there is
               no such thing as a male cavewoman) */
            /* whichpm = rndmonst(); */
            /* mmflags |= (d->fem == FEMALE) ? MM_FEMALE : MM_MALE; */
        }
        if (d->invisible)
            mmflags |= MM_MINVIS;
        if (d->sleeping)
            mmflags |= MM_ASLEEP;
        mtmp = makemon(whichpm, u.ux, u.uy, mmflags);
        if (!mtmp) {
            /* quit trying if creation failed and is going to repeat */
            if (d->monclass == MAXMCLASSES && !d->randmonst)
                break;
            /* otherwise try again */
            continue;
        }
        mx = mtmp->mx, my = mtmp->my;
        if (d->maketame) {
            (void) tamedog(mtmp, (struct obj *) 0, FALSE);
        } else if (d->makepeaceful || d->makehostile) {
            mtmp->mtame = 0; /* sanity precaution */
            mtmp->mpeaceful = d->makepeaceful ? 1 : 0;
            set_malign(mtmp);
            if (iflags.wc_underline_peacefuls)
                /* mpeaceful is only set here, so the previous calls to newsym
                 * will make it look like the monster isn't peaceful. */
                newsym(mtmp->mx, mtmp->my);
        }

        if (d->makesummon)
            mtmp->msummoned = 15; /* Arbitrary, for testing */

        if (d->saddled && can_saddle(mtmp) && !which_armor(mtmp, W_SADDLE)) {
            /* NULL obj arg means put_saddle_on_mon()
             * will create the saddle itself */
            put_saddle_on_mon((struct obj *) 0, mtmp);
        }
        if (d->hidden
           /* can't hide on a closed door (amorphous green slimes, etc) */
           && !closed_door(mtmp->mx, mtmp->my)
           && ((is_hider(mtmp->data) && mtmp->data->mlet != S_MIMIC)
               || (hides_under(mtmp->data) && concealed_spot(mx, my))
               || (mtmp->data->mlet == S_EEL && is_damp_terrain(mx, my))))
            mtmp->mundetected = 1;
        if (d->sleeping)
            mtmp->msleeping = 1;
        if (d->diseased
            && !(resists_sick(mtmp->data) || defended(mtmp, AD_DISE))) {
            mtmp->mdiseased = 1;
            mtmp->mdiseasetime = rn1(9, 6);
        }
        if (d->rabid && can_become_rabid(mtmp->data)) {
            if (mtmp->mpeaceful) {
                mtmp->mpeaceful = 0; /* Doesn't co-exist with rabid */
                newsym(mtmp->mx, mtmp->my);
            }
            if (mtmp->mtame) {
                mtmp->mtame = 0; /* Doesn't co-exist with rabid */
                newsym(mtmp->mx, mtmp->my);
            }
            mtmp->mrabid = 1;
        }
        /* if asking for 'hidden', show location of every created monster
           that can't be seen--whether that's due to successfully hiding
           or vision issues (line-of-sight, invisibility, blindness) */
        if ((d->hidden || d->invisible) && !canspotmon(mtmp))
            flash_mon(mtmp);

        madeany = TRUE;
        /* in case we got a doppelganger instead of what was asked
           for, make it start out looking like what was asked for */
        if (mtmp->cham != NON_PM && firstchoice != NON_PM
            && mtmp->cham != firstchoice)
            (void) newcham(mtmp, &mons[firstchoice], NO_NC_FLAGS);
    }
    return madeany;
}

/*
 * Make a new monster with the type controlled by the user.
 *
 * Note:  when creating a monster by class letter, specifying the
 * "strange object" (']') symbol produces a random monster rather
 * than a mimic.  This behavior quirk is useful so don't "fix" it
 * (use 'm'--or "mimic"--to create a random mimic).
 *
 * Used in wizard mode only (for ^G command and for scroll or spell
 * of create monster).  Once upon a time, an earlier incarnation of
 * this code was also used for the scroll/spell in explore mode.
 */
boolean
create_particular(void)
{
#define CP_TRYLIM 5
    struct _create_particular_data d;
    char *bufp, buf[BUFSZ], prompt[QBUFSZ];
    int  tryct = CP_TRYLIM, altmsg = 0;

    buf[0] = '\0'; /* for EDIT_GETLIN */
    Strcpy(prompt, "Create what kind of monster?");
    do {
        getlin(prompt, buf);
        bufp = mungspaces(buf);
        if (*bufp == '\033')
            return FALSE;

        if (create_particular_parse(bufp, &d))
            break;

        /* no good; try again... */
        if (*bufp || altmsg || tryct < 2) {
            pline("I've never heard of such monsters.");
        } else {
            pline("Try again (type * for random, ESC to cancel).");
            ++altmsg;
        }
        /* when a second try is needed, expand the prompt */
        if (tryct == CP_TRYLIM)
            Strcat(prompt, " [type name or symbol]");
    } while (--tryct > 0);

    if (!tryct)
        pline1(thats_enough_tries);
    else
        return create_particular_creation(&d);

    return FALSE;
}

boolean
create_particular_from_buffer(char* bufp)
{
    struct _create_particular_data d;

    if (create_particular_parse(bufp, &d))
        return create_particular_creation(&d);

    return FALSE;
}

/* This handles a scroll of create monster that is keyed to a
 * specific monster. Used when playing as a cartomancer.
 * Caller uses up card. */
void
use_moncard(
    struct obj *sobj,
    int x, int y)
{
    boolean yours = !svc.context.mon_moving;
    struct permonst *pm = sobj->corpsenm == NON_PM
            ? rndmonst() : &mons[sobj->corpsenm];

    (void) make_msummoned(pm, &gy.youmonst,
                          sobj->cursed ? FALSE : TRUE, x, y);
    if (yours)
        u.uen -= CARD_COST;
}

staticfn void
specified_id(void)
{
    static char buf[BUFSZ] = DUMMY;
    char promptbuf[BUFSZ];
    char bufcpy[BUFSZ];
    short otyp;
    int tries = 0;

    promptbuf[0] = '\0';
    if (flags.verbose)
        You("may learn about any non-artifact.");

retry:
    Strcpy(promptbuf, "What non-artifact do you want to learn about");
    Strcat(promptbuf, "?");
    getlin(promptbuf, buf);
    (void) mungspaces(buf);
    if (buf[0] == '\033') {
        buf[0] = '\0';
    }

    strcpy(bufcpy, buf);
    otyp = name_to_otyp(buf);
    if (otyp == STRANGE_OBJECT) {
        pline("No specific object of that name exists.");
        if (++tries < 5)
            goto retry;
        pline1(thats_enough_tries);
        if (!otyp)
            return; /* for safety; should never happen */
    }
    if (objects[otyp].oc_name_known) {
        You("already know what that object looks like.");
        if (++tries < 5)
            goto retry;
        pline1(thats_enough_tries);
        if (!otyp)
            return;
    }
    (void) makeknown(otyp);
    You("now know more about %s.", makeplural(simple_typename(otyp)));
    update_inventory();
}

static const int extra_classes[] = {
    WEAPON_CLASS,   ARMOR_CLASS,    TOOL_CLASS,
    GEM_CLASS,      SCROLL_CLASS,   SPBOOK_CLASS,
    POTION_CLASS,   RING_CLASS,     AMULET_CLASS,
    WAND_CLASS,
};

int
learnme(void)
{
    struct obj *otmp;
    int tries = 0, oclass, otyp;
    /* Identify a random magical item. */
    do {
        oclass = ROLL_FROM(extra_classes);
        otmp = mkobj(oclass, FALSE);
        otyp = otmp->otyp;
        if (objects[otyp].oc_magic && !objects[otyp].oc_name_known) {
            obfree(otmp, NULL); /* Destroy the item*/
            makeknown(otyp);
            return otyp;
        }
        obfree(otmp, NULL); /* Destroy the item*/
        tries++;
    } while (tries < 1000);

    return 0;
}


/* guarantees that worn cloak is scales, and that
 * otmp = uarmc, but does NOT guarantee existence of uarm
 *
 * no body armor under the scales = the scales are enchanted directly
 * onto you (no such thing as a scaled shirt). The wearer will
 * polymorph. Also caused by a confused scroll, _after_ the scales meld.
 */
boolean
maybe_merge_scales(struct obj *sobj, struct obj *otmp)
{
    boolean confused = (Confusion != 0);
    boolean poly_after_merge = (!uarm || confused);

    int old_light = artifact_light(otmp) ? arti_light_radius(otmp) : 0;
    if (uarm) {
        struct obj *scales = uarmc;
        struct obj *armor = uarm;

        pline("%s melds into your %s%s", Yname2(scales),
              suit_simple_name(armor),
              Is_dragon_scaled_armor(armor) ? "." : "!");

        if (Is_dragon_scaled_armor(armor)) {
            if (Dragon_armor_to_scales(armor) == scales->otyp) {
                /* scales match armor already; just use up scales */
                pline("Its scales still seem %s.",
                      dragon_scales_color(armor));
            } else {
                /* armor is already scaled but the new scales are
                 * different and will replace the old ones */
                pline("Its scales change from %s to %s!",
                      dragon_scales_color(armor),
                      dragon_scales_color(scales));
                /* remove properties of old scales */
                dragon_armor_handling(armor, FALSE, TRUE);
            }
        }
        setnotworn(armor);
        armor->dragonscales = scales->otyp;
        armor->cursed = 0;
        if (sobj->blessed) {
            armor->oeroded = armor->oeroded2 = 0;
            armor->blessed = 1;
        }
        setworn(armor, W_ARM);
        dragon_armor_handling(armor, TRUE, TRUE);
        gk.known = TRUE;
        if (otmp->unpaid)
            alter_cost(otmp, 0L); /* shop bill */

        /* handle gold dragon scaled armor... */
        if (scales->lamplit) {
            if (armor->lamplit) {
                /* if melding gold dragon scales onto already gold-scaled
                 * armor, avoid attaching a duplicate light source to the
                 * armor
                 * useup() won't take care of this, because it calls
                 * setnotworn(), which will make artifact_light() return
                 * false, so the regular check for deleting the light source
                 * when an object is deallocated will do nothing */
                del_light_source(LS_OBJECT, obj_to_any(scales));
            }
            else {
                /* this will set armor->lamplit */
                obj_move_light_source(scales, armor);
            }
            /* may be different radius depending on BUC of armor */
            maybe_adjust_light(armor, old_light);
        }
        else if (armor->lamplit) {
            /* scales not lit but armor is: melding non-gold scales onto
             * gold-scaled armor, which will no longer be a light source */
            end_burn(armor, FALSE);
        }
        useup(scales);
    }
    if (poly_after_merge) {
        polyself(POLY_DRAGON_ONLY);
        /* adjust duration for scroll beatitude - a blessed scroll will
         * give you more time as a dragon, a cursed scroll less */
        u.mtimedone = (u.mtimedone * (bcsign(sobj) + 2) / 2);
    }
    if (!sobj->cursed || !uarm) {
        return FALSE;
    } else {
        /* continue with regular cursed-enchant logic on the resulting
         * armor piece */
        otmp = uarm;
    }
    return TRUE;
}


/*read.c*/
