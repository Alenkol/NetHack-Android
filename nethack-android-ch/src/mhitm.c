﻿/* NetHack 3.6	mhitm.c	$NHDT-Date: 1446854229 2015/11/06 23:57:09 $  $NHDT-Branch: master $:$NHDT-Revision: 1.83 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

extern boolean notonhead;

static NEARDATA boolean vis, far_noise;
static NEARDATA long noisetime;
static NEARDATA struct obj *otmp;

static const char brief_feeling[] =
    "片刻有一种%s的感觉, 然后消失了.";

STATIC_DCL char *FDECL(mon_nam_too, (char *, struct monst *, struct monst *));
STATIC_DCL int FDECL(hitmm, (struct monst *, struct monst *,
                             struct attack *));
STATIC_DCL int FDECL(gazemm, (struct monst *, struct monst *,
                              struct attack *));
STATIC_DCL int FDECL(gulpmm, (struct monst *, struct monst *,
                              struct attack *));
STATIC_DCL int FDECL(explmm, (struct monst *, struct monst *,
                              struct attack *));
STATIC_DCL int FDECL(mdamagem, (struct monst *, struct monst *,
                                struct attack *));
STATIC_DCL void FDECL(mswingsm, (struct monst *, struct monst *,
                                 struct obj *));
STATIC_DCL void FDECL(noises, (struct monst *, struct attack *));
STATIC_DCL void FDECL(missmm, (struct monst *, struct monst *,
                               struct attack *));
STATIC_DCL int FDECL(passivemm, (struct monst *, struct monst *,
                                 BOOLEAN_P, int));

/* Needed for the special case of monsters wielding vorpal blades (rare).
 * If we use this a lot it should probably be a parameter to mdamagem()
 * instead of a global variable.
 */
static int dieroll;

/* returns mon_nam(mon) relative to other_mon; normal name unless they're
   the same, in which case the reference is to {him|her|it} self */
STATIC_OVL char *
mon_nam_too(outbuf, mon, other_mon)
char *outbuf;
struct monst *mon, *other_mon;
{
    Strcpy(outbuf, mon_nam(mon));
    if (mon == other_mon)
        switch (pronoun_gender(mon)) {
        case 0:
            Strcpy(outbuf, "他自己");
            break;
        case 1:
            Strcpy(outbuf, "她自己");
            break;
        default:
            Strcpy(outbuf, "它自己");
            break;
        }
    return outbuf;
}

STATIC_OVL void
noises(magr, mattk)
register struct monst *magr;
register struct attack *mattk;
{
    boolean farq = (distu(magr->mx, magr->my) > 15);

    if (!Deaf && (farq != far_noise || moves - noisetime > 10)) {
        far_noise = farq;
        noisetime = moves;
        You_hear("%s%s.",
                 farq ? "远处的" : "",
                 (mattk->aatyp == AT_EXPL) ? "爆炸声" : "一些声响");
    }
}

STATIC_OVL
void
missmm(magr, mdef, mattk)
register struct monst *magr, *mdef;
struct attack *mattk;
{
    const char *fmt;
    char buf[BUFSZ], mdef_name[BUFSZ];

    if (vis) {
        if (!canspotmon(magr))
            map_invisible(magr->mx, magr->my);
        if (!canspotmon(mdef))
            map_invisible(mdef->mx, mdef->my);
        if (mdef->m_ap_type)
            seemimic(mdef);
        if (magr->m_ap_type)
            seemimic(magr);
        fmt = (could_seduce(magr, mdef, mattk) && !magr->mcan)
                  ? "%s 假装友好地对"
                  : "%s 没打中";
        Sprintf(buf, fmt, Monnam(magr));
        pline("%s %s.", buf, mon_nam_too(mdef_name, mdef, magr));
    } else
        noises(magr, mattk);
}

/*
 *  fightm()  -- fight some other monster
 *
 *  Returns:
 *      0 - Monster did nothing.
 *      1 - If the monster made an attack.  The monster might have died.
 *
 *  There is an exception to the above.  If mtmp has the hero swallowed,
 *  then we report that the monster did nothing so it will continue to
 *  digest the hero.
 */
 /* have monsters fight each other */
int
fightm(mtmp)
register struct monst *mtmp;
{
    register struct monst *mon, *nmon;
    int result, has_u_swallowed;
#ifdef LINT
    nmon = 0;
#endif
    /* perhaps the monster will resist Conflict */
    if (resist(mtmp, RING_CLASS, 0, 0))
        return 0;

    if (u.ustuck == mtmp) {
        /* perhaps we're holding it... */
        if (itsstuck(mtmp))
            return 0;
    }
    has_u_swallowed = (u.uswallow && (mtmp == u.ustuck));

    for (mon = fmon; mon; mon = nmon) {
        nmon = mon->nmon;
        if (nmon == mtmp)
            nmon = mtmp->nmon;
        /* Be careful to ignore monsters that are already dead, since we
         * might be calling this before we've cleaned them up.  This can
         * happen if the monster attacked a cockatrice bare-handedly, for
         * instance.
         */
        if (mon != mtmp && !DEADMONSTER(mon)) {
            if (monnear(mtmp, mon->mx, mon->my)) {
                if (!u.uswallow && (mtmp == u.ustuck)) {
                    if (!rn2(4)) {
                        pline("%s 放出了你!", Monnam(mtmp));
                        u.ustuck = 0;
                    } else
                        break;
                }

                /* mtmp can be killed */
                bhitpos.x = mon->mx;
                bhitpos.y = mon->my;
                notonhead = 0;
                result = mattackm(mtmp, mon);

                if (result & MM_AGR_DIED)
                    return 1; /* mtmp died */
                /*
                 * If mtmp has the hero swallowed, lie and say there
                 * was no attack (this allows mtmp to digest the hero).
                 */
                if (has_u_swallowed)
                    return 0;

                /* Allow attacked monsters a chance to hit back. Primarily
                 * to allow monsters that resist conflict to respond.
                 */
                if ((result & MM_HIT) && !(result & MM_DEF_DIED) && rn2(4)
                    && mon->movement >= NORMAL_SPEED) {
                    mon->movement -= NORMAL_SPEED;
                    notonhead = 0;
                    (void) mattackm(mon, mtmp); /* return attack */
                }

                return (result & MM_HIT) ? 1 : 0;
            }
        }
    }
    return 0;
}

/*
 * mdisplacem() -- attacker moves defender out of the way;
 *                 returns same results as mattackm().
 */
int
mdisplacem(magr, mdef, quietly)
register struct monst *magr, *mdef;
boolean quietly;
{
    struct permonst *pa, *pd;
    int tx, ty, fx, fy;

    /* sanity checks; could matter if we unexpectedly get a long worm */
    if (!magr || !mdef || magr == mdef)
        return MM_MISS;
    pa = magr->data, pd = mdef->data;
    tx = mdef->mx, ty = mdef->my; /* destination */
    fx = magr->mx, fy = magr->my; /* current location */
    if (m_at(fx, fy) != magr || m_at(tx, ty) != mdef)
        return MM_MISS;

    /* The 1 in 7 failure below matches the chance in attack()
     * for pet displacement.
     */
    if (!rn2(7))
        return MM_MISS;

    /* Grid bugs cannot displace at an angle. */
    if (pa == &mons[PM_GRID_BUG] && magr->mx != mdef->mx
        && magr->my != mdef->my)
        return MM_MISS;

    /* undetected monster becomes un-hidden if it is displaced */
    if (mdef->mundetected)
        mdef->mundetected = 0;
    if (mdef->m_ap_type && mdef->m_ap_type != M_AP_MONSTER)
        seemimic(mdef);
    /* wake up the displaced defender */
    mdef->msleeping = 0;
    mdef->mstrategy &= ~STRAT_WAITMASK;
    finish_meating(mdef);

    /*
     * Set up the visibility of action.
     * You can observe monster displacement if you can see both of
     * the monsters involved.
     */
    vis = (canspotmon(magr) && canspotmon(mdef));

    if (touch_petrifies(pd) && !resists_ston(magr)) {
        if (which_armor(magr, W_ARMG) != 0) {
            if (poly_when_stoned(pa)) {
                mon_to_stone(magr);
                return MM_HIT; /* no damage during the polymorph */
            }
            if (!quietly && canspotmon(magr))
                pline("%s 变成了石头!", Monnam(magr));
            monstone(magr);
            if (magr->mhp > 0)
                return MM_HIT; /* lifesaved */
            else if (magr->mtame && !vis)
                You(brief_feeling, "特别难过");
            return MM_AGR_DIED;
        }
    }

    remove_monster(fx, fy); /* pick up from orig position */
    remove_monster(tx, ty);
    place_monster(magr, tx, ty); /* put down at target spot */
    place_monster(mdef, fx, fy);
    if (vis && !quietly)
        pline("%s 让 %s 让出了道路!", Monnam(magr), mon_nam(mdef));
    newsym(fx, fy);  /* see it */
    newsym(tx, ty);  /*   all happen */
    flush_screen(0); /* make sure it shows up */

    return MM_HIT;
}

/*
 * mattackm() -- a monster attacks another monster.
 *
 *          --------- aggressor died
 *         /  ------- defender died
 *        /  /  ----- defender was hit
 *       /  /  /
 *      x  x  x
 *
 *      0x4     MM_AGR_DIED
 *      0x2     MM_DEF_DIED
 *      0x1     MM_HIT
 *      0x0     MM_MISS
 *
 * Each successive attack has a lower probability of hitting.  Some rely on
 * success of previous attacks.  ** this doen't seem to be implemented -dl **
 *
 * In the case of exploding monsters, the monster dies as well.
 */
int
mattackm(magr, mdef)
register struct monst *magr, *mdef;
{
    int i,          /* loop counter */
        tmp,        /* amour class difference */
        strike,     /* hit this attack */
        attk,       /* attack attempted this time */
        struck = 0, /* hit at least once */
        res[NATTK]; /* results of all attacks */
    struct attack *mattk, alt_attk;
    struct permonst *pa, *pd;

    if (!magr || !mdef)
        return MM_MISS; /* mike@genat */
    if (!magr->mcanmove || magr->msleeping)
        return MM_MISS;
    pa = magr->data;
    pd = mdef->data;

    /* Grid bugs cannot attack at an angle. */
    if (pa == &mons[PM_GRID_BUG] && magr->mx != mdef->mx
        && magr->my != mdef->my)
        return MM_MISS;

    /* Calculate the armour class differential. */
    tmp = find_mac(mdef) + magr->m_lev;
    if (mdef->mconf || !mdef->mcanmove || mdef->msleeping) {
        tmp += 4;
        mdef->msleeping = 0;
    }

    /* undetect monsters become un-hidden if they are attacked */
    if (mdef->mundetected) {
        mdef->mundetected = 0;
        newsym(mdef->mx, mdef->my);
        if (canseemon(mdef) && !sensemon(mdef)) {
            if (Unaware)
                You("梦见 %s.", (mdef->data->geno & G_UNIQ)
                                        ? a_monnam(mdef)
                                        : makeplural(m_monnam(mdef)));
            else
                pline("突然, 你注意到 %s.", a_monnam(mdef));
        }
    }

    /* Elves hate orcs. */
    if (is_elf(pa) && is_orc(pd))
        tmp++;

    /* Set up the visibility of action */
    vis = (cansee(magr->mx, magr->my) && cansee(mdef->mx, mdef->my)
           && (canspotmon(magr) || canspotmon(mdef)));

    /* Set flag indicating monster has moved this turn.  Necessary since a
     * monster might get an attack out of sequence (i.e. before its move) in
     * some cases, in which case this still counts as its move for the round
     * and it shouldn't move again.
     */
    magr->mlstmv = monstermoves;

    /* Now perform all attacks for the monster. */
    for (i = 0; i < NATTK; i++) {
        res[i] = MM_MISS;
        mattk = getmattk(pa, i, res, &alt_attk);
        otmp = (struct obj *) 0;
        attk = 1;
        switch (mattk->aatyp) {
        case AT_WEAP: /* "hand to hand" attacks */
            if (magr->weapon_check == NEED_WEAPON || !MON_WEP(magr)) {
                magr->weapon_check = NEED_HTH_WEAPON;
                if (mon_wield_item(magr) != 0)
                    return 0;
            }
            possibly_unwield(magr, FALSE);
            otmp = MON_WEP(magr);

            if (otmp) {
                if (vis)
                    mswingsm(magr, mdef, otmp);
                tmp += hitval(otmp, mdef);
            }
            /*FALLTHRU*/
        case AT_CLAW:
        case AT_KICK:
        case AT_BITE:
        case AT_STNG:
        case AT_TUCH:
        case AT_BUTT:
        case AT_TENT:
            /* Nymph that teleported away on first attack? */
            if (distmin(magr->mx, magr->my, mdef->mx, mdef->my) > 1)
                return MM_MISS;
            /* Monsters won't attack cockatrices physically if they
             * have a weapon instead.  This instinct doesn't work for
             * players, or under conflict or confusion.
             */
            if (!magr->mconf && !Conflict && otmp && mattk->aatyp != AT_WEAP
                && touch_petrifies(mdef->data)) {
                strike = 0;
                break;
            }
            dieroll = rnd(20 + i);
            strike = (tmp > dieroll);
            /* KMH -- don't accumulate to-hit bonuses */
            if (otmp)
                tmp -= hitval(otmp, mdef);
            if (strike) {
                res[i] = hitmm(magr, mdef, mattk);
                if ((mdef->data == &mons[PM_BLACK_PUDDING]
                     || mdef->data == &mons[PM_BROWN_PUDDING]) && otmp
                    && objects[otmp->otyp].oc_material == IRON
                    && mdef->mhp > 1
                    && !mdef->mcan) {
                    if (clone_mon(mdef, 0, 0)) {
                        if (vis) {
                            char buf[BUFSZ];

                            Strcpy(buf, Monnam(mdef));
                            pline("%s 在%s打它时分开了!", buf,
                                  mon_nam(magr));
                        }
                    }
                }
            } else
                missmm(magr, mdef, mattk);
            break;

        case AT_HUGS: /* automatic if prev two attacks succeed */
            strike = (i >= 2 && res[i - 1] == MM_HIT && res[i - 2] == MM_HIT);
            if (strike)
                res[i] = hitmm(magr, mdef, mattk);

            break;

        case AT_GAZE:
            strike = 0;
            res[i] = gazemm(magr, mdef, mattk);
            break;

        case AT_EXPL:
            res[i] = explmm(magr, mdef, mattk);
            if (res[i] == MM_MISS) { /* cancelled--no attack */
                strike = 0;
                attk = 0;
            } else
                strike = 1; /* automatic hit */
            break;

        case AT_ENGL:
            if (u.usteed && (mdef == u.usteed)) {
                strike = 0;
                break;
            }
            /* Engulfing attacks are directed at the hero if
             * possible. -dlc
             */
            if (u.uswallow && magr == u.ustuck)
                strike = 0;
            else {
                if ((strike = (tmp > rnd(20 + i))))
                    res[i] = gulpmm(magr, mdef, mattk);
                else
                    missmm(magr, mdef, mattk);
            }
            break;

        default: /* no attack */
            strike = 0;
            attk = 0;
            break;
        }

        if (attk && !(res[i] & MM_AGR_DIED))
            res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED);

        if (res[i] & MM_DEF_DIED)
            return res[i];

        if (res[i] & MM_AGR_DIED)
            return res[i];
        /* return if aggressor can no longer attack */
        if (!magr->mcanmove || magr->msleeping)
            return res[i];
        if (res[i] & MM_HIT)
            struck = 1; /* at least one hit */
    }

    return (struck ? MM_HIT : MM_MISS);
}

/* Returns the result of mdamagem(). */
STATIC_OVL int
hitmm(magr, mdef, mattk)
register struct monst *magr, *mdef;
struct attack *mattk;
{
    if (vis) {
        int compat;
        char buf[BUFSZ], mdef_name[BUFSZ];

        if (!canspotmon(magr))
            map_invisible(magr->mx, magr->my);
        if (!canspotmon(mdef))
            map_invisible(mdef->mx, mdef->my);
        if (mdef->m_ap_type)
            seemimic(mdef);
        if (magr->m_ap_type)
            seemimic(magr);
        if ((compat = could_seduce(magr, mdef, mattk)) && !magr->mcan) {
            Sprintf(buf, "%s %s对%s", Monnam(magr),
                    compat == 2 ? "动人地" : "诱惑地",
                    mon_nam(mdef));
            pline("%s %s.", buf, mdef->mcansee ? "微笑" : "说话");
        } else {
            char magr_name[BUFSZ];

            Strcpy(magr_name, Monnam(magr));
            switch (mattk->aatyp) {
            case AT_BITE:
                Sprintf(buf, "%s 咬了一口", magr_name);
                break;
            case AT_STNG:
                Sprintf(buf, "%s 叮了一口", magr_name);
                break;
            case AT_BUTT:
                Sprintf(buf, "%s 撞了一下", magr_name);
                break;
            case AT_TUCH:
                Sprintf(buf, "%s 碰了一下", magr_name);
                break;
            case AT_TENT:
                Sprintf(buf, "%s 触手吸食", s_suffix(magr_name));
                break;
            case AT_HUGS:
                if (magr != u.ustuck) {
                    Sprintf(buf, "%s 挤压", magr_name);
                    break;
                }
            default:
                Sprintf(buf, "%s 打了一下", magr_name);
            }
            pline("%s %s.", buf, mon_nam_too(mdef_name, mdef, magr));
        }
    } else
        noises(magr, mattk);

    return mdamagem(magr, mdef, mattk);
}

/* Returns the same values as mdamagem(). */
STATIC_OVL int
gazemm(magr, mdef, mattk)
register struct monst *magr, *mdef;
struct attack *mattk;
{
    char buf[BUFSZ];

    if (vis) {
        Sprintf(buf, "%s 凝视着", Monnam(magr));
        pline("%s %s...", buf, mon_nam(mdef));
    }

    if (magr->mcan || !magr->mcansee
        || (magr->minvis && !perceives(mdef->data)) || !mdef->mcansee
        || mdef->msleeping) {
        if (vis)
            pline("但是无事发生.");
        return MM_MISS;
    }
    /* call mon_reflects 2x, first test, then, if visible, print message */
    if (magr->data == &mons[PM_MEDUSA] && mon_reflects(mdef, (char *) 0)) {
        if (canseemon(mdef))
            (void) mon_reflects(mdef, "凝视被 %s %s反射开.");
        if (mdef->mcansee) {
            if (mon_reflects(magr, (char *) 0)) {
                if (canseemon(magr))
                    (void) mon_reflects(
                        magr, "凝视被 %s %s反射开.");
                return MM_MISS;
            }
            if (mdef->minvis && !perceives(magr->data)) {
                if (canseemon(magr)) {
                    pline(
                      "%s 似乎没有注意到%s 凝视被反射了.",
                          Monnam(magr), mhis(magr));
                }
                return MM_MISS;
            }
            if (canseemon(magr))
                pline("%s 变成了石头!", Monnam(magr));
            monstone(magr);
            if (magr->mhp > 0)
                return MM_MISS;
            return MM_AGR_DIED;
        }
    }

    return mdamagem(magr, mdef, mattk);
}

/* return True if magr is allowed to swallow mdef, False otherwise */
boolean
engulf_target(magr, mdef)
struct monst *magr, *mdef;
{
    struct rm *lev;
    int dx, dy;

    /* can't swallow something that's too big */
    if (mdef->data->msize >= MZ_HUGE)
        return FALSE;

    /* (hypothetical) engulfers who can pass through walls aren't
     limited by rock|trees|bars */
    if ((magr == &youmonst) ? Passes_walls : passes_walls(magr->data))
        return TRUE;

    /* don't swallow something in a spot where attacker wouldn't
       otherwise be able to move onto; we don't want to engulf
       a wall-phaser and end up with a non-phaser inside a wall */
    dx = mdef->mx, dy = mdef->my;
    if (mdef == &youmonst)
        dx = u.ux, dy = u.uy;
    lev = &levl[dx][dy];
    if (IS_ROCK(lev->typ) || closed_door(dx, dy) || IS_TREE(lev->typ)
        /* not passes_bars(); engulfer isn't squeezing through */
        || (lev->typ == IRONBARS && !is_whirly(magr->data)))
        return FALSE;

    return TRUE;
}

/* Returns the same values as mattackm(). */
STATIC_OVL int
gulpmm(magr, mdef, mattk)
register struct monst *magr, *mdef;
register struct attack *mattk;
{
    xchar ax, ay, dx, dy;
    int status;
    char buf[BUFSZ];
    struct obj *obj;

    if (!engulf_target(magr, mdef))
        return MM_MISS;

    if (vis) {
        Sprintf(buf, "%s 吞下了", Monnam(magr));
        pline("%s %s.", buf, mon_nam(mdef));
    }
    for (obj = mdef->minvent; obj; obj = obj->nobj)
        (void) snuff_lit(obj);

    /*
     *  All of this manipulation is needed to keep the display correct.
     *  There is a flush at the next pline().
     */
    ax = magr->mx;
    ay = magr->my;
    dx = mdef->mx;
    dy = mdef->my;
    /*
     *  Leave the defender in the monster chain at it's current position,
     *  but don't leave it on the screen.  Move the aggressor to the
     *  defender's position.
     */
    remove_monster(ax, ay);
    place_monster(magr, dx, dy);
    newsym(ax, ay); /* erase old position */
    newsym(dx, dy); /* update new position */

    status = mdamagem(magr, mdef, mattk);

    if ((status & (MM_AGR_DIED | MM_DEF_DIED))
        == (MM_AGR_DIED | MM_DEF_DIED)) {
        ;                              /* both died -- do nothing  */
    } else if (status & MM_DEF_DIED) { /* defender died */
        /*
         *  Note:  remove_monster() was called in relmon(), wiping out
         *  magr from level.monsters[mdef->mx][mdef->my].  We need to
         *  put it back and display it.  -kd
         */
        place_monster(magr, dx, dy);
        newsym(dx, dy);
        /* aggressor moves to <dx,dy> and might encounter trouble there */
        if (minliquid(magr) || (t_at(dx, dy) && mintrap(magr) == 2))
            status |= MM_AGR_DIED;
    } else if (status & MM_AGR_DIED) { /* aggressor died */
        place_monster(mdef, dx, dy);
        newsym(dx, dy);
    } else { /* both alive, put them back */
        if (cansee(dx, dy))
            pline("%s 反胃的!", Monnam(mdef));

        place_monster(magr, ax, ay);
        place_monster(mdef, dx, dy);
        newsym(ax, ay);
        newsym(dx, dy);
    }

    return status;
}

STATIC_OVL int
explmm(magr, mdef, mattk)
struct monst *magr, *mdef;
struct attack *mattk;
{
    int result;

    if (magr->mcan)
        return MM_MISS;

    if (cansee(magr->mx, magr->my))
        pline("%s 爆炸了!", Monnam(magr));
    else
        noises(magr, mattk);

    result = mdamagem(magr, mdef, mattk);

    /* Kill off aggressor if it didn't die. */
    if (!(result & MM_AGR_DIED)) {
        mondead(magr);
        if (magr->mhp > 0)
            return result; /* life saved */
        result |= MM_AGR_DIED;
    }
    if (magr->mtame) /* give this one even if it was visible */
        You(brief_feeling, "忧郁");

    return result;
}

/*
 *  See comment at top of mattackm(), for return values.
 */
STATIC_OVL int
mdamagem(magr, mdef, mattk)
register struct monst *magr, *mdef;
register struct attack *mattk;
{
    struct obj *obj;
    char buf[BUFSZ];
    struct permonst *pa = magr->data, *pd = mdef->data;
    int armpro, num, tmp = d((int) mattk->damn, (int) mattk->damd),
                     res = MM_MISS;
    boolean cancelled;

    if ((touch_petrifies(pd) /* or flesh_petrifies() */
         || (mattk->adtyp == AD_DGST && pd == &mons[PM_MEDUSA]))
        && !resists_ston(magr)) {
        long protector = attk_protection((int) mattk->aatyp),
             wornitems = magr->misc_worn_check;

        /* wielded weapon gives same protection as gloves here */
        if (otmp != 0)
            wornitems |= W_ARMG;

        if (protector == 0L
            || (protector != ~0L && (wornitems & protector) != protector)) {
            if (poly_when_stoned(pa)) {
                mon_to_stone(magr);
                return MM_HIT; /* no damage during the polymorph */
            }
            if (vis)
                pline("%s 变成了石头!", Monnam(magr));
            monstone(magr);
            if (magr->mhp > 0)
                return MM_HIT; /* lifesaved */
            else if (magr->mtame && !vis)
                You(brief_feeling, "特别难过");
            return MM_AGR_DIED;
        }
    }

    /* cancellation factor is the same as when attacking the hero */
    armpro = magic_negation(mdef);
    cancelled = magr->mcan || !(rn2(10) >= 3 * armpro);

    switch (mattk->adtyp) {
    case AD_DGST:
        /* eating a Rider or its corpse is fatal */
        if (is_rider(pd)) {
            if (vis)
                pline("%s %s!", Monnam(magr),
                      (pd == &mons[PM_FAMINE])
                          ? "无力地打嗝, 枯萎然后死了"
                          : (pd == &mons[PM_PESTILENCE])
                                ? "断续性地咳嗽然后垮掉了"
                                : "剧烈地呕吐然后倒毙了");
            mondied(magr);
            if (magr->mhp > 0)
                return 0; /* lifesaved */
            else if (magr->mtame && !vis)
                You(brief_feeling, "恶心");
            return MM_AGR_DIED;
        }
        if (flags.verbose && !Deaf)
            verbalize("嗝!");
        tmp = mdef->mhp;
        /* Use up amulet of life saving */
        if (!!(obj = mlifesaver(mdef)))
            m_useup(mdef, obj);

        /* Is a corpse for nutrition possible?  It may kill magr */
        if (!corpse_chance(mdef, magr, TRUE) || magr->mhp < 1)
            break;

        /* Pets get nutrition from swallowing monster whole.
         * No nutrition from G_NOCORPSE monster, eg, undead.
         * DGST monsters don't die from undead corpses
         */
        num = monsndx(pd);
        if (magr->mtame && !magr->isminion
            && !(mvitals[num].mvflags & G_NOCORPSE)) {
            struct obj *virtualcorpse = mksobj(CORPSE, FALSE, FALSE);
            int nutrit;

            set_corpsenm(virtualcorpse, num);
            nutrit = dog_nutrition(magr, virtualcorpse);
            dealloc_obj(virtualcorpse);

            /* only 50% nutrition, 25% of normal eating time */
            if (magr->meating > 1)
                magr->meating = (magr->meating + 3) / 4;
            if (nutrit > 1)
                nutrit /= 2;
            EDOG(magr)->hungrytime += nutrit;
        }
        break;
    case AD_STUN:
        if (magr->mcan)
            break;
        if (canseemon(mdef))
            pline("%s %s了片刻.", Monnam(mdef),
                  makeplural(stagger(pd, "蹒跚")));
        mdef->mstun = 1;
        goto physical;
    case AD_LEGS:
        if (magr->mcan) {
            tmp = 0;
            break;
        }
        goto physical;
    case AD_WERE:
    case AD_HEAL:
    case AD_PHYS:
    physical:
        if (mattk->aatyp == AT_KICK && thick_skinned(pd)) {
            tmp = 0;
        } else if (mattk->aatyp == AT_WEAP) {
            if (otmp) {
                if (otmp->otyp == CORPSE
                    && touch_petrifies(&mons[otmp->corpsenm]))
                    goto do_stone;
                tmp += dmgval(otmp, mdef);
                if (otmp->oartifact) {
                    (void) artifact_hit(magr, mdef, otmp, &tmp, dieroll);
                    if (mdef->mhp <= 0)
                        return (MM_DEF_DIED
                                | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
                }
                if (tmp)
                    rustm(mdef, otmp);
            }
        } else if (pa == &mons[PM_PURPLE_WORM] && pd == &mons[PM_SHRIEKER]) {
            /* hack to enhance mm_aggression(); we don't want purple
               worm's bite attack to kill a shrieker because then it
               won't swallow the corpse; but if the target survives,
               the subsequent engulf attack should accomplish that */
            if (tmp >= mdef->mhp && mdef->mhp > 1)
                tmp = mdef->mhp - 1;
        }
        break;
    case AD_FIRE:
        if (cancelled) {
            tmp = 0;
            break;
        }
        if (vis)
            pline("%s %s!", Monnam(mdef), on_fire(pd, mattk));
        if (pd == &mons[PM_STRAW_GOLEM] || pd == &mons[PM_PAPER_GOLEM]) {
            if (vis)
                pline("%s 完全被灼烧了!", Monnam(mdef));
            mondied(mdef);
            if (mdef->mhp > 0)
                return 0;
            else if (mdef->mtame && !vis)
                pline("愿 %s 在平静中烘烤.", mon_nam(mdef));
            return (MM_DEF_DIED | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
        }
        tmp += destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
        tmp += destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
        if (resists_fire(mdef)) {
            if (vis)
                pline_The("或似乎没有灼伤 %s!", mon_nam(mdef));
            shieldeff(mdef->mx, mdef->my);
            golemeffects(mdef, AD_FIRE, tmp);
            tmp = 0;
        }
        /* only potions damage resistant players in destroy_item */
        tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
        break;
    case AD_COLD:
        if (cancelled) {
            tmp = 0;
            break;
        }
        if (vis)
            pline("%s 被冰霜覆盖了!", Monnam(mdef));
        if (resists_cold(mdef)) {
            if (vis)
                pline_The("冰霜似乎没有使 %s寒冷!", mon_nam(mdef));
            shieldeff(mdef->mx, mdef->my);
            golemeffects(mdef, AD_COLD, tmp);
            tmp = 0;
        }
        tmp += destroy_mitem(mdef, POTION_CLASS, AD_COLD);
        break;
    case AD_ELEC:
        if (cancelled) {
            tmp = 0;
            break;
        }
        if (vis)
            pline("%s 被电了!", Monnam(mdef));
        tmp += destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
        if (resists_elec(mdef)) {
            if (vis)
                pline_The("电没有冲击 %s!", mon_nam(mdef));
            shieldeff(mdef->mx, mdef->my);
            golemeffects(mdef, AD_ELEC, tmp);
            tmp = 0;
        }
        /* only rings damage resistant players in destroy_item */
        tmp += destroy_mitem(mdef, RING_CLASS, AD_ELEC);
        break;
    case AD_ACID:
        if (magr->mcan) {
            tmp = 0;
            break;
        }
        if (resists_acid(mdef)) {
            if (vis)
                pline("%s 被酸覆盖了, 但是似乎是无害的.",
                      Monnam(mdef));
            tmp = 0;
        } else if (vis) {
            pline("%s 被酸覆盖了!", Monnam(mdef));
            pline("灼伤了 %s!", mon_nam(mdef));
        }
        if (!rn2(30))
            erode_armor(mdef, ERODE_CORRODE);
        if (!rn2(6))
            acid_damage(MON_WEP(mdef));
        break;
    case AD_RUST:
        if (magr->mcan)
            break;
        if (pd == &mons[PM_IRON_GOLEM]) {
            if (vis)
                pline("%s 碎掉了!", Monnam(mdef));
            mondied(mdef);
            if (mdef->mhp > 0)
                return 0;
            else if (mdef->mtame && !vis)
                pline("愿 %s 在平静中生锈.", mon_nam(mdef));
            return (MM_DEF_DIED | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
        }
        erode_armor(mdef, ERODE_RUST);
        mdef->mstrategy &= ~STRAT_WAITFORU;
        tmp = 0;
        break;
    case AD_CORR:
        if (magr->mcan)
            break;
        erode_armor(mdef, ERODE_CORRODE);
        mdef->mstrategy &= ~STRAT_WAITFORU;
        tmp = 0;
        break;
    case AD_DCAY:
        if (magr->mcan)
            break;
        if (pd == &mons[PM_WOOD_GOLEM] || pd == &mons[PM_LEATHER_GOLEM]) {
            if (vis)
                pline("%s 碎掉了!", Monnam(mdef));
            mondied(mdef);
            if (mdef->mhp > 0)
                return 0;
            else if (mdef->mtame && !vis)
                pline("愿 %s 在平静中腐烂.", mon_nam(mdef));
            return (MM_DEF_DIED | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
        }
        erode_armor(mdef, ERODE_CORRODE);
        mdef->mstrategy &= ~STRAT_WAITFORU;
        tmp = 0;
        break;
    case AD_STON:
        if (magr->mcan)
            break;
    do_stone:
        /* may die from the acid if it eats a stone-curing corpse */
        if (munstone(mdef, FALSE))
            goto post_stone;
        if (poly_when_stoned(pd)) {
            mon_to_stone(mdef);
            tmp = 0;
            break;
        }
        if (!resists_ston(mdef)) {
            if (vis)
                pline("%s 变成了石头!", Monnam(mdef));
            monstone(mdef);
        post_stone:
            if (mdef->mhp > 0)
                return 0;
            else if (mdef->mtame && !vis)
                You(brief_feeling, "特别难过");
            return (MM_DEF_DIED | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
        }
        tmp = (mattk->adtyp == AD_STON ? 0 : 1);
        break;
    case AD_TLPT:
        if (!cancelled && tmp < mdef->mhp && !tele_restrict(mdef)) {
            char mdef_Monnam[BUFSZ];
            /* save the name before monster teleports, otherwise
               we'll get "it" in the suddenly disappears message */
            if (vis)
                Strcpy(mdef_Monnam, Monnam(mdef));
            mdef->mstrategy &= ~STRAT_WAITFORU;
            (void) rloc(mdef, TRUE);
            if (vis && !canspotmon(mdef) && mdef != u.usteed)
                pline("%s 突然消失了!", mdef_Monnam);
        }
        break;
    case AD_SLEE:
        if (!cancelled && !mdef->msleeping
            && sleep_monst(mdef, rnd(10), -1)) {
            if (vis) {
                Strcpy(buf, Monnam(mdef));
                pline("%s 被 %s陷入了沉睡.", buf, mon_nam(magr));
            }
            mdef->mstrategy &= ~STRAT_WAITFORU;
            slept_monst(mdef);
        }
        break;
    case AD_PLYS:
        if (!cancelled && mdef->mcanmove) {
            if (vis) {
                Strcpy(buf, Monnam(mdef));
                pline("%s 被%s所冰冻.", buf, mon_nam(magr));
            }
            paralyze_monst(mdef, rnd(10));
        }
        break;
    case AD_SLOW:
        if (!cancelled && mdef->mspeed != MSLOW) {
            unsigned int oldspeed = mdef->mspeed;

            mon_adjust_speed(mdef, -1, (struct obj *) 0);
            mdef->mstrategy &= ~STRAT_WAITFORU;
            if (mdef->mspeed != oldspeed && vis)
                pline("%s 慢下来了.", Monnam(mdef));
        }
        break;
    case AD_CONF:
        /* Since confusing another monster doesn't have a real time
         * limit, setting spec_used would not really be right (though
         * we still should check for it).
         */
        if (!magr->mcan && !mdef->mconf && !magr->mspec_used) {
            if (vis)
                pline("%s 看起来混乱的.", Monnam(mdef));
            mdef->mconf = 1;
            mdef->mstrategy &= ~STRAT_WAITFORU;
        }
        break;
    case AD_BLND:
        if (can_blnd(magr, mdef, mattk->aatyp, (struct obj *) 0)) {
            register unsigned rnd_tmp;

            if (vis && mdef->mcansee)
                pline("%s 被导致失明了.", Monnam(mdef));
            rnd_tmp = d((int) mattk->damn, (int) mattk->damd);
            if ((rnd_tmp += mdef->mblinded) > 127)
                rnd_tmp = 127;
            mdef->mblinded = rnd_tmp;
            mdef->mcansee = 0;
            mdef->mstrategy &= ~STRAT_WAITFORU;
        }
        tmp = 0;
        break;
    case AD_HALU:
        if (!magr->mcan && haseyes(pd) && mdef->mcansee) {
            if (vis)
                pline("%s 看起来 %s混乱的.", Monnam(mdef),
                      mdef->mconf ? "更 " : "");
            mdef->mconf = 1;
            mdef->mstrategy &= ~STRAT_WAITFORU;
        }
        tmp = 0;
        break;
    case AD_CURS:
        if (!night() && (pa == &mons[PM_GREMLIN]))
            break;
        if (!magr->mcan && !rn2(10)) {
            mdef->mcan = 1; /* cancelled regardless of lifesave */
            mdef->mstrategy &= ~STRAT_WAITFORU;
            if (is_were(pd) && pd->mlet != S_HUMAN)
                were_change(mdef);
            if (pd == &mons[PM_CLAY_GOLEM]) {
                if (vis) {
                    pline("一些文字从 %s 头上消失了!",
                          s_suffix(mon_nam(mdef)));
                    pline("%s 被消灭了!", Monnam(mdef));
                }
                mondied(mdef);
                if (mdef->mhp > 0)
                    return 0;
                else if (mdef->mtame && !vis)
                    You(brief_feeling, "异常的悲伤");
                return (MM_DEF_DIED
                        | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
            }
            if (!Deaf) {
                if (!vis)
                    You_hear("笑声.");
                else
                    pline("%s 咯咯地笑.", Monnam(magr));
            }
        }
        break;
    case AD_SGLD:
        tmp = 0;
        if (magr->mcan)
            break;
        /* technically incorrect; no check for stealing gold from
         * between mdef's feet...
         */
        {
            struct obj *gold = findgold(mdef->minvent);
            if (!gold)
                break;
            obj_extract_self(gold);
            add_to_minv(magr, gold);
        }
        mdef->mstrategy &= ~STRAT_WAITFORU;
        if (vis) {
            Strcpy(buf, Monnam(magr));
            pline("%s 从 %s偷走一些金币.", buf, mon_nam(mdef));
        }
        if (!tele_restrict(magr)) {
            (void) rloc(magr, TRUE);
            if (vis && !canspotmon(magr))
                pline("%s 突然消失了!", buf);
        }
        break;
    case AD_DRLI:
        if (!cancelled && !rn2(3) && !resists_drli(mdef)) {
            tmp = d(2, 6);
            if (vis)
                pline("%s 突然似乎虚弱了一些!", Monnam(mdef));
            mdef->mhpmax -= tmp;
            if (mdef->m_lev == 0)
                tmp = mdef->mhp;
            else
                mdef->m_lev--;
            /* Automatic kill if drained past level 0 */
        }
        break;
    case AD_SSEX:
    case AD_SITM: /* for now these are the same */
    case AD_SEDU:
        if (magr->mcan)
            break;
        /* find an object to steal, non-cursed if magr is tame */
        for (obj = mdef->minvent; obj; obj = obj->nobj)
            if (!magr->mtame || !obj->cursed)
                break;

        if (obj) {
            char onambuf[BUFSZ], mdefnambuf[BUFSZ];

            /* make a special x_monnam() call that never omits
               the saddle, and save it for later messages */
            Strcpy(mdefnambuf,
                   x_monnam(mdef, ARTICLE_THE, (char *) 0, 0, FALSE));

            otmp = obj;
            if (u.usteed == mdef && otmp == which_armor(mdef, W_SADDLE))
                /* "You can no longer ride <steed>." */
                dismount_steed(DISMOUNT_POLY);
            obj_extract_self(otmp);
            if (otmp->owornmask) {
                mdef->misc_worn_check &= ~otmp->owornmask;
                if (otmp->owornmask & W_WEP)
                    mwepgone(mdef);
                otmp->owornmask = 0L;
                update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
            }
            /* add_to_minv() might free otmp [if it merges] */
            if (vis)
                Strcpy(onambuf, doname(otmp));
            (void) add_to_minv(magr, otmp);
            if (vis) {
                Strcpy(buf, Monnam(magr));
                pline("%s 从 %s 偷走了 %s!", buf, mdefnambuf, onambuf);
            }
            possibly_unwield(mdef, FALSE);
            mdef->mstrategy &= ~STRAT_WAITFORU;
            mselftouch(mdef, (const char *) 0, FALSE);
            if (mdef->mhp <= 0)
                return (MM_DEF_DIED
                        | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
            if (pa->mlet == S_NYMPH && !tele_restrict(magr)) {
                (void) rloc(magr, TRUE);
                if (vis && !canspotmon(magr))
                    pline("%s 突然消失了!", buf);
            }
        }
        tmp = 0;
        break;
    case AD_DREN:
        if (!cancelled && !rn2(4))
            xdrainenergym(mdef, vis && mattk->aatyp != AT_ENGL);
        tmp = 0;
        break;
    case AD_DRST:
    case AD_DRDX:
    case AD_DRCO:
        if (!cancelled && !rn2(8)) {
            if (vis)
                pline("%s %s 是有毒的!", s_suffix(Monnam(magr)),
                      mpoisons_subj(magr, mattk));
            if (resists_poison(mdef)) {
                if (vis)
                    pline_The("毒似乎没有影响 %s.",
                              mon_nam(mdef));
            } else {
                if (rn2(10))
                    tmp += rn1(10, 6);
                else {
                    if (vis)
                        pline_The("毒时致命的...");
                    tmp = mdef->mhp;
                }
            }
        }
        break;
    case AD_DRIN:
        if (notonhead || !has_head(pd)) {
            if (vis)
                pline("%s 似乎没有受到伤害.", Monnam(mdef));
            /* Not clear what to do for green slimes */
            tmp = 0;
            break;
        }
        if ((mdef->misc_worn_check & W_ARMH) && rn2(8)) {
            if (vis) {
                Strcpy(buf, s_suffix(Monnam(mdef)));
                pline("%s 头盔阻碍了 %s 对 %s 头的攻击.", buf,
                      mon_nam(magr), mhis(mdef));
            }
            break;
        }
        res = eat_brains(magr, mdef, vis, &tmp);
        break;
    case AD_SLIM:
        if (cancelled)
            break; /* physical damage only */
        if (!rn2(4) && !slimeproof(pd)) {
            if (!munslime(mdef, FALSE) && mdef->mhp > 0) {
                if (newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, vis))
                    pd = mdef->data;
                mdef->mstrategy &= ~STRAT_WAITFORU;
                res = MM_HIT;
            }
            /* munslime attempt could have been fatal,
               potentially to multiple monsters (SCR_FIRE) */
            if (magr->mhp < 1)
                res |= MM_AGR_DIED;
            if (mdef->mhp < 1)
                res |= MM_DEF_DIED;
            tmp = 0;
        }
        break;
    case AD_STCK:
        if (cancelled)
            tmp = 0;
        break;
    case AD_WRAP: /* monsters cannot grab one another, it's too hard */
        if (magr->mcan)
            tmp = 0;
        break;
    case AD_ENCH:
        /* there's no msomearmor() function, so just do damage */
        /* if (cancelled) break; */
        break;
    default:
        tmp = 0;
        break;
    }
    if (!tmp)
        return res;

    if ((mdef->mhp -= tmp) < 1) {
        if (m_at(mdef->mx, mdef->my) == magr) { /* see gulpmm() */
            remove_monster(mdef->mx, mdef->my);
            mdef->mhp = 1; /* otherwise place_monster will complain */
            place_monster(mdef, mdef->mx, mdef->my);
            mdef->mhp = 0;
        }
        monkilled(mdef, "", (int) mattk->adtyp);
        if (mdef->mhp > 0)
            return res; /* mdef lifesaved */
        else if (res == MM_AGR_DIED)
            return (MM_DEF_DIED | MM_AGR_DIED);

        if (mattk->adtyp == AD_DGST) {
            /* various checks similar to dog_eat and meatobj.
             * after monkilled() to provide better message ordering */
            if (mdef->cham >= LOW_PM) {
                (void) newcham(magr, (struct permonst *) 0, FALSE, TRUE);
            } else if (pd == &mons[PM_GREEN_SLIME] && !slimeproof(pa)) {
                (void) newcham(magr, &mons[PM_GREEN_SLIME], FALSE, TRUE);
            } else if (pd == &mons[PM_WRAITH]) {
                (void) grow_up(magr, (struct monst *) 0);
                /* don't grow up twice */
                return (MM_DEF_DIED | (magr->mhp > 0 ? 0 : MM_AGR_DIED));
            } else if (pd == &mons[PM_NURSE]) {
                magr->mhp = magr->mhpmax;
            }
        }
        /* caveat: above digestion handling doesn't keep `pa' up to date */

        return (MM_DEF_DIED | (grow_up(magr, mdef) ? 0 : MM_AGR_DIED));
    }
    return (res == MM_AGR_DIED) ? MM_AGR_DIED : MM_HIT;
}

void
paralyze_monst(mon, amt)
struct monst *mon;
int amt;
{
    if (amt > 127)
        amt = 127;

    mon->mcanmove = 0;
    mon->mfrozen = amt;
    mon->meating = 0; /* terminate any meal-in-progress */
    mon->mstrategy &= ~STRAT_WAITFORU;
}

/* `mon' is hit by a sleep attack; return 1 if it's affected, 0 otherwise */
int
sleep_monst(mon, amt, how)
struct monst *mon;
int amt, how;
{
    if (resists_sleep(mon)
        || (how >= 0 && resist(mon, (char) how, 0, NOTELL))) {
        shieldeff(mon->mx, mon->my);
    } else if (mon->mcanmove) {
        finish_meating(mon); /* terminate any meal-in-progress */
        amt += (int) mon->mfrozen;
        if (amt > 0) { /* sleep for N turns */
            mon->mcanmove = 0;
            mon->mfrozen = min(amt, 127);
        } else { /* sleep until awakened */
            mon->msleeping = 1;
        }
        return 1;
    }
    return 0;
}

/* sleeping grabber releases, engulfer doesn't; don't use for paralysis! */
void
slept_monst(mon)
struct monst *mon;
{
    if ((mon->msleeping || !mon->mcanmove) && mon == u.ustuck
        && !sticks(youmonst.data) && !u.uswallow) {
        pline("%s 控制放松了.", s_suffix(Monnam(mon)));
        unstuck(mon);
    }
}

void
rustm(mdef, obj)
struct monst *mdef;
struct obj *obj;
{
    int dmgtyp;

    if (!mdef || !obj)
        return; /* just in case */
    /* AD_ACID is handled in passivemm */
    if (dmgtype(mdef->data, AD_CORR))
        dmgtyp = ERODE_CORRODE;
    else if (dmgtype(mdef->data, AD_RUST))
        dmgtyp = ERODE_RUST;
    else if (dmgtype(mdef->data, AD_FIRE))
        dmgtyp = ERODE_BURN;
    else
        return;
    (void) erode_obj(obj, NULL, dmgtyp, EF_GREASE | EF_VERBOSE);
}

STATIC_OVL void
mswingsm(magr, mdef, otemp)
struct monst *magr, *mdef;
struct obj *otemp;
{
    if (flags.verbose && !Blind && mon_visible(magr)) {
        pline("%s 对%s %s%s %s %s.", Monnam(magr),
              mon_nam(mdef),
              (objects[otemp->otyp].oc_dir & PIERCE) ? "戳刺着" : "挥舞着",
              (otemp->quan > 1L) ? "一个" : "", mhis(magr), xname(otemp));
    }
}

/*
 * Passive responses by defenders.  Does not replicate responses already
 * handled above.  Returns same values as mattackm.
 */
STATIC_OVL int
passivemm(magr, mdef, mhit, mdead)
register struct monst *magr, *mdef;
boolean mhit;
int mdead;
{
    register struct permonst *mddat = mdef->data;
    register struct permonst *madat = magr->data;
    char buf[BUFSZ];
    int i, tmp;

    for (i = 0;; i++) {
        if (i >= NATTK)
            return (mdead | mhit); /* no passive attacks */
        if (mddat->mattk[i].aatyp == AT_NONE)
            break;
    }
    if (mddat->mattk[i].damn)
        tmp = d((int) mddat->mattk[i].damn, (int) mddat->mattk[i].damd);
    else if (mddat->mattk[i].damd)
        tmp = d((int) mddat->mlevel + 1, (int) mddat->mattk[i].damd);
    else
        tmp = 0;

    /* These affect the enemy even if defender killed */
    switch (mddat->mattk[i].adtyp) {
    case AD_ACID:
        if (mhit && !rn2(2)) {
            Strcpy(buf, Monnam(magr));
            if (canseemon(magr))
                pline("%s 被 %s 酸溅到了!", buf,
                      s_suffix(mon_nam(mdef)));
            if (resists_acid(magr)) {
                if (canseemon(magr))
                    pline("%s 没有受影响.", Monnam(magr));
                tmp = 0;
            }
        } else
            tmp = 0;
        if (!rn2(30))
            erode_armor(magr, ERODE_CORRODE);
        if (!rn2(6))
            acid_damage(MON_WEP(magr));
        goto assess_dmg;
    case AD_ENCH: /* KMH -- remove enchantment (disenchanter) */
        if (mhit && !mdef->mcan && otmp) {
            (void) drain_item(otmp);
            /* No message */
        }
        break;
    default:
        break;
    }
    if (mdead || mdef->mcan)
        return (mdead | mhit);

    /* These affect the enemy only if defender is still alive */
    if (rn2(3))
        switch (mddat->mattk[i].adtyp) {
        case AD_PLYS: /* Floating eye */
            if (tmp > 127)
                tmp = 127;
            if (mddat == &mons[PM_FLOATING_EYE]) {
                if (!rn2(4))
                    tmp = 127;
                if (magr->mcansee && haseyes(madat) && mdef->mcansee
                    && (perceives(madat) || !mdef->minvis)) {
                    Sprintf(buf, "%s 凝视被 %%s %%s反射.",
                            s_suffix(Monnam(mdef)));
                    if (mon_reflects(magr,
                                     canseemon(magr) ? buf : (char *) 0))
                        return (mdead | mhit);
                    Strcpy(buf, Monnam(magr));
                    if (canseemon(magr))
                        pline("%s 被 %s 凝视所冰冻!", buf,
                              s_suffix(mon_nam(mdef)));
                    paralyze_monst(magr, tmp);
                    return (mdead | mhit);
                }
            } else { /* gelatinous cube */
                Strcpy(buf, Monnam(magr));
                if (canseemon(magr))
                    pline("%s 被 %s所冰冻.", buf, mon_nam(mdef));
                paralyze_monst(magr, tmp);
                return (mdead | mhit);
            }
            return 1;
        case AD_COLD:
            if (resists_cold(magr)) {
                if (canseemon(magr)) {
                    pline("%s 轻微的寒冷.", Monnam(magr));
                    golemeffects(magr, AD_COLD, tmp);
                }
                tmp = 0;
                break;
            }
            if (canseemon(magr))
                pline("%s 突然非常冷!", Monnam(magr));
            mdef->mhp += tmp / 2;
            if (mdef->mhpmax < mdef->mhp)
                mdef->mhpmax = mdef->mhp;
            if (mdef->mhpmax > ((int) (mdef->m_lev + 1) * 8))
                (void) split_mon(mdef, magr);
            break;
        case AD_STUN:
            if (!magr->mstun) {
                magr->mstun = 1;
                if (canseemon(magr))
                    pline("%s %s...", Monnam(magr),
                          makeplural(stagger(magr->data, "蹒跚")));
            }
            tmp = 0;
            break;
        case AD_FIRE:
            if (resists_fire(magr)) {
                if (canseemon(magr)) {
                    pline("%s 轻微的温暖.", Monnam(magr));
                    golemeffects(magr, AD_FIRE, tmp);
                }
                tmp = 0;
                break;
            }
            if (canseemon(magr))
                pline("%s 突然非常热!", Monnam(magr));
            break;
        case AD_ELEC:
            if (resists_elec(magr)) {
                if (canseemon(magr)) {
                    pline("%s 轻微的刺痛.", Monnam(magr));
                    golemeffects(magr, AD_ELEC, tmp);
                }
                tmp = 0;
                break;
            }
            if (canseemon(magr))
                pline("%s 被电所冲击了!", Monnam(magr));
            break;
        default:
            tmp = 0;
            break;
        }
    else
        tmp = 0;

assess_dmg:
    if ((magr->mhp -= tmp) <= 0) {
        monkilled(magr, "", (int) mddat->mattk[i].adtyp);
        return (mdead | mhit | MM_AGR_DIED);
    }
    return (mdead | mhit);
}

/* hero or monster has successfully hit target mon with drain energy attack */
void
xdrainenergym(mon, givemsg)
struct monst *mon;
boolean givemsg;
{
    if (mon->mspec_used < 20 /* limit draining */
        && (attacktype(mon->data, AT_MAGC)
            || attacktype(mon->data, AT_BREA))) {
        mon->mspec_used += d(2, 2);
        if (givemsg)
            pline("%s 似乎无精打采.", Monnam(mon));
    }
}

/* "aggressive defense"; what type of armor prevents specified attack
   from touching its target? */
long
attk_protection(aatyp)
int aatyp;
{
    long w_mask = 0L;

    switch (aatyp) {
    case AT_NONE:
    case AT_SPIT:
    case AT_EXPL:
    case AT_BOOM:
    case AT_GAZE:
    case AT_BREA:
    case AT_MAGC:
        w_mask = ~0L; /* special case; no defense needed */
        break;
    case AT_CLAW:
    case AT_TUCH:
    case AT_WEAP:
        w_mask = W_ARMG; /* caller needs to check for weapon */
        break;
    case AT_KICK:
        w_mask = W_ARMF;
        break;
    case AT_BUTT:
        w_mask = W_ARMH;
        break;
    case AT_HUGS:
        w_mask = (W_ARMC | W_ARMG); /* attacker needs both to be protected */
        break;
    case AT_BITE:
    case AT_STNG:
    case AT_ENGL:
    case AT_TENT:
    default:
        w_mask = 0L; /* no defense available */
        break;
    }
    return w_mask;
}

/*mhitm.c*/
