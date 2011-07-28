#include "include/common.h"

static integer *new_rid(rid_manager * ridm, const vector* v, size_t level, bool *done) {
  *done = true;
  integer * i = mkint((int)ridm->d1++);
  return i;
}

static ent_lookup internal_funcs[] = {
  {DEFAULT_LOOKUP_FUNC, managed_lookup_func_not_found},
  {"new!", new_rid},
};

MAKE_LOOKUP(internal_funcs);

static void cleanup() { printk("TODO"); }

static ent_funcs ridm_funcs = {
  .lookup = NAME_LOOKUP(internal_funcs),
  .link = disable_link,
  .unlink = disable_unlink,
  .map = disable_map,
  .unmap = disable_unmap,
  .cleanup = cleanup,
};

rid_manager* mkrid_manager()
{
  rid_manager *ridm = entalloc(&ridm_funcs);
  ridm->d1 = (void*)1;
  simple_managed_create(ridm);
  return ridm;
}

