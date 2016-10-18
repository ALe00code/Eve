#include <runtime.h>

void compile_into_bag(evaluation ev, bag b, estring code) {
    heap h = ev->working;
    buffer codebuf = alloca_wrap_buffer(code->body, code->length);
    bag compiler_bag = (bag)create_edb(h, 0);
    bag compiled = compile_eve(ev->h, codebuf, false);

    //    vector_foreach(blocks, block) {
    //        vector_insert(b->blocks, block);
    //    }

}

static CONTINUATION_1_4(bagbag_insert, evaluation, value, value, value, uuid bku);
static void bagbag_insert(evaluation ev, value e, value a, value v, uuid bku)
{
}

static CONTINUATION_1_1(bagbag_commit, evaluation, edb)
static void bagbag_commit(evaluation ev, edb s)
{
    edb_foreach_e(s, e, sym(tag), sym(bag)) {
        bag b = table_find(ev->t_input, e);
        if(!b) {
            b = (bag)create_edb(ev->h, 0);
            table_set(ev->t_input, e, b);
        }
        value v;
        if ((v = lookupv(s, e, sym(name)))){
            table_set(ev->scopes, v, e);
        }
    }

    edb_foreach_ev(s, e, sym(bags), v, m) {
        // we're going to silent refuse to bind fruits into the bag namespace?
        // maybe this map should be raw eavs?
        bag b;
        if (table_find(ev->t_input, e)) {
            table_set(ev->scopes, v, e);
        }
    }

    edb_foreach_ev(s, e, sym(code), code, m) {
        bag b = (bag)table_find(ev->t_input, e);

        if(b) {
            compile_into_bag(ev, b, code);
        }
    }
}


CONTINUATION_1_5(bagbag_scan, evaluation, int, listener, value, value, value);
void bagbag_scan(evaluation ev, int sig, listener out, value e, value a, value v)
{
    if ((sig == s_eAV) && (a == sym(name))) {
        value e;
        if ((e = table_find(ev->scopes, v))) {
            apply(out, e, a, v, 0);
        }
    }
}

bag init_bag_bag(evaluation ev)
{
    bag b = allocate(ev->h, sizeof(struct bag));
    b->insert = cont(ev->h, bagbag_insert, ev);
    b->scan = cont(ev->h, bagbag_scan, ev);
    b->commit = cont(ev->h, bagbag_commit, ev);
    b->listeners = allocate_table(ev->h, key_from_pointer, compare_pointer);
    b->blocks = allocate_vector(ev->h, 0);
    return b;
}


// @FIXME
// The #process can start before the #bag is loaded with blocks.
// This means that build_evaluation will not be able to guarantee that it has access to all of its blocks yet.
// Blocks can't be built on the bag itself because they are evaluation-specific.
// Stratifying this in-eve only hides the fact that the process watcher is now order-sensitive wrt its bags.
//
// @NOTE: How is this not already a race between the process or its bag getting created first?
// Is it because the process doesn't care when scopes is filled in?
//
// Approaches to fix this include:
// 1. Backlink the evaluations using a bag into a vector on the bag, so the bag watcher can inject new blocks into evaluations.
//    Pros:
//      - easy
//    Cons:
//      - kind of a layer violation
//      - can't remove blocks this way
// 2. Include a vector of block_listeners on the bag that will be called when a block is added or removed (more general 1)
//    Pros:
//      - supports addition and removal
//   Cons:
//      - who's in charge of registering these?
//      - extra work
// 3. Store the ids of compiled blocks in an evaluation and every time setup_evaluation is called, ensure that all bag blocks are accounted for.
//    Pros:
//      - easy
//    Cons:
//      - pretty wasteful
//      - doesn't handle removal
// 4. Move module loading from bag watcher to process watcher
//    Pros:
//      - easy
//    Cons:
//      - even more hard-coded
//      - doesn't really mirror our 'contexts have blocks' semantics
//      - basically back to the magic table of names to blocks
