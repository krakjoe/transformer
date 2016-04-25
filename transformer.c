/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_transformer.h"

static zend_ast* php_transformer_insert_contract(zend_ast *stmts, zval *contract) { /* {{{ */
	zval fn;
	zend_ast *call, *fn_ast, *args_ast, *new_list;
	uint32_t i;

	ZVAL_STRING(&fn, "assert");

	fn_ast = zend_ast_create_zval(&fn);

	if (Z_TYPE_P(contract) == IS_ARRAY) {
		zval *next = NULL;

		call = zend_ast_create_list(0, ZEND_AST_STMT_LIST);
		
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(contract), next) {
			zend_ast *frame;

			args_ast = 
				zend_ast_create_list(1, ZEND_AST_ARG_LIST, Z_ASTVAL_P(next));
			frame = zend_ast_create(ZEND_AST_CALL, fn_ast, args_ast);
			call = zend_ast_list_add(call, frame);			
			Z_TRY_ADDREF(fn);
		} ZEND_HASH_FOREACH_END();
	} else {
		args_ast = zend_ast_create_list(1, ZEND_AST_ARG_LIST, Z_ASTVAL_P(contract));
		call = zend_ast_create(ZEND_AST_CALL, fn_ast, args_ast);
		Z_TRY_ADDREF(fn);
	}

	if (zend_ast_is_list(stmts)) {
		zend_ast_list *stmt_list = zend_ast_get_list(stmts);
		new_list = 
			zend_ast_create_list(0, ZEND_AST_STMT_LIST);

		if (zend_ast_is_list(call)) {
			zend_ast_list *call_list = zend_ast_get_list(call);

			for (i = 0; i < call_list->children; i++) {
				new_list = zend_ast_list_add(new_list, call_list->child[i]);
			}
		} else {
			new_list = zend_ast_list_add(new_list, call);
		}

		for (i = 0; i < stmt_list->children; i++) {
			new_list = 
				zend_ast_list_add(new_list, stmt_list->child[i]);
		}
	} else {
		new_list = 
			zend_ast_create_list(0, ZEND_AST_STMT_LIST);

		if (zend_ast_is_list(call)) {
			zend_ast_list *call_list = zend_ast_get_list(call);

			for (i = 0; i < call_list->children; i++) {
				new_list = zend_ast_list_add(new_list, call_list->child[i]);
			}
		} else {
			new_list = zend_ast_list_add(new_list, call);
		}	

		new_list = zend_ast_list_add(new_list, stmts);
	}
	
	zval_ptr_dtor(&fn);	
	
	return new_list;
} /* }}} */

static zend_ast_transformer_status php_transform_method(zend_ast **in) { /* {{{ */
	zend_ast_decl *decl = 
		(zend_ast_decl*) *in;
	zval *contract = NULL;

	if (!decl || !decl->attributes ||
		!(contract = zend_hash_str_find(decl->attributes, ZEND_STRL("contract")))) {
		return ZEND_AST_TRANSFORM_NEXT;
	}

	decl->child[2] = php_transformer_insert_contract(decl->child[2], contract);

	return ZEND_AST_TRANSFORM_NEXT;
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(transformer)
{
	zend_ast_add_transformer(ZEND_AST_METHOD, php_transform_method);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(transformer)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(transformer)
{
#if defined(COMPILE_DL_TRANSFORMER) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(transformer)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(transformer)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "transformer support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ transformer_module_entry
 */
zend_module_entry transformer_module_entry = {
	STANDARD_MODULE_HEADER,
	"transformer",
	NULL,
	PHP_MINIT(transformer),
	PHP_MSHUTDOWN(transformer),
	PHP_RINIT(transformer),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(transformer),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(transformer),
	PHP_TRANSFORMER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TRANSFORMER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(transformer)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
