#include <string.h>
#include "erl_nif.h"
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/string.h"

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
    strncpy(reason_bin.data, reason, len);
    ERL_NIF_TERM reason_term = enif_make_binary(env, &reason_bin);
    return enif_make_tuple2(env, error_atom, reason_term);
}

static ERL_NIF_TERM
result_tuple(ErlNifEnv* env, const char* status, const char* reason)
{
    return nresult_tuple(env, status, reason, strlen(reason));
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

    if(mrb->exc) {
        mrb_value error_desc = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
        mrb->exc = 0;
        return nresult_tuple(env, "error", RSTRING_PTR(error_desc), RSTRING_LEN(error_desc));
    }

    mrb_value error_desc = mrb_funcall(mrb, obj, "to_s", 0);
    return nresult_tuple(env, "ok", RSTRING_PTR(error_desc), RSTRING_LEN(error_desc));
}

static ErlNifFunc nif_funcs[] = {
    {"create_nif", 0, create_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND},
    {"exec_nif", 2, exec_nif, ERL_NIF_DIRTY_JOB_CPU_BOUND}
};


ERL_NIF_INIT(Elixir.MXRuby, nif_funcs, &load, NULL, NULL, NULL);
