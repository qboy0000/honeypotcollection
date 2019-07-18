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

/*! \file log.c
 \brief Log function for honeybrid

 \author Julien Vehent, 2007
 \author Thomas Coquelin, 2008
 */

#include "log.h"

#include <fcntl.h>
#include <glib/gprintf.h>

#include "globals.h"
#include "structs.h"
#include "convenience.h"

#ifdef HAVE_MYSQL
#include <mysql.h>

MYSQL *mysqlConn;
#endif

/*!
 \def last_rotation
 \brief last timestamp the log file was rotated
 */
unsigned long last_rotation;

/*! 
 \Def file descriptor to log connections
 */

FILE *logfd;

/*! log_header
 *
 *\brief return a header for debug log messages, including
 * the timestamp and the name of the function
 */
const char*
log_header(const char* function_name, int id) {
	static char log_header[200];
	snprintf(log_header, 200, "%s;%6u;%s:\t", now(), id, function_name);
	return log_header;
}

/*! now
 *
 *\brief return the current timestamp as a string
 */
const char*
now(void) {
	static char now[30];
	struct tm *tm;
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	if (tm == NULL) {
		perror("localtime");
		return '\0';
	}
	snprintf(now, 30, "%d-%02d-%02d %02d:%02d:%02d.%.6d", (1900 + tm->tm_year),
			(1 + tm->tm_mon), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
			(int) tv.tv_usec);
	return now;
}

/*! honeylog
 *
 \brief add a log entry in the singly linked list
 *
 \param[in] static string pointer
 \param[in] dynamicaly allocated string pointer
 \param[in] log level of event
 \param[in] connection id
 *
 \return OK when done, NOK on failure
 */
status_t honeylog(char *sdata, char *ddata, log_verbosity_t level, unsigned id) {
	/*!filter events upon their log level*/
	if (level > LOG_LEVEL) {
		if (ddata != NULL)
			free(ddata);
	} else {
		struct tm *tm;
		struct timeval tv;
		struct timezone tz;
		struct log_event *event = (struct log_event *) malloc(
				sizeof(struct log_event));
		gettimeofday(&tv, &tz);
		tm = localtime(&tv.tv_sec);
		if (tm == NULL) {
			perror("localtime");
			return NOK;
		}
		event->sdata = sdata;
		event->ddata = ddata;
		event->level = level;
		event->id = id;
		event->curtime = malloc(30);
		sprintf(event->curtime, "%d-%02d-%02d %02d:%02d:%02d.%.6d",
				(1900 + tm->tm_year), (1 + tm->tm_mon), tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec, (int) tv.tv_usec);

		if (event->sdata != NULL && event->ddata == NULL) {
			g_print("%s;id:%5u;%s", event->curtime, event->id, event->sdata);
		} else if (event->sdata == NULL && event->ddata != NULL) {
			g_print("%s;id:%5u;%s", event->curtime, event->id, event->ddata);
			g_free(event->ddata);
		} else {
			g_print("%s;id:%5u;honeylog(): Incorrect event!!\n", event->curtime,
					event->id);
		}
		g_free(event->curtime);
		g_free(event);
	}
	return OK;
}

int open_debug_log(void) {
	int fd;
	if (0 != chdir(g_hash_table_lookup(config, "log_directory")))
		errx(1, "%s: can't change directory", __func__);
	if (NULL == g_hash_table_lookup(config, "debug_file"))
		errx(1, "%s: no log file specified in the config", __func__);
	if ((fd = open(g_hash_table_lookup(config, "debug_file"),
			O_CREAT | O_WRONLY | O_APPEND, 0744)) == -1)
		err(1, "%s: open", __func__);
	if (0 != chdir(g_hash_table_lookup(config, "exec_directory")))
		warnx("%s: can't change directory", __func__);
	return fd;
}

int close_connection_log(void) {
	return fclose(logfd);
}

/*! open log file
 \brief open the file honeybrid.log
 */
void open_connection_log(void) {
	if (0 != chdir(CONFIG("log_directory"))) {
		errx(1, "%s: can't change directory", __func__);
	}

	if (NULL == CONFIG("log_file")) {
		errx(1, "%s: no log file specified in the config", __func__);
	}
	if (NULL == (logfd = fopen(CONFIG("log_file"), (char *) "a"))) {
		err(1, "fopen");
	}

	/*! Enable line buffer */
	setlinebuf(logfd);

	if (0 != chdir(g_hash_table_lookup(config, "exec_directory"))) {
		warnx("%s: can't change directory", __func__);
	}
}

/*! rotate_connection_log
 *\brief rotate the file honeybrid.log every hour
 */
//void rotate_connection_log(int signal_nb, void *siginfo, void *context)
void rotate_connection_log(int signal_nb) {
	L("rotate_connection_log()\tCalled\n", NULL, LOG_ALL, LOG_LOG);

	unsigned long timestamp;
	//char *logfile_name;
	//char *new_name;
	GString *logfile_name;
	GString *new_name;
	char *logbuf;

	struct tm *tm;
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	if (tm == NULL) {
		perror("localtime");
		return;
	}

	timestamp = ((tm->tm_year) * (1000000) + (1 + tm->tm_mon) * (10000)
			+ (tm->tm_mday) * (100) + (tm->tm_hour));

	if (last_rotation == 0) {
		last_rotation = timestamp;

		logbuf = malloc(256);
		sprintf(logbuf,
				"rotate_connection_log()\tlast_rotation initialized to %lu\n",
				last_rotation);
		L(NULL, logbuf, 4, LOG_LOG);

		return;
	}

	if (timestamp > last_rotation || signal_nb > 0) {
		if (signal_nb > 0) {
			L("rotate_connection_log()\tSIGUSR1 received, rotating log...\n",
					NULL, LOG_MED, LOG_LOG);
		} else {
			L("rotate_connection_log()\tTime to rotate the log...\n", NULL,
					LOG_MED, LOG_LOG);
		}

		fclose(logfd);

		//logfile_name = malloc(sizeof(g_hash_table_lookup(config,"log_file"))+1);
		//logfile_name = malloc(512);
		///logfile_name = g_new0(char, 256);
		///logfile_name = g_hash_table_lookup(config,"log_file");
		///new_name = malloc(sizeof(logfile_name) + 64);
		///sprintf(new_name,"%s.%d%02d%02d_%02d%02d", logfile_name, (1900+tm->tm_year), (1+tm->tm_mon), tm->tm_mday, tm->tm_hour, tm->tm_min);
		logfile_name = g_string_new(g_hash_table_lookup(config, "log_file"));
#ifdef DEBUG
		g_print("rotate_connection_log()\tlogfile_name is %s\n",
				logfile_name->str);
#endif

		new_name = g_string_new("");
		g_string_printf(new_name, "%s.%d%02d%02d_%02d%02d", logfile_name->str,
				(1900 + tm->tm_year), (1 + tm->tm_mon), tm->tm_mday,
				tm->tm_hour, tm->tm_min);

		logbuf = malloc(512);
		sprintf(logbuf,
				"rotate_connection_log()\tRotating log file from %s to %s\n",
				logfile_name->str, new_name->str);
		L(NULL, logbuf, LOG_HIGH, LOG_LOG);


		if(chdir(g_hash_table_lookup(config, "log_directory")) < 0)
			errx(1, "Failed to chdir to log_directory!\n");

		if (rename(logfile_name->str, new_name->str)) {
			L("rotate_connection_log()\tERROR: can't rename log file!\n", NULL,
					LOG_MED, LOG_LOG);
		}

		//i = open(logfile_name, O_RDWR | O_CREAT, 0640);
		logfd = fopen(g_hash_table_lookup(config, "log_file"), (char *) "a");

		if(chdir(g_hash_table_lookup(config, "exec_directory")) < 0)
			errx(1, "Failed to chdir to exec_directory!\n");

		///g_free(logfile_name);
		///free(new_name);
		g_string_free(logfile_name, TRUE);
		g_string_free(new_name, TRUE);

		/*! The last step is to update last_rotation
		 */
		last_rotation = timestamp;

		logbuf = g_malloc0(256);
		sprintf(logbuf,
				"rotate_connection_log()\tLog file re-opened. last_rotation updated to %lu\n",
				last_rotation);
		L(NULL, logbuf, LOG_HIGH, LOG_LOG);

		free(logbuf);

	}

	return;
}

const char *custom_conn_data(GSList *list) {
	static char custom_conn_data[128];
	GString *buff = g_string_new("");

	while (list != NULL) {

		struct custom_conn_data *custom = (struct custom_conn_data *) g_slist_nth(
				list, 0);

		if (custom && custom->data && custom->data_print) {
			buff = g_string_append(buff, custom->data_print(custom->data));
		}

		list = g_slist_next(list);
	}

	snprintf(custom_conn_data, 128, "%s", buff->str);
	g_string_free(buff, TRUE);

	return custom_conn_data;

}

/*! connection_stat
 *\brief compile a single line of final statistics for every connection handled by honeybrid:
 * Basic flow information: start timestamp, source IP, source Port, destination IP, destination Port, protocol, cumulative flags if TCP
 * INVALID information: duration, reason, packet, byte
 * INIT information: duration, packet, byte
 * DECIDE information: duration, rule, packet id, high interaction ip and port
 * REPLAY information: duration, problem, packet, byte
 * FORWARD information: duration, packet, byte
 */

void connection_log(const struct conn_struct *conn) {

	/*! if log rotation is configured, then we call rotate_connection_log()
	 */
	if (ICONFIG("log_rotation") == 1) {
		rotate_connection_log(0);
	}

	GString *proto = g_string_new("");
	switch (conn->protocol) {
	case 6:
		g_string_printf(proto, "TCP");
		break;
	case 17:
		g_string_printf(proto, "UDP");
		break;
	default:
		g_string_printf(proto, "%d", conn->protocol);
		break;
	}

	GString *status = g_string_new("");
	switch (conn->state) {
	case INIT:
		g_string_printf(status, "INIT");
		break;
	case DECISION:
		g_string_printf(status, "DECISION");
		break;
	case REPLAY:
		g_string_printf(status, "REPLAY");
		break;
	case FORWARD:
		g_string_printf(status, "FORWARD");
		break;
	case PROXY:
		g_string_printf(status, "PROXY");
		break;
	case DROP:
		g_string_printf(status, "DROP");
		break;
	case CONTROL:
		g_string_printf(status, "CONTROL");
		break;
	default:
		g_string_printf(status, "INVALID");
		break;
	}

	gint i;
	GString *status_info[6];
	gdouble lasttime = conn->start_microtime;
	gdouble duration = 0.0;
	for (i = INIT; i <= PROXY; i++) {
		status_info[i] = g_string_new("");
		if (i <= conn->state) {
			if (conn->stat_time[i] > 0) {
				duration = (conn->stat_time[i] - lasttime);
				lasttime = conn->stat_time[i];
			} else {
				duration = 0.0;
			}
			if (i == REPLAY && conn->replay_problem > 0) {
				g_string_printf(status_info[i], "%.3f|%d|%d|error:%d", duration,
						conn->stat_packet[i], conn->stat_byte[i],
						conn->replay_problem);
			} else if (i == DECISION) {
				g_string_printf(status_info[i], "%.3f|%s", duration,
						conn->decision_rule->str);
			} else {
				g_string_printf(status_info[i], "%.3f|%d|%d", duration,
						conn->stat_packet[i], conn->stat_byte[i]);
			}
		} else {
			if (i == REPLAY)
				g_string_printf(status_info[i], ".|.|.|.");
			else if (i == DECISION)
				g_string_printf(status_info[i], ".|.");
			else
				g_string_printf(status_info[i], ".|.|.");
		}
	}

	gdouble total_duration = (lasttime - conn->start_microtime);
	output_t output = (output_t) ICONFIG_REQUIRED("output");

	/*! Output according to the format configured */
	if (output == OUTPUT_MYSQL) {
		log_mysql(conn, proto, status, status_info, total_duration);
	} else if (output == OUTPUT_STDOUT || output == OUTPUT_LOGFILES) {
		if (NULL != CONFIG("log_format")
				&& !strcmp(CONFIG("log_format"), "csv")) {
			log_csv(conn, proto, status, status_info, total_duration, output);
		} else {
			log_std(conn, proto, status, status_info, total_duration, output);
		}
	}

	g_string_free(proto, TRUE);
	g_string_free(status, TRUE);
	g_string_free(status_info[INIT], TRUE);
	g_string_free(status_info[DECISION], TRUE);
	g_string_free(status_info[REPLAY], TRUE);
	g_string_free(status_info[FORWARD], TRUE);
	g_string_free(status_info[PROXY], TRUE);

}

status_t log_csv(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration,
		output_t output) {

	gchar **tuple;
	tuple = g_strsplit(conn->key, ":", 0);

	int log_size = snprintf(NULL, 0,
#ifdef HAVE_XMPP
			"%s,%.3f,%d,%s,%s,%s,%s,%s,%d,%d,%s,%d,%s,%s,%s,%s,%s,%s,%i,%i\n",
#else
			"%s,%.3f,%d,%s,%s,%s,%s,%s,%d,%d,%s,%d,%s,%s,%s,%s,%s,%s\n",
#endif
			conn->start_timestamp->str, duration, conn->uplink_mark, proto->str,
			tuple[0], tuple[1], tuple[2], tuple[3], conn->total_packet,
			conn->total_byte, status->str, conn->id,
			//status_info[INVALID]->str,
			status_info[INIT]->str, status_info[DECISION]->str,
			status_info[REPLAY]->str, status_info[FORWARD]->str,
			status_info[PROXY]->str,
			(conn->custom_data ? custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
			,
			conn->dionaeaDownload,
			conn->dionaeaDownloadTime
#endif
			);

	char *logbuf = g_malloc0(log_size + 1);

	sprintf(logbuf,
#ifdef HAVE_XMPP
			"%s,%.3f,%d,%s,%s,%s,%s,%s,%d,%d,%s,%d,%s,%s,%s,%s,%s,%s,%i,%i\n",
#else
			"%s,%.3f,%d,%s,%s,%s,%s,%s,%d,%d,%s,%d,%s,%s,%s,%s,%s,%s\n",
#endif
			conn->start_timestamp->str, duration, conn->uplink_mark, proto->str,
			tuple[0], tuple[1], tuple[2], tuple[3], conn->total_packet,
			conn->total_byte, status->str, conn->id,
			//status_info[INVALID]->str,
			status_info[INIT]->str, status_info[DECISION]->str,
			status_info[REPLAY]->str, status_info[FORWARD]->str,
			status_info[PROXY]->str,
			(conn->custom_data ? custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
			,
			conn->dionaeaDownload,
			conn->dionaeaDownloadTime
#endif
			);

	if (output == OUTPUT_STDOUT)
		printf("%s", logbuf);
	else if (output == OUTPUT_LOGFILES)
		fprintf(logfd, "%s", logbuf);

	free(logbuf);
	g_strfreev(tuple);

	return OK;
}

status_t log_std(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration,
		output_t output) {

	gchar **tuple;
	tuple = g_strsplit(conn->key, ":", 0);

	int log_size =
			snprintf(NULL, 0,
#ifdef HAVE_XMPP
					"%s %.3f %d %s %s:%s -> %s:%s %d %d %s ** %d %s %s %s %s %s [%s] | %i %i\n",
#else
					"%s %.3f %d %s %s:%s -> %s:%s %d %d %s ** %d %s %s %s %s %s [%s]\n",
#endif
					conn->start_timestamp->str, duration, conn->uplink_mark,
					proto->str, tuple[0], tuple[1], tuple[2], tuple[3],
					conn->total_packet, conn->total_byte, status->str, conn->id,
					//status_info[INVALID]->str,
					status_info[INIT]->str, status_info[DECISION]->str,
					status_info[REPLAY]->str, status_info[FORWARD]->str,
					status_info[PROXY]->str,
					(conn->custom_data ?
							custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
					,
					conn->dionaeaDownload,
					conn->dionaeaDownloadTime
#endif
					);

	char *logbuf = g_malloc0(log_size + 1);

	sprintf(logbuf,
#ifdef HAVE_XMPP
			"%s %.3f %d %s %s:%s -> %s:%s %d %d %s ** %d %s %s %s %s %s [%s] | %i %i \n",
#else
			"%s %.3f %d %s %s:%s -> %s:%s %d %d %s ** %d %s %s %s %s %s [%s]\n",
#endif
			conn->start_timestamp->str, duration, conn->uplink_mark, proto->str,
			tuple[0], tuple[1], tuple[2], tuple[3], conn->total_packet,
			conn->total_byte, status->str, conn->id,
			//status_info[INVALID]->str,
			status_info[INIT]->str, status_info[DECISION]->str,
			status_info[REPLAY]->str, status_info[FORWARD]->str,
			status_info[PROXY]->str,
			(conn->custom_data ? custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
			,
			conn->dionaeaDownload,
			conn->dionaeaDownloadTime
#endif
			);

	if (output == OUTPUT_STDOUT)
		printf("%s", logbuf);
	else
		g_fprintf(logfd, "%s", logbuf);

	free(logbuf);
	g_strfreev(tuple);

	return OK;
}

#ifdef HAVE_MYSQL
status_t init_mysql_log() {
	if (mysqlConn == NULL) {

		mysqlConn = mysql_init(NULL);
		my_bool reconnect = 1;
		mysql_options(mysqlConn, MYSQL_OPT_RECONNECT, &reconnect);

		if (!mysql_real_connect(mysqlConn, CONFIG_REQUIRED("mysql_host"),
				CONFIG_REQUIRED("mysql_user"),
				CONFIG_REQUIRED("mysql_password"), CONFIG_REQUIRED("mysql_db"),
				ICONFIG_REQUIRED("mysql_port"), NULL, 0)) {
			g_printerr("Failed to connect to MySQL database: Error: %s\n",
					mysql_error(mysqlConn));

			//TODO: init c_id here from database

			return OK;
		}

		return NOK;

	} else {
		if (mysql_ping(mysqlConn)) {
			g_printerr(
					"Connection to the MySQL has gone away database: Error: %s\n",
					mysql_error(mysqlConn));

			mysql_close(mysqlConn);
			return OK;
		}

		return NOK;
	}
}

status_t log_mysql(const struct conn_struct *conn, const GString *proto,
		const GString *status, GString **status_info, gdouble duration) {

	status_t ret = OK;

	if (init_mysql_log() == 0) {

		gchar **tuple;
		tuple = g_strsplit(conn->key, ":", 0);

		int log_size = snprintf(NULL, 0, "INSERT INTO honeybrid VALUES ("
				"'',"
				"%.3f, %.3f, %i,"
				"'%s', '%s', '%s',"
				"'%s', '%s', %d,"
				"%d, '%s', %d,"
				"'%s', '%s', '%s',"
				"'%s', '%s',"
				"%s"
#ifdef HAVE_XMPP
				",%i,%i"
#endif
				");", conn->start_microtime, duration, (int) conn->uplink_mark,
				proto->str, tuple[0], tuple[1], tuple[2], tuple[3],
				conn->total_packet, conn->total_byte, status->str, conn->id,
				status_info[INIT]->str, status_info[DECISION]->str,
				status_info[REPLAY]->str, status_info[FORWARD]->str,
				status_info[PROXY]->str,
				(conn->custom_data ? custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
				conn->dionaeaDownload, conn->dionaeaDownloadTime
#endif
				);

		char *logbuf = g_malloc0(log_size + 1);

		sprintf(logbuf, "INSERT INTO honeybrid VALUES ("
				"'',"
				"%.3f, %.3f, %i,"
				"'%s', '%s', '%s',"
				"'%s', '%s', %d,"
				"%d, '%s', %d,"
				"'%s', '%s', '%s',"
				"'%s', '%s',"
				"%s"
#ifdef HAVE_XMPP
				",%i,%i"
#endif
				");", conn->start_microtime, duration, (int) conn->uplink_mark,
				proto->str, tuple[0], tuple[1], tuple[2], tuple[3],
				conn->total_packet, conn->total_byte, status->str, conn->id,
				status_info[INIT]->str, status_info[DECISION]->str,
				status_info[REPLAY]->str, status_info[FORWARD]->str,
				status_info[PROXY]->str,
				(conn->custom_data ? custom_conn_data(conn->custom_data) : "-")
#ifdef HAVE_XMPP
				,conn->dionaeaDownload, conn->dionaeaDownloadTime
#endif
				);

		if (mysql_query(mysqlConn, logbuf)) {
			g_printerr("Logging to MySQL failed: %s\n", mysql_error(mysqlConn));
			ret = NOK;
		}

		g_strfreev(tuple);

	}

	return ret;
}
#else
int init_mysql_log() {
	return NOK;
}

status_t log_mysql(const struct conn_struct *conn, const GString *proto, const GString *status, GString **status_info, gdouble duration) {
	return NOK;
}
#endif
