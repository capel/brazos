#include "ko.h"
#include "idir.h"

size_t next_ko_id(void) {
  static size_t id = 1;
  return id++;
}

static void register_ko(ko* o);
static void unregister_ko(ko* o);

ko* mk_ko(size_t size, cleanup_func cleanup, view_func view, size_t type) {
  ko* o = kmalloc(size);

  o->cleanup = cleanup;
  o->view = view;
  o->type = type;
  o->rc = 1;
  o->flags = 0;
  o->id = next_ko_id();

  register_ko(o);
  return o;
}

void _kput(ko* o, const char* file, const char* func) {
  o->rc--; 
  if (o->rc == 1) {
   // printk("CLEANUP %s %s : %k ", file, func, o);
    o->rc = 99; // don't let the hashmap delete it too
    unregister_ko(o);
    CLEANUP(o);
    kfree(o);
  }
}

static idir* registry;
static bool created;

void setup_ko_registry() {
  created = false;
  printk("in reg");
  registry = mk_idir();
  printk("registery: %p", registry);
  created = true;
}

static void register_ko(ko* o) {
  if (!created) return;
  idir_insert(registry, o->id, o);
}

static void unregister_ko(ko* o) {
  idir_delete(registry, o->id);
}

ko* get_ko(size_t id) {
  return idir_lookup(registry, id);
}


void err_cleanup(ko* o) {}


ko* mk_err_ko(int err, view_func view) {
  ko* o = kmalloc(sizeof(ko));

  o->cleanup = err_cleanup;
  o->view = view;
  o->type = KO_ERROR;
  o->rc = 9;
  o->flags = 0;
  o->id = err;

  register_ko(o);
  return o;
}

CS_VIEW_FUNC(not_sup, "Operation not supported on this type");
CS_VIEW_FUNC(error, "An unspecified server error occured.");
CS_VIEW_FUNC(bad_filename, "Bad filename.");
CS_VIEW_FUNC(bad_program, "Bad program name.");
CS_VIEW_FUNC(not_dir, "Not a directory.");
CS_VIEW_FUNC(not_sinkhole, "Not a sinkhole.");
CS_VIEW_FUNC(bad_rid, "Invalid rid called");
CS_VIEW_FUNC(bad_arg, "Bad argument to syscalls");
CS_VIEW_FUNC(bad_syscall, "Bad syscall number");
CS_VIEW_FUNC(is_future, "A future was passed when a concrete object needed.");
CS_VIEW_FUNC(not_found, "Requested filename was not found");
CS_VIEW_FUNC(not_fountain, "Not a fountain.");

void setup_err_ko() {
  mk_err_ko(E_NOT_SUPPORTED, not_sup);
  mk_err_ko(E_ERROR, error);
  mk_err_ko(E_BAD_FILENAME, bad_filename);
  mk_err_ko(E_BAD_PROGRAM, bad_program);
  mk_err_ko(E_NOT_DIR, not_dir);
  mk_err_ko(E_NOT_SINKHOLE, not_sinkhole);
  mk_err_ko(E_BAD_RID, bad_rid);
  mk_err_ko(E_BAD_ARG, bad_arg);
  mk_err_ko(E_BAD_SYSCALL, bad_syscall);
  mk_err_ko(E_IS_FUTURE, is_future);
  mk_err_ko(E_NOT_FOUND, not_found);
  mk_err_ko(E_NOT_FOUNTAIN, not_fountain);
}
