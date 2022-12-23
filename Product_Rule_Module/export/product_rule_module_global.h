#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PRODUCT_RULE_MODULE_LIB)
#  define PRODUCT_RULE_MODULE_EXPORT Q_DECL_EXPORT
# else
#  define PRODUCT_RULE_MODULE_EXPORT 
# endif
#else
# define PRODUCT_RULE_MODULE_EXPORT
#endif
