/*
 * This file is part of the honeybrid project.
 *
 * 2007-2009 University of Maryland (http://www.umd.edu)
 * (Written by Robin Berthier <robinb@umd.edu>, Thomas Coquelin <coquelin@umd.edu> and Julien Vehent <julien@linuxwall.info> for the University of Maryland)
 *
 * 2012-2013 University of Connecticut (http://www.uconn.edu)
 * (Extended by Tamas K Lengyel <tamas.k.lengyel@gmail.com>
 *
 * Honeybrid is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/*! \file incpsh_mod.c
 * \brief Packet counter Module for honeybrid Decision Engine
 *
 * This module returns the position of a packet in the connection
 *
 *
 \author Julien Vehent, 2007
 \author Thomas Coquelin, 2008
 */

#include "modules.h"

// Assign Module ID to each available module
typedef enum {

	MOD_INVALID,

	MOD_SOURCE,

	MOD_SOURCE_TIME,

	MOD_RANDOM,

	MOD_YESNO,

	MOD_VMI,

	MOD_CONTROL,

	MOD_BACKPICK_RANDOM,

#ifdef HAVE_CRYPTO
	MOD_HASH,
#endif

#ifdef HAVE_XMPP
	MOD_DIONAEA,
#endif

	__MAX_HONEYBRID_MODULE
} honeybrid_modules_t;

// Define each module's name and main function
// The name can be used in the configuration of a module's function
const struct mod_def module_definitions[__MAX_HONEYBRID_MODULE] = {

	// Initialize all modules to invalid and NULL, to be overwritten by actual definitions
	[0 ... __MAX_HONEYBRID_MODULE-1] = {.name = "invalid", .function = NULL},

	[MOD_SOURCE] = {.name = "source", .function = mod_source},

	[MOD_SOURCE_TIME] = {.name = "source_time", .function = mod_source_time},

	[MOD_RANDOM] = {.name = "random", .function = mod_random},

	[MOD_YESNO] = {.name = "yesno", .function = mod_yesno},

	[MOD_VMI] = {.name = "vmi", .function = mod_vmi},

	[MOD_CONTROL] = {.name = "control", .function = mod_control},

	[MOD_BACKPICK_RANDOM] = {.name = "backpick_random", .function = mod_backpick_random},

#ifdef HAVE_CRYPTO
	[MOD_HASH] = {.name = "hash", .function = mod_hash},
#endif

#ifdef HAVE_XMPP
	[MOD_DIONAEA] = {.name = "hash", .function = mod_hash},
#endif
};

/*! \todo create two functions to handle module backup to file:
 - a function called by modules to add themselves to a backup queue
 - a timer event callback function to process the backup queue periodically, and save backups to files
 */

/*! init_modules
 \brief setup modules that need to be initialized
 */

// TODO: Only init modules/threads here that are actually used by a target
void init_modules() {
	g_printerr("%s Initiate modules\n", H(6));

#ifdef HAVE_CRYPTO
	init_mod_hash();
#endif

#ifdef HAVE_XMPP
	init_mod_dionaea();
#endif

	init_mod_vmi();

	/*! create a thread that will save module memory every minute */
	if ((mod_backup = g_thread_new("module_backup_saver",
			(void *) save_backup_handler, NULL)) == NULL) {
		errx(1, "%s Cannot create a thread to save module memory", H(6));
	}
}

/*! run_module
 * \brief test of a new function to run module based on module name (without calling get_module)
 */

/*void run_module(const module_function module, struct mod_args *args)
{
    if (module && args)
    {
        module(args);
    }
}*/


/*! get_module
 \brief return the module function pointer from name
 \param[in] modname: module name
 \return function pointer to the module
 */
module_function get_module(const char *modname) {

	uint32_t i = 0;
	for (; i < __MAX_HONEYBRID_MODULE; ++i) {
		if (!strncmp(modname, module_definitions[i].name, strlen(module_definitions[i].name))) {
			return module_definitions[i].function;
		}
	}

	errx(1, "%s No module could be found with the name: %s", H(6), modname);

	return NULL;
}

/*! write_backup
 *  \brief This function write a module backup memory to a file
 */

int write_backup(const char *filename, GKeyFile *data, void *userdata)
{
    g_printerr("%s saving backup module %p to %s\n", H(6), data, filename);
    gchar *buf;
    buf = g_key_file_to_data(data, NULL, NULL);

    FILE *file_fd;
    if (NULL == (file_fd = fopen(filename, (char *) "w+")))
    {
        g_printerr(
                "%s Failed to save module backup \"%s\": can't open file for writing\n",
                H(0), filename);
        return FALSE;
    }
    fprintf(file_fd, "%s", buf);
    fclose(file_fd);
    g_free(buf);

    return TRUE;
}

/*! save_backup_handler
 * \brief This function handles the automatic saving of modules to external files.
 * It works by checking an array in which modules to save have been registered
 * This array is updated after each module run
 */
void save_backup_handler() {
	int removed;

	gint64 sleep_cycle;

	while (threading == OK) {
		g_mutex_lock(&threading_cond_lock);
		sleep_cycle = g_get_monotonic_time() + 60 * G_TIME_SPAN_SECOND;
		g_cond_wait_until(&threading_cond, &threading_cond_lock, sleep_cycle);
		g_mutex_unlock(&threading_cond_lock);

		if (module_to_save != NULL) {
			removed = g_hash_table_foreach_steal(module_to_save,
					(GHRFunc) write_backup, NULL);
			g_printerr("%s %d entries saved and removed from module_to_save\n",
					H(0), removed);
		}
	}

	g_thread_exit(0);
}

/*! save_backup
 *  \brief This function adds a module backup memory to a queue in order to be written to a file later
 */
int save_backup(GKeyFile *data, char *filename)
{
    g_printerr("%s called for %p (%s)\n", H(0), data, filename);
    if (FALSE
            == g_hash_table_lookup_extended(module_to_save, filename, NULL,
                    NULL))
    {
        g_printerr("%s adding a new entry in module_to_save\n", H(0));
        g_hash_table_insert(module_to_save, filename, data);
        return 1;
    }
    else
    {
        g_hash_table_replace(module_to_save, filename, data);
        g_printerr("%s module_to_save already had this entry, updated\n", H(0));
        return 0;
    }
}
