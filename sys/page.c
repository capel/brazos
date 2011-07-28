#include "include/common.h"

size_t kalloc_raw_pages(size_t num);
void* kget_addr_from_page(size_t page);
static pages* mkpages(size_t pnum, size_t num);

static pages * new_pages(mm * m, const vector* v, size_t level, bool *done)
{
  printk("size %d level %d: name %s", v->size, level, v->data[level]);
  // new!/num
  if (v->size != level + 2) {
    return (pages*)E_NOT_FOUND;
  }
  printk("making!");
  size_t num = atoi((char*)v->data[level+1]);
  size_t pnum = kalloc_raw_pages(num);
  pages * pp = mkpages(pnum, num);

  m->f->link = simple_managed_link;
  err_t err = LINK_R(m, pp, "%d", pnum);
  printk("linking page %d", pnum);
  assert(err == SUCCESS);
  m->f->link = disable_link;
  *done = true;
  return kget(pp);
}

static ent_lookup internal_funcs[] = {
  {DEFAULT_LOOKUP_FUNC, managed_lookup_func_not_found},
  {"new!", new_pages}
};

MAKE_LOOKUP(internal_funcs);
MAKE_MAP(internal_funcs);
MAKE_UNMAP(internal_funcs);

static void cleanup(ent* e) { panic("mm was cleaned up!"); }

static ent_funcs mm_funcs = {
  .lookup = NAME_LOOKUP(internal_funcs),
  .link = disable_link,
  .unlink = disable_unlink,
  .map = NAME_MAP(internal_funcs),
  .unmap = NAME_UNMAP(internal_funcs),
  .cleanup = cleanup,
};

ent* mkmm()
{
  mm *m = entalloc(&mm_funcs);
  simple_managed_create(m);
  return m;
}

static void cleanup_pages(pages* p)
{
    // make it obvious we have freed this.
    memset(p->d1, 0xff, PAGE_SIZE * (size_t)p->d2);
    entdealloc(p);
}

static err_t map_pages(pages* p, perms_t rp, size_t *out_size, void **out_ptr) {
  *out_ptr = p->d1;
  *out_size = (size_t)p->d2;
  return SUCCESS;
}

static err_t unmap_pages(pages* p, void *ptr) {
  return SUCCESS;
}

static ent_funcs pages_funcs = {
  .lookup = disable_lookup,
  .link = disable_link,
  .unlink = disable_unlink,
  .map = map_pages,
  .unmap = unmap_pages,
  .cleanup = cleanup_pages,
};

static pages* mkpages(size_t pnum, size_t num)
{
    pages* p = entalloc(&pages_funcs);
    p->d2 = (void*)num;
    p->d1 = kget_addr_from_page(pnum);
    return p;
}



