/*
 * Conservative, real-number-oriented radical simplification.
 * Only bounded positive integer radicands are multiplied/extracted.
 * Like radicals may be added if their root ASTs match exactly.
 * Never merge dissimilar radicals across addition (sqrt(2)+sqrt(3)).
 * Run after the regular algebraic simplifier and the radical formatter,
 * not before normalization (which removes OP_ROOT).
 */
#include "cas.h"

/* Existing commutative flattener; unlike SIMP_NORMALIZE it keeps OP_ROOT. */
extern bool simplify_commutative(pcas_ast_t *e);

#define RADICAL_BOUND 100000

static bool bounded_int(pcas_ast_t *e, mp_small *n, bool signed_value) {
    if(e == NULL || e->type != NODE_NUMBER ||
       !mp_rat_is_integer(e->op.num))
        return false;
    if(mp_int_compare_value(&e->op.num->num, RADICAL_BOUND) > 0 ||
       mp_int_compare_value(&e->op.num->num, -RADICAL_BOUND) < 0)
        return false;
    if(!signed_value && mp_rat_compare_zero(e->op.num) <= 0)
        return false;
    return mp_int_to_int(&e->op.num->num, n) == MP_OK;
}

/* Simplify roots of small positive integers, such as sqrt(72)=6sqrt(2).
 * This deliberately does not touch roots of negative numbers, because
 * principal complex roots require different branch rules.
 */
static bool extract_integer_root(pcas_ast_t *e) {
    mp_small order, number, rest, outside = 1, factor, power, p;
    pcas_ast_t *radicand, *degree;

    if(!isoptype(e, OP_ROOT))
        return false;

    degree = ast_ChildGet(e, 0);
    radicand = ast_ChildGet(e, 1);

    if(!bounded_int(degree, &order, false) || order < 2 || order > 6)
        return false;

    /* Reduce any wholly numeric radicand first. */
    eval(radicand, EVAL_EASY);
    if(!bounded_int(radicand, &number, false))
        return false;

    rest = number;
    for(p = 2; p <= rest / p; ++p) {
        unsigned i;
        power = p;
        for(i = 1; i < (unsigned)order; ++i) {
            if(power > RADICAL_BOUND / p) {
                power = RADICAL_BOUND + 1;
                break;
            }
            power *= p;
        }
        if(power > RADICAL_BOUND)
            continue;
        while(rest % power == 0) {
            outside *= p;
            rest /= power;
        }
    }

    if(rest == 1) {
        replace_node(e, ast_MakeNumber(num_FromInt(outside)));
        return true;
    }
    if(outside > 1) {
        replace_node(e, ast_MakeBinary(OP_MULT,
            ast_MakeNumber(num_FromInt(outside)),
            ast_MakeBinary(OP_ROOT,
                ast_MakeNumber(num_FromInt(order)),
                ast_MakeNumber(num_FromInt(rest)))));
        return true;
    }
    return false;
}

static bool unpack_radical_term(pcas_ast_t *e, pcas_ast_t **root,
                               mp_small *coefficient) {
    pcas_ast_t *a, *b;
    if(isoptype(e, OP_ROOT)) {
        *root = e;
        *coefficient = 1;
        return true;
    }
    if(!isoptype(e, OP_MULT) || ast_ChildLength(e) != 2)
        return false;

    a = ast_ChildGet(e, 0);
    b = ast_ChildGet(e, 1);
    if(isoptype(a, OP_ROOT) && bounded_int(b, coefficient, true)) {
        *root = a;
        return true;
    }
    if(isoptype(b, OP_ROOT) && bounded_int(a, coefficient, true)) {
        *root = b;
        return true;
    }
    return false;
}

static bool pair_radicals(pcas_ast_t *e) {
    unsigned i, j;
    bool changed = false;

    if(isoptype(e, OP_MULT)) {
        /* sqrt(a)*sqrt(b)=sqrt(a*b) for real a,b>0.
         * Restrict the rule to known positive integer radicands.
         */
        for(i = 0; i < ast_ChildLength(e); ++i) {
            pcas_ast_t *a = ast_ChildGet(e, i);
            mp_small index, av;
            if(!isoptype(a, OP_ROOT) ||
               !bounded_int(ast_ChildGet(a, 0), &index, false) ||
               !bounded_int(ast_ChildGet(a, 1), &av, false))
                continue;

            for(j = i + 1; j < ast_ChildLength(e); ++j) {
                pcas_ast_t *b = ast_ChildGet(e, j);
                mp_small index2, bv;
                pcas_ast_t *new_root;

                if(!isoptype(b, OP_ROOT) ||
                   !bounded_int(ast_ChildGet(b, 0), &index2, false) ||
                   index != index2 ||
                   !bounded_int(ast_ChildGet(b, 1), &bv, false) ||
                   av > RADICAL_BOUND / bv)
                    continue;

                new_root = ast_MakeBinary(OP_ROOT,
                    ast_MakeNumber(num_FromInt(index)),
                    ast_MakeNumber(num_FromInt(av * bv)));
                ast_Cleanup(ast_ChildRemove(e, a));
                ast_Cleanup(ast_ChildRemove(e, b));
                extract_integer_root(new_root);
                ast_ChildAppend(e, new_root);
                return true;
            }
        }
    } else if(isoptype(e, OP_ADD)) {
        /* Like radicals: a*sqrt(k)+b*sqrt(k) = (a+b)*sqrt(k).
         * Compare both radicand and root index, not just text labels.
         */
        for(i = 0; i < ast_ChildLength(e); ++i) {
            pcas_ast_t *a = ast_ChildGet(e, i), *ra;
            mp_small ca;
            if(!unpack_radical_term(a, &ra, &ca))
                continue;
            for(j = i + 1; j < ast_ChildLength(e); ++j) {
                pcas_ast_t *b = ast_ChildGet(e, j), *rb, *replacement;
                mp_small cb, total;
                if(!unpack_radical_term(b, &rb, &cb) ||
                   !ast_Compare(ra, rb) ||
                   (ca > 0 && cb > RADICAL_BOUND - ca) ||
                   (ca < 0 && cb < -RADICAL_BOUND - ca))
                    continue;
                total = ca + cb;
                if(total == 0)
                    replacement = ast_MakeNumber(num_FromInt(0));
                else if(total == 1)
                    replacement = ast_Copy(ra);
                else
                    replacement = ast_MakeBinary(OP_MULT,
                        ast_MakeNumber(num_FromInt(total)), ast_Copy(ra));

                ast_Cleanup(ast_ChildRemove(e, a));
                ast_Cleanup(ast_ChildRemove(e, b));
                ast_ChildAppend(e, replacement);
                return true;
            }
        }
    }
    return changed;
}

static bool radical_pass(pcas_ast_t *e) {
    pcas_ast_t *child;
    bool changed = false;

    if(e == NULL || e->type != NODE_OPERATOR)
        return false;

    for(child = opbase(e); child != NULL; child = child->next)
        changed |= radical_pass(child);

    if(isoptype(e, OP_ROOT))
        changed |= extract_integer_root(e);

    if(isoptype(e, OP_MULT) || isoptype(e, OP_ADD)) {
        /* Flatten extracted coefficients; do not normalize OP_ROOT. */
        changed |= simplify_commutative(e);
        changed |= eval(e, EVAL_COMMUTATIVE | EVAL_BASIC_IDENTITIES);
        if(pair_radicals(e))
            changed = true;
    }

    return changed;
}

bool simplify_radical_pairs(pcas_ast_t *e) {
    bool changed = false;
    unsigned limit;
    /* Bound passes to prevent a pathological expression from looping. */
    for(limit = 0; limit < 48; ++limit) {
        if(!radical_pass(e))
            break;
        changed = true;
    }
    return changed;
}
