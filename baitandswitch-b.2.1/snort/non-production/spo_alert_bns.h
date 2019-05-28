#ifndef __SPO_ALERT_BNS_H__
#define __SPO_ALERT_BNS_H__

#include "snort.h"
#include "event.h"
#include "plugbase.h"

/* not used yet */
typedef struct _SpoAlertBNSData
{
    char *filename;

} SpoAlertBNSData;

/* list of function prototypes for this preprocessor */
void SetupAlertBNS();
void AlertBNSInit(u_char *);
void SpoAlertBNS(Packet *, char *, void *, Event *);
void ParseAlertBNSArgs(char *);
void SpoAlertBNSCleanExitFunc(int, void *);
void SpoAlertBNSRestartFunc(int, void *);

#endif  /* __SPO_ALERT_BNS_H__ */
