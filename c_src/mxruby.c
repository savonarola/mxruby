#include <string.h>
#include "erl_nif.h"
#include "mruby.h"
#include "mruby/hash.h"
#include "mruby/compile.h"
#include "mruby/string.h"
#include "mruby/value.h"
#include "mruby/variable.h"

ErlNifResourceType* RESOURCE_TYPE;

static void
resource_destructor(ErlNifEnv* env, void* obj)
{
    mrb_state *mrb_resource = (mrb_state*) obj;
    mrb_state *mrb = enif_alloc(sizeof(mrb_state));
    memcpy(mrb, mrb_resource, sizeof(mrb_state));
    mrb_close(mrb);
}

static int
load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    const char* mod = "Elixir.MXRuby";
    const char* name = "mrb_state";
    int flags = ERL_NIF_RT_CREATE;

    RESOURCE_TYPE = enif_open_resource_type(env, mod, name, resource_destructor, flags, NULL);
    if(RESOURCE_TYPE == NULL) return -1;
    return 0;
}

static void*
enif_mrb_allocf(mrb_state *mrb, void *p, size_t size, void *ud)
{
  if (size == 0) {
    enif_free(p);
    return NULL;
  }
  else {
    return enif_realloc(p, size);
  }
}

static ERL_NIF_TERM
nresult_tuple(ErlNifEnv* env, const char* status,  const char* reason, size_t len)
{
    ERL_NIF_TERM error_atom = enif_make_atom_len(env, status, strlen(status));
    ErlNifBinary reason_bin;
    if(!enif_alloc_binary(len, &reason_bin)) {
        return error_atom;
    }
    memcpy(reason_bin.data, reason, len);
    ERL_NIF_TERM reason_term = enif_make_binary(env, &reason_bin);
    return enif_make_tuple2(env, error_atom, reason_term);
}

static mrb_value
atom_to_symbol(ErlNifEnv* env, mrb_state* mrb, ERL_NIF_TERM atom)
{
    unsigned len = 0;
    if(!enif_get_atom_length(env, atom, &len, ERL_NIF_LATIN1))
        return mrb_nil_value();

    char buf[len + 1];

    if(!enif_get_atom(env, atom, buf, len + 1, ERL_NIF_LATIN1))
        return mrb_nil_value();

    mrb_sym mid = mrb_intern(mrb, buf, len);
    return mrb_symbol_value(mid);
}

static mrb_value
binary_to_string(ErlNifEnv* env, mrb_state* mrb, ERL_NIF_TERM binary_term)
{
    ErlNifBinary binary;
    if(!enif_inspect_binary(env, binary_term, &binary))
        return mrb_nil_value();

    mrb_value str = mrb_str_new(mrb, binary.data, binary.size);
    return str;
}


static mrb_value
kw_to_hash(ErlNifEnv* env, mrb_state* mrb, ERL_NIF_TERM kw)
{

    mrb_value mrb_hash = mrb_hash_new(mrb);

    unsigned kw_len = 0;
    if(!enif_get_list_length(env, kw, &kw_len))
        return mrb_hash;

    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;

    for(int i = 0; i < kw_len; i++) {
        enif_get_list_cell(env, kw, &head, &tail);

        const ERL_NIF_TERM *kw_entry;
        unsigned arity = 0;
        if(!enif_get_tuple(env, head, &arity, &kw_entry))
            return mrb_hash;
        if(arity != 2)
            return mrb_hash;

        ERL_NIF_TERM key = kw_entry[0];
        ERL_NIF_TERM value = kw_entry[1];

        mrb_value mrb_key = atom_to_symbol(env, mrb, key);
        mrb_value mrb_value = binary_to_string(env, mrb, value);

        mrb_hash_set(mrb, mrb_hash, mrb_key, mrb_value);

        kw = tail;
    }

    return mrb_hash;
}

static ERL_NIF_TERM
create_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(argc != 0)
        return enif_make_badarg(env);

    mrb_state *mrb = mrb_open_allocf(enif_mrb_allocf, NULL);
    mrb_state* mrb_resource = enif_alloc_resource(RESOURCE_TYPE, sizeof(mrb_state));
    memcpy(mrb_resource, mrb, sizeof(mrb_state));
    enif_free(mrb);

    ERL_NIF_TERM term = enif_make_resource(env, mrb_resource);
    enif_release_resource(mrb_resource);

    return term;
}


static ERL_NIF_TERM
exec_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(argc != 3)
        return enif_make_badarg(env);

    void* mrb_state_obj;
    if(!enif_get_resource(env, argv[0], RESOURCE_TYPE, &mrb_state_obj))
        return enif_make_badarg(env);

    mrb_state* mrb = (mrb_state*) mrb_state_obj;

    ErlNifBinary code;
    if(!enif_inspect_binary(env, argv[1], &code))
        return enif_make_badarg(env);

    mrb_value args = kw_to_hash(env, mrb, argv[2]);
    mrb_sym sym = mrb_intern_lit(mrb, "$mxruby");
    mrb_gv_set(mrb, sym, args);

    mrb_value obj = mrb_load_nstring(mrb, code.data, code.size);

    if(mrb->exc) {
        mrb_value error_desc = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
        mrb->exc = 0;
        return nresult_tuple(env, "error", RSTRING_PTR(error_desc), RSTRING_LEN(error_desc));
    }

    mrb_value result = mrb_funcall(mrb, obj, "to_s", 0);
    return nresult_tuple(env, "ok", RSTRING_PTR(result), RSTRING_LEN(result));
}

static ErlNifFunc nif_funcs[] = {
    {"create_nif", 0, create_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND},
    {"exec_nif", 3, exec_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND}
};


ERL_NIF_INIT(Elixir.MXRuby, nif_funcs, &load, NULL, NULL, NULL);
