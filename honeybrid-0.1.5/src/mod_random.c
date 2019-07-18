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

/*! \file mod_random.c
 * \brief RANDOM module for honeybrid Decision Engine
 *
 * This module is called by a boolean decision tree to filter attacker randomly
 * using a probability given as an argument
 *
 \author Robin Berthier 2009
 */


#include "modules.h"

/*! mod_random requires the configuration of the following mandatory parameter:
 - "value", to define a basis for the probability to accept the packet, which is 1 out of value
 */

/*! mod_random
 \param[in] args, struct that contain the node and the data to process
 */
void mod_random(struct mod_args *args)
{
    g_printerr("%s Module called\n", H(args->pkt->conn->id));

    unsigned int proba;
    uint32_t selector = 1;
    const uint32_t *value;

    /*! getting the value provided as parameter */
    if ((value = (const uint32_t *)g_hash_table_lookup(args->node->arg, "value")) == NULL)
    {
        /*! We can't decide */
        args->node->result = -1;
        g_printerr("%s Incorrect value parameter: %d\n", H(args->pkt->conn->id),
                *value);
        return;
    }

    if (*value < selector)
    {
        /*! We can't decide */
        args->node->result = -1;
        g_printerr("%s Incorrect value parameter: %d\n", H(args->pkt->conn->id),
                *value);
        return;
    }

    /*! deciding based on a probability of 1 out of "value": */
    proba = (int) (((double) *value) * (rand() / (RAND_MAX + 1.0)));

    if (proba == selector)
    {
        /*! We accept this packet */
        args->node->result = 1;
        g_printerr("%s PACKET MATCH RULE for random(%d)\n",
                H(args->pkt->conn->id), *value);
    }
    else
    {
        /*! We reject this packet */
        args->node->result = 0;
        g_printerr("%s PACKET DOES NOT MATCH RULE for random(%d)\n",
                H(args->pkt->conn->id), *value);
    }
}

