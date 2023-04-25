#include "filemagic.h"

/* Returns the magic version */
static VALUE
rb_magic_version(VALUE klass) {
  char version[16] = "0";
#ifdef HAVE_MAGIC_VERSION
  RB_MAGIC_SET_VERSION(magic_version() / 100, magic_version() % 100)
#endif
  return rb_str_new2(version);
}

/* Returns the magic path */
static VALUE
rb_magic_getpath(VALUE klass) {
  const char *path = magic_getpath(NULL, 0);
  return path != NULL ? rb_str_new2(path) : Qnil;
}

/* Converts flags to integer */
static VALUE
rb_magic_flags(VALUE klass, VALUE flags) {
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_SYM")), f, g;
  int i = MAGIC_NONE, j;

  if (TYPE(flags) != T_ARRAY) {
    rb_raise(rb_eTypeError,
      "wrong argument type %s (expected Array)",
      rb_obj_classname(flags));
  }

  for (j = 0; j < RARRAY_LEN(flags); j++) {
    f = rb_ary_entry(flags, j);

    switch (TYPE(f)) {
      case T_SYMBOL:
        if (RTEST(g = rb_hash_aref(map, f))) {
          f = g;
          /* fall through */
        }
        else {
          f = rb_funcall(f, rb_intern("inspect"), 0);
          rb_raise(rb_eArgError, "%s: %s",
            NIL_P(g) ? "no such flag" : "flag not available",
            StringValueCStr(f));

          break;
        }
      case T_FIXNUM:
        i |= NUM2INT(f);
        break;
      default:
        rb_raise(rb_eTypeError,
          "wrong argument type %s (expected Fixnum or Symbol)",
          rb_obj_classname(f));
    }
  }

  return INT2FIX(i);
}

/* FileMagic.new */
static VALUE
rb_magic_new(int argc, VALUE *argv, VALUE klass) {
  VALUE obj, args[2];
  magic_t ms;

  if (rb_block_given_p()) {
    rb_warn(
      "FileMagic.new() does not take a block; use FileMagic.open() instead");
  }

  if (argc > 0 && TYPE(args[1] = argv[argc - 1]) == T_HASH) {
    argc--;
  }
  else {
    args[1] = rb_hash_new();
  }

  args[0] = rb_magic_flags(klass, rb_ary_new4(argc, argv));

  if ((ms = magic_open(NUM2INT(args[0]))) == NULL) {
    rb_raise(rb_FileMagicError,
      "failed to initialize magic cookie (%d)", errno || -1);
  }

  if (magic_load(ms, NULL) == -1) {
    rb_raise(rb_FileMagicError,
      "failed to load database: %s", magic_error(ms));
  }

  obj = Data_Wrap_Struct(klass, 0, rb_magic_free, ms);
  rb_obj_call_init(obj, 2, args);

  return obj;
}

static void
rb_magic_free(magic_t ms) {
  magic_close(ms);
}

static VALUE
rb_magic_init(int argc, VALUE *argv, VALUE self) {
  VALUE flags, options, keys, k, m;
  ID mid;
  int i;

  if (rb_scan_args(argc, argv, "11", &flags, &options) == 1) {
    options = rb_hash_new();
  }

  rb_iv_set(self, "iflags", flags);
  rb_iv_set(self, "closed", Qfalse);
  rb_iv_set(self, "@simplified", Qfalse);

  keys = rb_funcall(options, rb_intern("keys"), 0);

  for (i = 0; i < RARRAY_LEN(keys); i++) {
    k = rb_ary_entry(keys, i);
    m = rb_str_plus(rb_obj_as_string(k), rb_str_new2("="));

    if (rb_respond_to(self, mid = rb_intern(StringValueCStr(m)))) {
      rb_funcall(self, mid, 1, rb_hash_aref(options, k));
    }
    else {
      k = rb_funcall(k, rb_intern("inspect"), 0);
      rb_raise(rb_eArgError, "illegal option: %s", StringValueCStr(k));
    }
  }

  return Qnil;
}

/* Frees resources allocated */
static VALUE
rb_magic_close(VALUE self) {
  magic_t ms;

  if (RTEST(rb_magic_closed_p(self))) {
    return Qnil;
  }

  GetMagicSet(self, ms);
  rb_magic_free(ms);

  /* This keeps rb_magic_free from trying to free closed objects */
  DATA_PTR(self) = NULL;

  rb_iv_set(self, "closed", Qtrue);

  return Qnil;
}

static VALUE
rb_magic_closed_p(VALUE self) {
  return rb_attr_get(self, rb_intern("closed"));
}

/* Return a string describing the named file */
RB_MAGIC_TYPE(file, FILE)

/* Return a string describing the string buffer */
RB_MAGIC_TYPE(buffer, BUFFER)

/* Return a string describing the file descriptor */
RB_MAGIC_TYPE(descriptor, DESCRIPTOR)

/* Get the flags as array of symbols */
static VALUE
rb_magic_getflags(VALUE self) {
  VALUE ary = rb_ary_new();
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_INT"));
  int i = NUM2INT(rb_attr_get(self, rb_intern("iflags"))), j = 0;

  while ((i -= j) > 0) {
    j = pow(2, (int)(log(i) / log(2)));
    rb_ary_unshift(ary, rb_hash_aref(map, INT2FIX(j)));
  }

  return ary;
}

/* Set flags on the ms object */
static VALUE
rb_magic_setflags(VALUE self, VALUE flags) {
  magic_t ms;

  GetMagicSet(self, ms);

  rb_iv_set(self, "iflags",
    flags = rb_magic_flags(CLASS_OF(self), rb_Array(flags)));

  return INT2FIX(magic_setflags(ms, NUM2INT(flags)));
}

/* Lists a magic database file */
RB_MAGIC_APPRENTICE(list)

/* Loads a magic database file */
RB_MAGIC_APPRENTICE(load)

/* Checks validity of a magic database file */
RB_MAGIC_APPRENTICE(check)

/* Compiles a magic database file */
RB_MAGIC_APPRENTICE(compile)

void
Init_ruby_filemagic() {
  char version[16] = "0";
  cFileMagic = rb_define_class("FileMagic", rb_cObject);
  rb_undef_alloc_func(cFileMagic);

#if defined(FILE_VERSION_MAJOR)
  RB_MAGIC_SET_VERSION(FILE_VERSION_MAJOR, patchlevel)
#elif defined(MAGIC_VERSION)
  RB_MAGIC_SET_VERSION(MAGIC_VERSION / 100, MAGIC_VERSION % 100)
#endif

  /* Version of libmagic(3) this version of ruby-filemagic was built with. */
  rb_define_const(cFileMagic, "MAGIC_VERSION", rb_str_new2(version));

  rb_define_singleton_method(cFileMagic, "library_version", rb_magic_version,  0);
  rb_define_singleton_method(cFileMagic, "path",            rb_magic_getpath,  0);
  rb_define_singleton_method(cFileMagic, "flags",           rb_magic_flags,    1);
  rb_define_singleton_method(cFileMagic, "new",             rb_magic_new,     -1);

  rb_define_method(cFileMagic, "initialize", rb_magic_init,       -1);
  rb_define_method(cFileMagic, "close",      rb_magic_close,       0);
  rb_define_method(cFileMagic, "closed?",    rb_magic_closed_p,    0);
  rb_define_method(cFileMagic, "file",       rb_magic_file,       -1);
  rb_define_method(cFileMagic, "buffer",     rb_magic_buffer,     -1);
  rb_define_method(cFileMagic, "descriptor", rb_magic_descriptor, -1);
  rb_define_method(cFileMagic, "flags",      rb_magic_getflags,    0);
  rb_define_method(cFileMagic, "flags=",     rb_magic_setflags,    1);
  rb_define_method(cFileMagic, "list",       rb_magic_list,       -1);
  rb_define_method(cFileMagic, "load",       rb_magic_load,       -1);
  rb_define_method(cFileMagic, "check",      rb_magic_check,      -1);
  rb_define_method(cFileMagic, "compile",    rb_magic_compile,    -1);

  rb_alias(cFileMagic, rb_intern("valid?"), rb_intern("check"));

  rb_FileMagicError = rb_define_class_under(cFileMagic, "FileMagicError", rb_eStandardError);
  rb_undef_alloc_func(rb_FileMagicError);

  /*======= BEGIN CONSTANTS =======*/
#ifdef MAGIC_NONE
  /* No special handling. */
  rb_define_const(cFileMagic, "MAGIC_NONE", INT2FIX(MAGIC_NONE));
#endif
#ifdef MAGIC_DEBUG
  /* Print debugging messages to stderr. */
  rb_define_const(cFileMagic, "MAGIC_DEBUG", INT2FIX(MAGIC_DEBUG));
#endif
#ifdef MAGIC_SYMLINK
  /* If the file queried is a symlink, follow it. */
  rb_define_const(cFileMagic, "MAGIC_SYMLINK", INT2FIX(MAGIC_SYMLINK));
#endif
#ifdef MAGIC_COMPRESS
  /* If the file is compressed, unpack it and look at the contents. */
  rb_define_const(cFileMagic, "MAGIC_COMPRESS", INT2FIX(MAGIC_COMPRESS));
#endif
#ifdef MAGIC_DEVICES
  /* If the file is a block or character special device, then open the device and try to look in its contents. */
  rb_define_const(cFileMagic, "MAGIC_DEVICES", INT2FIX(MAGIC_DEVICES));
#endif
#ifdef MAGIC_MIME_TYPE
  /* Return a MIME type string, instead of a textual description. */
  rb_define_const(cFileMagic, "MAGIC_MIME_TYPE", INT2FIX(MAGIC_MIME_TYPE));
#endif
#ifdef MAGIC_CONTINUE
  /* Return all matches, not just the first. */
  rb_define_const(cFileMagic, "MAGIC_CONTINUE", INT2FIX(MAGIC_CONTINUE));
#endif
#ifdef MAGIC_CHECK
  /* Check the magic database for consistency and print warnings to stderr. */
  rb_define_const(cFileMagic, "MAGIC_CHECK", INT2FIX(MAGIC_CHECK));
#endif
#ifdef MAGIC_PRESERVE_ATIME
  /* On systems that support utime(3) or utimes(2), attempt to preserve the access time of files analysed. */
  rb_define_const(cFileMagic, "MAGIC_PRESERVE_ATIME", INT2FIX(MAGIC_PRESERVE_ATIME));
#endif
#ifdef MAGIC_RAW
  /* Don't translate unprintable characters to a \ooo octal representation. */
  rb_define_const(cFileMagic, "MAGIC_RAW", INT2FIX(MAGIC_RAW));
#endif
#ifdef MAGIC_ERROR
  /* Treat operating system errors while trying to open files and follow symlinks as real errors, instead of printing them in the magic buffer. */
  rb_define_const(cFileMagic, "MAGIC_ERROR", INT2FIX(MAGIC_ERROR));
#endif
#ifdef MAGIC_MIME_ENCODING
  /* Return a MIME encoding, instead of a textual description. */
  rb_define_const(cFileMagic, "MAGIC_MIME_ENCODING", INT2FIX(MAGIC_MIME_ENCODING));
#endif
#ifdef MAGIC_MIME
  /* Shorthand for `MAGIC_MIME_TYPE \| MAGIC_MIME_ENCODING`. */
  rb_define_const(cFileMagic, "MAGIC_MIME", INT2FIX(MAGIC_MIME));
#endif
#ifdef MAGIC_APPLE
  /* Return the Apple creator and type. */
  rb_define_const(cFileMagic, "MAGIC_APPLE", INT2FIX(MAGIC_APPLE));
#endif
#ifdef MAGIC_EXTENSION
  /* Return a slash-separated list of extensions for this file type. */
  rb_define_const(cFileMagic, "MAGIC_EXTENSION", INT2FIX(MAGIC_EXTENSION));
#endif
#ifdef MAGIC_COMPRESS_TRANSP
  /* Don't report on compression, only report about the uncompressed data. */
  rb_define_const(cFileMagic, "MAGIC_COMPRESS_TRANSP", INT2FIX(MAGIC_COMPRESS_TRANSP));
#endif
#ifdef MAGIC_NO_COMPRESS_FORK
  /* Don't allow decompressors that use fork. */
  rb_define_const(cFileMagic, "MAGIC_NO_COMPRESS_FORK", INT2FIX(MAGIC_NO_COMPRESS_FORK));
#endif
#ifdef MAGIC_NODESC
  /* Shorthand for `MAGIC_EXTENSION \| MAGIC_MIME \| MAGIC_APPLE`. */
  rb_define_const(cFileMagic, "MAGIC_NODESC", INT2FIX(MAGIC_NODESC));
#endif
#ifdef MAGIC_NO_CHECK_COMPRESS
  /* Don't look inside compressed files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_COMPRESS", INT2FIX(MAGIC_NO_CHECK_COMPRESS));
#endif
#ifdef MAGIC_NO_CHECK_TAR
  /* Don't examine tar files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TAR", INT2FIX(MAGIC_NO_CHECK_TAR));
#endif
#ifdef MAGIC_NO_CHECK_SOFT
  /* Don't consult magic files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_SOFT", INT2FIX(MAGIC_NO_CHECK_SOFT));
#endif
#ifdef MAGIC_NO_CHECK_APPTYPE
  /* Don't check for EMX application type (only on EMX). */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_APPTYPE", INT2FIX(MAGIC_NO_CHECK_APPTYPE));
#endif
#ifdef MAGIC_NO_CHECK_ELF
  /* Don't print ELF details. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ELF", INT2FIX(MAGIC_NO_CHECK_ELF));
#endif
#ifdef MAGIC_NO_CHECK_TEXT
  /* Don't check for various types of text files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TEXT", INT2FIX(MAGIC_NO_CHECK_TEXT));
#endif
#ifdef MAGIC_NO_CHECK_CDF
  /* Don't get extra information on MS Composite Document Files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_CDF", INT2FIX(MAGIC_NO_CHECK_CDF));
#endif
#ifdef MAGIC_NO_CHECK_CSV
  /* Don't examine CSV files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_CSV", INT2FIX(MAGIC_NO_CHECK_CSV));
#endif
#ifdef MAGIC_NO_CHECK_TOKENS
  /* Don't look for known tokens inside ascii files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TOKENS", INT2FIX(MAGIC_NO_CHECK_TOKENS));
#endif
#ifdef MAGIC_NO_CHECK_ENCODING
  /* Don't check text encodings. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ENCODING", INT2FIX(MAGIC_NO_CHECK_ENCODING));
#endif
#ifdef MAGIC_NO_CHECK_JSON
  /* Don't examine JSON files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_JSON", INT2FIX(MAGIC_NO_CHECK_JSON));
#endif
#ifdef MAGIC_NO_CHECK_SIMH
  /* Don't examine SIMH tape files. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_SIMH", INT2FIX(MAGIC_NO_CHECK_SIMH));
#endif
#ifdef MAGIC_NO_CHECK_BUILTIN
  /* No built-in tests; only consult the magic file. Shorthand for `MAGIC_NO_CHECK_COMPRESS \| MAGIC_NO_CHECK_TAR \| MAGIC_NO_CHECK_APPTYPE \| MAGIC_NO_CHECK_ELF \| MAGIC_NO_CHECK_TEXT \| MAGIC_NO_CHECK_CSV \| MAGIC_NO_CHECK_CDF \| MAGIC_NO_CHECK_TOKENS \| MAGIC_NO_CHECK_ENCODING \| MAGIC_NO_CHECK_JSON \| MAGIC_NO_CHECK_SIMH`. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_BUILTIN", INT2FIX(MAGIC_NO_CHECK_BUILTIN));
#endif
#ifdef MAGIC_NO_CHECK_ASCII
  /* Defined for backwards compatibility. Renamed from MAGIC_NO_CHECK_TEXT. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ASCII", INT2FIX(MAGIC_NO_CHECK_ASCII));
#endif
#ifdef MAGIC_NO_CHECK_FORTRAN
  /* Don't check ascii/fortran. Defined for backwards compatibility; does nothing. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_FORTRAN", INT2FIX(MAGIC_NO_CHECK_FORTRAN));
#endif
#ifdef MAGIC_NO_CHECK_TROFF
  /* Don't check ascii/troff. Defined for backwards compatibility; does nothing. */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TROFF", INT2FIX(MAGIC_NO_CHECK_TROFF));
#endif
#ifdef MAGIC_PARAM_INDIR_MAX
  /* How many levels of recursion will be followed for indirect magic entries. Default is 50. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_INDIR_MAX", INT2FIX(MAGIC_PARAM_INDIR_MAX));
#endif
#ifdef MAGIC_PARAM_NAME_MAX
  /* The maximum number of calls for name/use. Default is 50. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_NAME_MAX", INT2FIX(MAGIC_PARAM_NAME_MAX));
#endif
#ifdef MAGIC_PARAM_ELF_PHNUM_MAX
  /* How many ELF program sections will be processed. Default is 2,048. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_ELF_PHNUM_MAX", INT2FIX(MAGIC_PARAM_ELF_PHNUM_MAX));
#endif
#ifdef MAGIC_PARAM_ELF_SHNUM_MAX
  /* How many ELF sections will be processed. Default is 32,768. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_ELF_SHNUM_MAX", INT2FIX(MAGIC_PARAM_ELF_SHNUM_MAX));
#endif
#ifdef MAGIC_PARAM_ELF_NOTES_MAX
  /* How many ELF notes will be processed. Default is 256. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_ELF_NOTES_MAX", INT2FIX(MAGIC_PARAM_ELF_NOTES_MAX));
#endif
#ifdef MAGIC_PARAM_REGEX_MAX
  /* Length limit for REGEX searches. Default is 8,192. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_REGEX_MAX", INT2FIX(MAGIC_PARAM_REGEX_MAX));
#endif
#ifdef MAGIC_PARAM_BYTES_MAX
  /* Maximum number of bytes to look at inside a file. Default is 7,340,032. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_BYTES_MAX", INT2FIX(MAGIC_PARAM_BYTES_MAX));
#endif
#ifdef MAGIC_PARAM_ENCODING_MAX
  /* Maximum number of bytes to scan for encoding. Default is 65,536. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_ENCODING_MAX", INT2FIX(MAGIC_PARAM_ENCODING_MAX));
#endif
#ifdef MAGIC_PARAM_ELF_SHSIZE_MAX
  /* Maximum ELF section size to process. Default is 134,217,728. */
  rb_define_const(cFileMagic, "MAGIC_PARAM_ELF_SHSIZE_MAX", INT2FIX(MAGIC_PARAM_ELF_SHSIZE_MAX));
#endif
  /*======= END CONSTANTS =======*/
}
