/* spo_alert_bns
 * 
 * Purpose:  Output IP address via FIFO for BNS switchcore to poll
 *
 * Effect:	???
 *
 */

#define DEBUG
#define USE_THREADS
#include "alert_bns.vars"
#define TRUE 1
#define FALSE 0

/* output plugin header file */
#include "spo_alert_bns.h"
#include "snort.h"
#include "event.h"

#include <stdlib.h>
#include <unistd.h>
#ifdef USE_THREADS
#include <pthread.h>
#endif
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

/* external globals from rules.c */
extern char *file_name;
extern int file_line;

char		ScriptName[1024];

#ifdef USE_THREADS
typedef struct ip_holder{
	struct in_addr		IP;
	struct ip_holder*	next;
} IPHold;

IPHold*  		bns_alert_head;
pthread_mutex_t	bns_alert_mutex = PTHREAD_MUTEX_INITIALIZER;
char			exit_bns_thread;
pthread_t		alert_thread_id;

void* BNSCallerFunc(void* v);
void BNSCleanExitFunction(int signal, void* arg);
void BNSRestartFunction(int signal, void* arg);
#endif

void SetupAlertBNS()
{
    /* link the preprocessor keyword to the init function in 
       the preproc list */
    RegisterOutputPlugin("alert_bns", NT_OUTPUT_ALERT, AlertBNSInit);

#ifdef DEBUG
    printf("Output plugin: AlertBNS is setup...\n");
#endif
}


void AlertBNSInit(u_char *args)
{
#ifdef DEBUG
    printf("Output: AlertBNS Initialized\n");
#endif

    pv.alert_plugin_active = 1;

    /* parse the argument list from the rules file */
    ParseAlertBNSArgs(args);


#ifdef DEBUG
    printf("Linking BNSAlert functions to call lists...\n");
#endif

#ifdef USE_THREADS
	bns_alert_head=NULL;
	exit_bns_thread=FALSE;
	
	pthread_create(&alert_thread_id, NULL, BNSCallerFunc, NULL);
	
    AddFuncToCleanExitList(BNSCleanExitFunction, NULL);
    AddFuncToRestartList(BNSRestartFunction, NULL);
#endif	

    /* Set the preprocessor function into the function list */
    AddFuncToOutputList(SpoAlertBNS, NT_OUTPUT_ALERT, NULL);
}


void ParseAlertBNSArgs(char *args)
{
#ifdef DEBUG
    printf("ParseAlertBNSArgs: %s\n", args);
#endif

	/*TODO: make ScriptName not a global*/
	
	snprintf(ScriptName, sizeof ScriptName, args);
	
#ifdef DEBUG
	printf("ScriptName is now %s\n",ScriptName);
#endif	
}

/****************************************************************************
*  use the system() call to execute a shell script
****************************************************************************/
void CallScript(struct in_addr ip){
/*********Jacks Changes*********/

	FILE *fp;

	if((fp = fopen(FIFO_FILE, "w")) == NULL)
       	{
        	printf("fifo fucked up");
        	exit(1);
	}

	fprintf (fp, "%s", inet_ntoa(ip));

	fclose(fp);
	printf("sending data: %s\n", inet_ntoa(ip));

/********End Changes************/

}

/****************************************************************************
 *
 * Function: SpoBNSAlert(Packet *, char *)
 *
 * Arguments: p => pointer to the packet data struct
 *            msg => the message to print in the alert
 *
 * Returns: void function
 *
 ***************************************************************************/
void SpoAlertBNS(Packet *p, char *msg, void *arg, Event *event)
{
#ifndef USE_THREADS
      printf("HEEEEEEEYYYYY");
	CallScript(p->iph->ip_src);
#else
	IPHold*	this;
	
	/*put this on the list to be called*/
	pthread_mutex_lock(&bns_alert_mutex);
	if (!bns_alert_head){
		bns_alert_head=calloc(sizeof(IPHold), 1);	
		bns_alert_head->IP = p->iph->ip_src;
	}else{
		/*Seek to the end.  Hopefully this will be short*/
		this = bns_alert_head;
		while (this->next) this=this->next;
		
		/*stick the new one on the end*/
		this->next=calloc(sizeof(IPHold),1);
		this->next->IP = p->iph->ip_src;
	}
	pthread_mutex_unlock(&bns_alert_mutex);	
#endif	
}

#ifdef USE_THREADS
/************************************************************
* This runs in the other thread to actually do the shell
* calling.
************************************************************/
void* BNSCallerFunc(void* v){
	IPHold*		e;
	
	while (!exit_bns_thread){
		e=NULL;
		pthread_mutex_lock(&bns_alert_mutex);
		if (bns_alert_head){
			/*pop the entry off*/
			e=bns_alert_head;
			bns_alert_head = bns_alert_head->next;
		}
		pthread_mutex_unlock(&bns_alert_mutex);
		
		if (e){
			/*call the script*/
			CallScript(e->IP);
			
			/*erase this*/
			free(e);
			e=NULL;
		}
		
		usleep(10);
	}
	
	return NULL;
}

void BNSCleanExitFunction(int signal, void* arg){
#ifdef DEBUG
	printf("Exiting BNS Output...\n");
#endif

	/*exit the logger thread*/
	exit_bns_thread=TRUE;
	
	/*wait for the other thread to exit*/
	pthread_join(alert_thread_id, NULL);
	
#ifdef DEBUG
	printf("Done Exiting BNS Output...\n");
#endif	
}

void BNSRestartFunction(int signal, void* arg){
#ifdef DEBUG
	printf("Restarting BNS Output...\n");
#endif

	/*exit the logger thread*/
	exit_bns_thread=TRUE;
	
	/*wait for the other thread to exit*/
	pthread_join(alert_thread_id, NULL);
	
#ifdef DEBUG
	printf("Done Restarting BNS Output...\n");
#endif	
}

#endif
