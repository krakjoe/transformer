dnl $Id$
dnl config.m4 for extension transformer

PHP_ARG_ENABLE(transformer, whether to enable transformer support,
[  --enable-transformer           Enable transformer support])

if test "$PHP_TRANSFORMER" != "no"; then
  PHP_NEW_EXTENSION(transformer, transformer.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
