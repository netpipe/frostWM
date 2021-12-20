/* sru-debug.c
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include "sru-debug.h"

#define SRU_DEBUG_CNT

#ifdef SRU_DEBUG

static GPtrArray *sru_debug_tags = NULL;
static FILE *sru_debug_file	 = NULL;

static gboolean
sru_debug_is_module_and_scope_logged (const gchar *module,
		    		      const gchar *scope)	    
{
    gchar *tag;
    gint i;
    gboolean rv = FALSE;

    g_assert (module && scope && module[0] && scope[0]);
    g_assert (sru_debug_tags);

    tag = g_strconcat (module, ":", scope, NULL);
    for (i = 0; i < sru_debug_tags->len; i++)
    {
	g_assert (g_ptr_array_index (sru_debug_tags, i));
	if (strcmp (tag, g_ptr_array_index (sru_debug_tags, i)) == 0)
	{
	    rv = TRUE;
	    break;
	}
    }
    g_free (tag);

    return rv;
}

static gchar*
sru_debug_module_and_scopes_real_logged (const gchar *module,
			    	    	 const gchar *scopes)
{
    gchar ** scopes_array;
    gint i;
    GString *rv = g_string_new (NULL);

    g_assert (module && scopes && module[0] && scopes[0]);
    g_assert (sru_debug_tags);

    scopes_array = g_strsplit (scopes, ":", -1);
    g_assert (scopes_array);
    for (i = 0; scopes_array[i]; i++)
    {
	g_assert (scopes_array[i][0]);
	if (sru_debug_is_module_and_scope_logged (module, scopes_array[i]))
	{
	    if (rv->len)
		g_string_append (rv, ":");
	    g_string_append (rv, scopes_array[i]);
	}
    }
    g_strfreev (scopes_array);

    if (rv->len)
	return g_string_free (rv, FALSE);

    g_string_free (rv, TRUE);
    return NULL;    
}

void
sru_debug_add_module_and_scopes (const gchar *module,
			         const gchar *scopes)
{
    gchar ** scopes_array;
    gint i;

    g_assert (module && scopes);
    g_assert (sru_debug_tags);

    if (!module[0] || !scopes[0])
	return;

    scopes_array = g_strsplit (scopes, ":", -1);
    g_assert (scopes_array);
    for (i = 0; scopes_array[i]; i++)
    {
	g_assert (scopes_array[i][0]);
	g_ptr_array_add (sru_debug_tags, 
			    g_strconcat (module, ":", scopes_array[i], NULL));
    }
    g_strfreev (scopes_array);
}

void
sru_debug_log_all (const gchar *module,
	    	   const gchar *scopes,
	           const gchar *format,
	           ...)
{
    va_list args;
    gchar *message;
    gchar *scopes_real;
#ifdef SRU_DEBUG_CNT
    static long cnt = 0;
#endif /* SRU_DEBUG_CNT */

    g_assert (module && module[0] && scopes && scopes[0]); 
    g_assert (sru_debug_file);

    scopes_real = sru_debug_module_and_scopes_real_logged (module, scopes);
    if (!scopes_real)
	return;

    va_start (args, format);
    message = g_strdup_vprintf (format, args);
#ifdef SRU_DEBUG_CNT
    fprintf (sru_debug_file, "\n%03ld %s\t%s\t%s", cnt++, module,
						    scopes_real, message);
#else /* SRU_DEBUG_CNT */
    fprintf (sru_debug_file, "\n%s\t%s\t%s", module, scopes_real, message);
#endif /* SRU_DEBUG_CNT */

    g_free (message);
    g_free (scopes_real);
    va_end (args);
}

void
sru_debug_init ()
{
    sru_debug_file = stderr;
    sru_debug_tags = g_ptr_array_new ();
}

void
sru_debug_terminate ()
{
    g_assert (sru_debug_tags);

    g_ptr_array_add (sru_debug_tags, NULL);
    g_strfreev ((gchar**)g_ptr_array_free (sru_debug_tags, FALSE));
}

#endif /* SRU_DEBUG */
