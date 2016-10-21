#include <math.h>
#include <core/core.h>
#include <unix/unix.h>
#include <types.h>


u64 key_of(value);
boolean equals(value, value);

typedef value eboolean;
extern heap efence;

void print(buffer, value);


typedef struct bag *bag;

void init_runtime();

void error(char *);

#define UUID_LENGTH 12

uuid generate_uuid();

void uuid_base_print(char *, void *);
uuid parse_uuid(string c);
string aprintf(heap h, char *fmt, ...);
void bbprintf(string b, string fmt, ...);

#define pages (tcontext()->page_heap)
#define init (tcontext()->h)

typedef struct perf {
    int count;
    ticks start;
    ticks time;
    int trig;
} *perf;


typedef struct edb *edb;

typedef closure(committer, edb);
typedef closure(listener, value, value, value, uuid);
typedef closure(scanner, int, listener, value, value, value);

struct bag {
    //  uuid u; ?
    scanner scan;
    scanner scan_sync;
    committer commit;
    table listeners;
    ticks last_commit;
};

typedef closure(execf, heap, perf, value *);
typedef closure(flushf);

#define def(__s, __v, __i)  table_set(__s, intern_string((unsigned char *)__v, cstring_length((char *)__v)), __i);

void print_value(buffer, value);

typedef struct evaluation *evaluation;

static value compress_fat_strings(value v)
{
    if (type_of(v) == estring_space) {
        estring x = v;
        if (x->length > 64) return(sym(...));
    }
    return v;
}

#include <edb.h>
#include <multibag.h>

typedef closure(evaluation_result, multibag, multibag, boolean);

typedef closure(error_handler, char *, bag, uuid);
typedef closure(bag_handler, edb);
typedef closure(bag_block_handler, bag, vector, vector); // source, inserts, removes

typedef void (*commit_function)(multibag backing, multibag delta, closure(finish, boolean));

struct evaluation  {
    heap h;
    int regs;
    execf head;
    flushf flush;
    evaluation ev;
    table nmap;
    uuid start;
    vector cleanup;

    estring name;
    heap working; // lifetime is a whole f-t pass
    error_handler error;

    table scopes;
    // f should be going away
    multibag t_input;
    multibag block_t_solution;
    multibag block_f_solution;
    multibag f_solution;
    multibag last_f_solution;
    multibag t_solution;
    multibag t_solution_for_f;

    table counters;
    ticks t;
    boolean non_empty;
    evaluation_result complete;

    thunk terminal;
    thunk run;
    ticks cycle_time;

    // needs to be set up before build time and resolved there
    vector default_scan_scopes;
    vector default_insert_scopes; // really 'session'
    bag bag_bag;
    commit_function commit;
    uuid id;
};


typedef evaluation block;

void execute(evaluation);

table builders_table();
void build(evaluation ev, bag b, uuid root);
table start_fixedpoint(heap, table, table, table);
void close_evaluation(evaluation);

extern char *pathroot;

bag compile_eve(heap h, buffer b, boolean tracing);

//evaluation build_evaluation(heap h,
//                            bag source,
//                            bag block_root);

void run_solver(evaluation s);
void block_close(block);
bag init_request_service();

bag filebag_init(buffer);
extern thunk ignore;

static void get_stack_trace(string *out)
{
    void **stack = 0;
    asm("mov %%rbp, %0": "=rm"(stack)::);
    while (*stack) {
        stack = *stack;
        void * addr = *(void **)(stack - 1);
        if(addr == 0) break;
        bprintf(*out, "0x%016x\n", addr);
    }
}

void merge_scan(evaluation ev, vector scopes, int sig, listener result, value e, value a, value v);


bag init_debug_bag(evaluation ev);
bag init_bag_bag(evaluation ev);

typedef struct process_bag *process_bag;
process_bag process_bag_init(multibag, boolean);

typedef closure(object_handler, edb, uuid);
object_handler create_json_session(heap h, evaluation ev, endpoint down);
evaluation process_resolve(process_bag, uuid);


bag connect_postgres(station s, estring user, estring password, estring database);
bag env_init();
bag start_log(bag base, char *filename);


static inline value blookupv(bag b, value e, value a)
{
    return lookupv((edb)b, e, a);
}

static inline vector blookup_vector(heap h, bag b, value e, value a)
{
    return lookup_vector(h, (edb)b, e, a);
}
bag connect_postgres(station s, estring user, estring password, estring database);
bag env_init();
bag start_log(bag base, char *filename);
void serialize_edb(buffer dest, edb db);
bag udp_bag_init();
bag timer_bag_init();

station create_station(unsigned int address, unsigned short port);
void init_station();
extern heap station_heap;
