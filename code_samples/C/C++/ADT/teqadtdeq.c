/* 
teqadtdeq.c 
Description - Dequeue ADT message from the Transactional Event Queue

To run this .c file 
Step 1. Compile it $ loutl teqadtdeq.c
Step 2. $ make libclntsh.so --To link all the binaries
Step 3 Run it with specifying the username and password of the database user
        $ ./teqadtdeq <user> <password>
*/

#if defined(WIN32COMMON) || defined(WIN32) || defined(_WIN32) 
#include <windows.h>
#endif
#ifndef S_ORACLE
# include<s.h>
#endif
#include <oratypes.h>
#ifndef OCI_ORACLE
#include <oci.h>
#endif
#ifndef OCIJSON_ORACLE
#include <ocijson.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

static void checkerr( OCIError *errhp, sword status);  //Declaration of checkerr() - Check error
static void checkenverr( OCIEnv *envhp, sword status); //Declaration of checkenverr() - Check environment creation error

/*This function would get error for you*/
static void checkerr( OCIError *errhp, sword status) 
{
    /*The first case arise when there is no error 
    in that case we simply return the function*/
    if (status == OCI_SUCCESS){
        return;
    }
    /*If there are any other type of error
    print the error. For OCI_ERROR, we
    have a buffer for storing the error
    text and then the error code*/
    text errbuf[512];  
    sb4 errcode; 
    switch (status)
    {
        case OCI_SUCCESS_WITH_INFO:
            printf("Error - OCI_SUCCESS_WITH_INFO\n");
            break;
        case OCI_NEED_DATA:
            printf("Error - OCI_NEED_DATA\n");
            break;
        case OCI_NO_DATA:
            printf("Error - OCI_NO_DATA\n");
            break;
        case OCI_ERROR:
            OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
                        errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
            printf("Error - %s\n", errbuf);
            exit(1);
        case OCI_INVALID_HANDLE:
            printf("Error - OCI_INVALID_HANDLE\n");
            break;
        case OCI_STILL_EXECUTING:
            printf("Error - OCI_STILL_EXECUTE\n");
            break;
        case OCI_CONTINUE:
            printf("Error - OCI_CONTINUE\n");
            break;
        default:
            printf("Error - %d\n", status);
            break;
    }
}

/*  OCIEnvCreate error checking routine */
static void checkenverr( OCIEnv *envhp, sword status)
{
  text errbuf[512];
  sb4 errcode;

  switch (status)
  {
  case OCI_SUCCESS_WITH_INFO:
    printf("Error - OCI_SUCCESS_WITH_INFO\n");
    break;
  case OCI_ERROR:
    OCIErrorGet ((dvoid *) envhp, (ub4) 1, (text *) NULL, (sb4 *)&errcode,
            errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ENV);
    printf("Error - %s\n", errbuf);
    break;
  case OCI_INVALID_HANDLE:
    printf("Error - OCI_INVALID_HANDLE\n");
    break;
  default:
    printf("Error - %d\n", status);
    break;
  }
}

/* Free up the space that we were using*/
void clean_up(OCIEnv *envhp, 
              OCISvcCtx *svchp, 
              OCIServer *srvhp, 
              OCIError  *errhp, 
              OCISession *usrhp, 
              OCIAQMsgProperties *msgprop)
{
  /* free the message properties decriptor */
  checkerr(errhp, OCIDescriptorFree((void *)msgprop,
                                      OCI_DTYPE_AQMSG_PROPERTIES));

  /* detach from the server */
  checkerr(errhp, OCISessionEnd(svchp, errhp, usrhp, OCI_DEFAULT));
  checkerr(errhp, OCIServerDetach(srvhp, errhp, (ub4)OCI_DEFAULT));

  if (usrhp)
    checkerr(NULL, OCIHandleFree((void *) usrhp, (ub4) OCI_HTYPE_SESSION));
  if (svchp)
    checkerr(NULL, OCIHandleFree((void *) svchp, (ub4) OCI_HTYPE_SVCCTX));
  if (srvhp)
    checkerr(NULL, OCIHandleFree((void *) srvhp, (ub4) OCI_HTYPE_SERVER));
  if (errhp)
    checkerr(NULL, OCIHandleFree((void *) errhp, (ub4) OCI_HTYPE_ERROR));
  if (envhp)
    checkerr(NULL, OCIHandleFree((void *) envhp, (ub4) OCI_HTYPE_ENV));
}

/* Struct for person */
struct person
{
    OCINumber    id;
    OCIString    *name;
};
typedef struct person person;

/* Struct for person message Indicator*/
struct null_person
{
    OCIInd    null_adt;
    OCIInd    null_id;
    OCIInd    null_name;
};
typedef struct null_person null_person;

int main(int argc, char *argv[])
{
  OCIEnv             *envhp = (OCIEnv *)NULL;                     /* Environment Handle */
  OCIServer          *srvhp = (OCIServer *)NULL;                  /* Server Handle */
  OCIError           *errhp = (OCIError *)NULL;                   /* Error Handle */
  OCISvcCtx          *svchp = (OCISvcCtx *)NULL;                  /* Service Context Handle */
  OCISession         *usrhp = (OCISession *)NULL;                 /* Session Handle */
  OCIType         *mesg_tdo = (OCIType *) NULL;                   /* Message Type Handle */
  OCIAQMsgProperties *msgprop  = (OCIAQMsgProperties *)NULL;      /* Message Properties Descriptor */
  person              *mesg = (person *)NULL;                     /* ADT Payload Pointer */
  null_person        *nmesg = (null_person *)NULL;                /* ADT Payload Indicator Pointer*/
  sword               status = 0;                                 /* Error status*/
  char usrnm[256];                                                /* Username of size 256 */
  char *pass;                                                     /* password string pointer */ 
  /* Copy the argument passed to the username*/
  strncpy(usrnm, argv[1], sizeof(usrnm) - 1);
  /* Ensure that the string is NULL-terminated*/
  usrnm[sizeof(usrnm) - 1] = '\0';
  /* Assign the password */
  pass = argv[2];

  /* Standard OCI initialization  */

  /* OCI Environment creation*/
  status = OCIEnvCreate((OCIEnv **) &envhp, (ub4) OCI_OBJECT,
                (void *) 0, (void * (*)(void *,size_t)) 0,
                (void * (*)(void *, void *, size_t)) 0,
                (void (*)(void *, void *)) 0, (size_t) 0,
                (void **) 0);
  /* Error Occurence */
  if (status != 0) {
    printf("FAILED: OCIEnvCreate() with status = %d.\n", status);
    checkenverr(envhp,status);
    return -1;
  }

  /* allocate a error report handle */
  checkerr(NULL, OCIHandleAlloc( (void *) envhp, (void **) &errhp,
                  (ub4) OCI_HTYPE_ERROR, 0, (void **) 0));

  /* allocate a server handle */
  checkerr(NULL, OCIHandleAlloc( (void *) envhp, (void **) &srvhp,
                  (ub4) OCI_HTYPE_SERVER, 0, (void **) 0));
  /* Create an association between the server and the oci application */
  OCIServerAttach( srvhp, errhp, (text *) 0, (sb4) 0, (ub4) OCI_DEFAULT);

  /* allocate a service context handle */
  checkerr(NULL, OCIHandleAlloc( (void *) envhp, (void **) &svchp,
                  (ub4) OCI_HTYPE_SVCCTX, 0, (void **) 0));
  /* set attribute server context in the service context */
  OCIAttrSet( (dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX, (dvoid *)srvhp,
              (ub4) 0 , (ub4) OCI_ATTR_SERVER, (OCIError *) errhp);

  /* allocate a user session handle */
  checkerr(NULL, OCIHandleAlloc((dvoid *)envhp, (dvoid **)&usrhp,
                  (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0));
  /* set attribute username in the user name */
  OCIAttrSet( (dvoid *)usrhp, (ub4)OCI_HTYPE_SESSION, (dvoid *)usrnm,
              (ub4)strlen(usrnm), OCI_ATTR_USERNAME, errhp);
  /* set attribute password in user password */
  OCIAttrSet( (dvoid *)usrhp, (ub4)OCI_HTYPE_SESSION,
              (dvoid *)pass, (ub4)strlen(pass),
              OCI_ATTR_PASSWORD, errhp);
  /* Start the user session*/
  checkerr(errhp, OCISessionBegin (svchp, errhp, usrhp, OCI_CRED_RDBMS,
          OCI_DEFAULT));
  /* set attribute session user context in the service context */
  OCIAttrSet( (dvoid *)svchp, (ub4)OCI_HTYPE_SVCCTX,
              (dvoid *)usrhp, (ub4)0, OCI_ATTR_SESSION, errhp);

  /* Get the ADT Message Type*/
  checkerr(errhp, OCITypeByName(envhp, errhp, svchp,
          (CONST text *)"TEQUSER", (ub4)strlen("TEQUSER"),
          (CONST text *)"PERSON", (ub4)strlen("PERSON"), (text *)0, 0,
          OCI_DURATION_SESSION, OCI_TYPEGET_ALL, &mesg_tdo));

  /* allocate message properties descriptor */
  checkerr(errhp, OCIDescriptorAlloc(envhp, (dvoid **)&msgprop,
          OCI_DTYPE_AQMSG_PROPERTIES, 0, (dvoid **)0));

  /* Construct the queue name as <SCHEMA.QUEUE_NAME> */
    char queuename[100];
  strcpy(queuename , usrnm); 
  strcat(queuename, ".adt_queue_teq");
  printf("Dequeueing...\n");

  /* Enqueue the message */
  checkerr(errhp, OCIAQDeq(svchp, errhp, (text *) queuename, 
        (OCIAQDeqOptions *)0, (OCIAQMsgProperties *)msgprop,
        mesg_tdo, (dvoid **)&mesg, (dvoid **)&nmesg, (OCIRaw **)0, 0));

  /* Commit to see the enqueue message in the queue */
  checkerr(errhp, OCITransCommit(svchp, errhp, (ub4) 0));
  printf("Dequeue Done.\n");
  int id;
  checkerr(errhp,OCINumberToInt(errhp, &mesg->id, sizeof(id), OCI_NUMBER_SIGNED, &id));
  char *name = (char *)OCIStringPtr(envhp, mesg->name);
  printf("The message is id = %d, name = %s\n", id, name);
  /* Clean up the handles*/
  clean_up(envhp, svchp, srvhp, errhp, usrhp, msgprop);
  exit (0);
}