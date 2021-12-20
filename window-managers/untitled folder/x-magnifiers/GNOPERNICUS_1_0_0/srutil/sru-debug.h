/* sru-debug.h
 *
 * Copyright 2001 - 2005 Sun Microsystems, Inc.,
 * Copyright 2001 - 2005 BAUM Retec, A.G.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __SRU_DEBUG_H__
#define __SRU_DEBUG_H__

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */
#include <glib.h>

#if defined SR_DEBUG && ! defined SRU_DEBUG
#  define SRU_DEBUG
#endif /* defined SR_DEBUG && ! defined SRU_DEBUG */


G_BEGIN_DECLS 

#ifdef SRU_DEBUG

#ifndef SRU_DEBUG_MODULE
#  define SRU_DEBUG_MODULE ""
#endif /* SRU_DEBUG_MODULE */

#ifndef SRU_DEBUG_SCOPES
#  define SRU_DEBUG_SCOPES ""
#endif /* SRU_DEBUG_SCOPES */

void sru_debug_init  			();
void sru_debug_terminate 		();
void sru_debug_add_module_and_scopes 	(const gchar *module,
							const gchar *scopes);
void sru_debug_log_all   		(const gchar *module,
					    const gchar *scopes,
					    const gchar *format, ...)
					     G_GNUC_PRINTF (3, 4);

#define SRU_DEBUG_ADD_MODULE_AND_SCOPES					\
	    {								\
		gchar *module, *module_dbg;				\
		const gchar *scopes;					\
		g_assert (SRU_DEBUG_MODULE);				\
		module = g_ascii_strup (SRU_DEBUG_MODULE, -1);		\
		module_dbg = g_strconcat (module, "_DEBUG", NULL);	\
		scopes = g_getenv (module_dbg);				\
		if (!scopes)						\
		    scopes = SRU_DEBUG_SCOPES;				\
		sru_debug_add_module_and_scopes (SRU_DEBUG_MODULE, 	\
							scopes);	\
		g_free (module);					\
		g_free (module_dbg);					\
	    }

#define sru_debug_log(scopes, ...) 					\
		sru_debug_log_all (SRU_DEBUG_MODULE, scopes, __VA_ARGS__);

#define sru_debug_log_enter_function()					\
		sru_debug_log ("entry", "start of \""			\
			    G_GNUC_PRETTY_FUNCTION "\" function");

#define sru_debug_log_leave_function()					\
		sru_debug_log ("entry", "end   of \""			\
			    G_GNUC_PRETTY_FUNCTION "\" function");

#define sru_debug_log_cond(cond, scopes, ...) 				\
	    if (cond)							\
		sru_debug_log_all (SRU_DEBUG_MODULE, scopes, __VA_ARGS__);


#else /* SRU_DEBUG */

#define sru_debug_init()
#define sru_debug_terminate()
#define sru_debug_add_module_and_scopes(module, scopes)
#define sru_debug_log_all(module, scopes, format, ...)
#define SRU_DEBUG_ADD_MODULE_AND_SCOPES
#define sru_debug_log(scopes, ...)
#define sru_debug_log_enter_function()
#define sru_debug_log_leave_function()
#define sru_debug_log_cond(cond, scopes, ...)

#endif /* SRU_DEBUG */

G_END_DECLS

#endif /* __SRU_DEBUG_H__ */
