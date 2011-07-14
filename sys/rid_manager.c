#include "include/common.h"

static ent_lookup internal_funcs[] = {
  {DEFAULT_LOOKUP_FUNC, managed_lookup_func_not_found},
  {"new!", new_proc},
  {"sched!", do_sched},
  {"me", me},
};


