#include <string.h>
#include "erl_nif.h"
#include "mruby.h"
#include "mruby/compile.h"

ErlNifResourceType* RESOURCE_TYPE;

static void
resource_desructor(ErlNifEnv* env, void* obj)
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

    RESOURCE_TYPE = enif_open_resource_type(env, mod, name, resource_desructor, flags, NULL);
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
error(ErlNifEnv* env, char* reason)
{
    ERL_NIF_TERM error_atom = enif_make_atom_len(env, "error", 5);
    ERL_NIF_TERM reason_atom = enif_make_atom_len(env, reason, strlen(reason));
    return enif_make_tuple2(env, error_atom, reason_atom);
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

    if(argc != 2)
        return enif_make_badarg(env);

    void* mrb_state_obj;
    if(!enif_get_resource(env, argv[0], RESOURCE_TYPE, &mrb_state_obj))
        return enif_make_badarg(env);

    mrb_state* mrb = (mrb_state*) mrb_state_obj;

    ErlNifBinary code;
    if(!enif_inspect_binary(env, argv[1], &code))
        return enif_make_badarg(env);

    mrb_value obj = mrb_load_nstring(mrb, code.data, code.size);
    ERL_NIF_TERM ok = enif_make_atom_len(env, "ok", 2);

    return ok;
}

static ErlNifFunc nif_funcs[] = {
    {"create_nif", 0, create_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND},
    {"exec_nif", 2, exec_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND}
};


ERL_NIF_INIT(Elixir.MXRuby, nif_funcs, &load, NULL, NULL, NULL);
