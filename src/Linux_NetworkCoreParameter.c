/*
 * Linux_NetworkCoreParameter.c
 *
 * (C) Copyright IBM Corp. 2003, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.txt
 *
 * Author:       Gareth S Bestor <bestorga@us.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: CMPI provider to expose the Linux kernel parameters
 *		under /proc/sys/net/core
 *		Kernel parameter files may be listed, and the string value of
 *		each kernel parameter can be read. The value of a kernel
 *		parameter can also be modified if the associated file
 *		has write permission enabled.
 */

/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*//* private declarations                                                       */#ifdef CMPI_VER_100
#define Linux_NetworkCoreParameterSetInstance Linux_NetworkCoreParameterModifyInstance
#endif

static char * _FILEROOT = "/proc/sys/net/core";
static const char * _CLASSNAME = "Linux_NetworkCoreParameter";

/* ---------------------------------------------------------------------------*/

/*
 * Cleanup
 */
CMPIStatus Linux_NetworkCoreParameterCleanup(
		CMPIInstanceMI * mi, 
		const CMPIContext * context,
		CMPIBoolean terminate) 
{
   _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_CLASSNAME));
   /* Nothing needs to be done for cleanup */

   _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_CLASSNAME));
   CMReturn(CMPI_RC_OK);
}

/* --------------------------------------------------------------------------- */

/*
 * EnumInstanceNames
 */
CMPIStatus Linux_NetworkCoreParameterEnumInstanceNames(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref) 
{
   CMPIObjectPath * objectpath;	/* New object path to store each new kernel parameter */
   CMPIStatus status = {CMPI_RC_OK, NULL};	/* Return status of CIM operations */

   char tmpfilename[L_tmpnam];	/* tmp file storing the list of kernel parameter filenames */
   FILE * tmpfile = NULL;	/* tmp file handle */
   char filename[1024];		/* string buffer to read in each filename */
   char findcommand[1024];	/* Unix find command to find the list of kernel parameter */
   char * hostname = NULL;	/* Name of this machine */

   _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_CLASSNAME));
  
   /* Need this machine's hostname to set CSName attributes */
   hostname = get_system_name();

   /* Find all the kernel parameter filenames under the target directory */
   if (tmpnam(tmpfilename)) {
      sprintf(findcommand, "find %s -type f > %s", _FILEROOT, tmpfilename);
      if (system(findcommand) == 0) {
         tmpfile = fopen(tmpfilename,"r");
      }
   }

   /* Go through the list of kernel parameter filenames and create a new object path for each one */
   while (tmpfile && fscanf(tmpfile, "%s\n", filename) != EOF) {

      _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() : adding object path for %s",_CLASSNAME,filename));

      /* Create a new object path for this kernel parameter file */
      objectpath = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ref,&status)),
		      _CLASSNAME,  &status);
      if (CMIsNullObject(objectpath)) {
         CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Failed to create new object path." );
	 _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
         CMReturn(CMPI_RC_ERR_FAILED);
      }

      /* Store this kernel parameter's attributes in the newly created object path */
      CMAddKey(objectpath, "CSCreationClassName", CSCreationClassName, CMPI_chars);
      CMAddKey(objectpath, "CSName", hostname, CMPI_chars);
      CMAddKey(objectpath, "CreationClassName", _CLASSNAME, CMPI_chars);
      CMAddKey(objectpath, "SettingID", filename, CMPI_chars);

      /* Add this new kernel parameter object path to the list of results */
      CMReturnObjectPath(results, objectpath);
   }
   if (tmpfile != NULL) {
      fclose(tmpfile);
   }
   remove(tmpfilename);

   /* Finished returning all the kernel parameter names */
   CMReturnDone(results);
   _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_CLASSNAME));
   CMReturn(CMPI_RC_OK);
}


/*
 * EnumInstances
 */
CMPIStatus Linux_NetworkCoreParameterEnumInstances(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref, 
		const char ** properties) 
{
   CMPIObjectPath * objectpath;	/* New object path to store each new kernel parameter */
   CMPIInstance * instance;	/* New instacne to store each new kernel parameter */
   CMPIValue value;		/* Generic CIM value to store edittable flag */
   CMPIStatus status = {CMPI_RC_OK, NULL};      /* Return status of CIM operations */

   char tmpfilename[L_tmpnam];  /* tmp file storing the list of kernel parameter filenames */
   FILE * tmpfile = NULL;       /* tmp file handle */
   char filename[1024];         /* string buffer to read in each filename */
   char findcommand[1024];      /* Unix find command to find the list of kernel parameter */
   char * hostname = NULL;	/* Name of this machine */
   FILE * paramfile = NULL;	/* file handle for each kernel parameter file */
   char paramvalue[1024] = "";	/* string buffer to read in contents of kernel parameter file */ 
   struct stat fileinfo;	/* stat() structure to read in file permission bits */
   char * charptr = NULL;	/* string pointer to walk thru paramvalue */
   
   _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_CLASSNAME));

   /* Need this machine's hostname to set CSName attributes */
   hostname = get_system_name();

   /* Find all the kernel parameter filenames under the target directory */
   if (tmpnam(tmpfilename)) {
      sprintf(findcommand, "find %s -type f > %s", _FILEROOT, tmpfilename);
      if (system(findcommand) == 0) {
         tmpfile = fopen(tmpfilename,"r");
      }
   }

   /* Go through the list of kernel parameter filenames and create a new instance for each one */
   while (tmpfile && fscanf(tmpfile, "%s\n", filename) != EOF) {

      _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() : adding instance for %s",_CLASSNAME,filename));

      /* Create a new object path for this kernel parameter file */
      objectpath = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ref,&status)),
				   _CLASSNAME, &status);
      if (CMIsNullObject(objectpath)) {
         CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Failed to create new object path." );
	 _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
         CMReturn(CMPI_RC_ERR_FAILED);
      }

      /* Create a new instance record for this object path */
      instance = CMNewInstance(_broker, objectpath, &status);
      if (CMIsNullObject(instance)) {
         CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Failed to create new instance." );
	 _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
         CMReturn(CMPI_RC_ERR_FAILED);
      }

      /* Store this kernel parameter's attributes in the newly created instance */
      CMSetProperty(instance, "CSCreationClassName", CSCreationClassName, CMPI_chars);
      CMSetProperty(instance, "CSName", hostname, CMPI_chars);
      CMSetProperty(instance, "CreationClassName", _CLASSNAME, CMPI_chars);
      CMSetProperty(instance, "SettingID", filename, CMPI_chars);

      /* Read in the kernel parameter's value from the associated file */
      if ( (paramfile = fopen(filename,"r")) != NULL) {
         fscanf(paramfile, "%1023c", paramvalue);

	 /* Remove embedded tabs and newlines from the value */
         while ( (charptr = strchr(paramvalue,'\t')) != NULL) { *charptr = ' '; }
         while ( (charptr = strchr(paramvalue,'\n')) != NULL) { *charptr = '\0'; }
	
         CMSetProperty(instance, "Value", paramvalue, CMPI_chars);
         fclose(paramfile);
      } else {
	 _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() WARNING : cannot read kernel parameter value from %s",_CLASSNAME,filename));
      }

      /* Determine if kernel parameter is edittable by checking the user write permission of the file */
      if (stat(filename, &fileinfo) == 0) {
         /* BUG?!? We must zero out the CIMValue union before setting booleans! */
         value.uint64 = 0L;
         value.boolean = (fileinfo.st_mode & S_IWUSR) != 0;
	 CMSetProperty(instance, "Edittable", &value, CMPI_boolean);
      } else {
	 _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() WARNING : cannot determine file permissions of %s",_CLASSNAME,filename));
      }
      
      /* Add the new kernel parameter instance to the list of results */
      CMReturnInstance(results, instance);
   }
   if (tmpfile != NULL) {
      fclose(tmpfile); 
   }
   remove(tmpfilename);

   /* Finished returning all the kernel parameters */
   CMReturnDone(results);
   _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_CLASSNAME));
   CMReturn(CMPI_RC_OK);
}


/*
 * GetInstance
 */
CMPIStatus Linux_NetworkCoreParameterGetInstance(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref, 
		const char ** properties) 
{
   CMPIObjectPath * objectpath; /* New object path to store the kernel parameter */
   CMPIInstance * instance;     /* New instacne to store the kernel parameter */
   CMPIValue value;             /* Generic CIM value to store edittable flag */
   CMPIStatus status = {CMPI_RC_OK, NULL};      /* Return status of CIM operations */
   CMPIData data;               /* CMPI structure which holds the object name; i.e. filename */

   char * filename;             /* Name of the target kernel parameter file */
   char * hostname = NULL;      /* Name of this machine */
   FILE * paramfile = NULL;     /* file handle for the kernel parameter file */
   char paramvalue[1024] = "";  /* string buffer to read in contents of kernel parameter file */
   struct stat fileinfo;        /* stat() structure to read in file permission bits */
   char * charptr = NULL;       /* string pointer to walk thru paramvalue */

   _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_CLASSNAME));

   /* Need this machine's hostname to set CSName attributes */
   hostname = get_system_name();

   /* Extract the target kernel parameter filename from the ref object path */
   data = CMGetKey(ref, "SettingID", &status);
   if (data.value.string == NULL) {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Cannot read kernel parameter SettingID value." );
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }
   filename = CMGetCharPtr(data.value.string);  

   _OSBASE_TRACE(1,("--- %s CMPI GetInstance() : adding instance for %s",_CLASSNAME,filename));

   /* Create a new object path for this kernel parameter file */
   objectpath = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ref,&status)),
		   _CLASSNAME, &status);
   if (CMIsNullObject(objectpath)) {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Failed to create new object path." );
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }

   /* Create a new instance record for this object path */
   instance = CMNewInstance(_broker, objectpath, &status);
   if (CMIsNullObject(instance)) {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Failed to create new instance." );
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }

   /* Store this kernel parameter's attributes in the newly created instance */
   CMSetProperty(instance, "CSCreationClassName", CSCreationClassName, CMPI_chars);
   CMSetProperty(instance, "CSName", hostname, CMPI_chars);
   CMSetProperty(instance, "CreationClassName", _CLASSNAME, CMPI_chars);
   CMSetProperty(instance, "SettingID", filename, CMPI_chars);

   /* Read in the kernel parameter's value from the associated file */
   if ( (paramfile = fopen(filename,"r")) != NULL) {
      fscanf(paramfile, "%1023c", paramvalue);

      /* Remove embedded tabs and newlines from the value */
      while ( (charptr = strchr(paramvalue,'\t')) != NULL) { *charptr = ' '; }
      while ( (charptr = strchr(paramvalue,'\n')) != NULL) { *charptr = '\0'; }

      CMSetProperty(instance, "Value", paramvalue, CMPI_chars);
      fclose(paramfile);
   } else {
     _OSBASE_TRACE(1,("--- %s CMPI GetInstance() WARNING : cannot read kernel parameter value from %s",_CLASSNAME,filename));
   }

   /* Determine if kernel parameter is edittable by checking the user write permission of the file */
   if (stat(filename, &fileinfo) == 0) {
      /* BUG?!? We must zero out the CIMValue union before setting booleans! */
      value.uint64 = 0L;
      value.boolean = (fileinfo.st_mode & S_IWUSR) != 0;
      CMSetProperty(instance, "Edittable", &value, CMPI_boolean);
   } else {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() WARNING : cannot determine file permissions of %s",_CLASSNAME,filename));
   }

   /* Add the new kernel parameter instance to the list of results */
   CMReturnInstance(results, instance);

   /* Finished returning all the results */
   CMReturnDone(results);
   _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_CLASSNAME));
   CMReturn(CMPI_RC_OK);
}


/*
 * CreateInstance
 */
CMPIStatus Linux_NetworkCoreParameterCreateInstance(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * objectpath, 
		const CMPIInstance * instance) 
{
   _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_CLASSNAME));
   /* We cannot create new kernel parameters */
   _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_CLASSNAME));
   CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}


/*
 * SetInstance
 */
CMPIStatus Linux_NetworkCoreParameterSetInstance(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref,
		const CMPIInstance * instance, 
		const char **properties) 
{
   CMPIStatus status = {CMPI_RC_OK, NULL};      /* Return status of CIM operations */
   CMPIData data;               /* CMPI structure which holds the object name; i.e. filename */

   char * filename;             /* Name of the target kernel parameter file */
   FILE * paramfile = NULL;     /* file handle for the kernel parameter file */
   char * paramvalue;           /* string containing the new kernel paramter value */
   char newvalue[1024] = "";    /* string buffer to re-read the new contents of the kernel parameter file */ 
   char * charptr1 = NULL;       /* string pointer to walk thru newvalue */
   char * charptr2 = NULL;       /* string pointer to walk thru paramvalue */
   char * token1;
   char * token2;
   
   _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_CLASSNAME));

   /* Extract the target kernel parameter filename from the instance record */
/*   data = CMGetProperty(instance, "SettingID", &status); */
   data = CMGetKey(ref, "SettingID", &status);
   if (data.value.string == NULL) {
      fprintf(stderr, "--- ERROR: Cannot read kernel parameter SettingID\n");
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Cannot read kernel parameter SettingID value." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }
   filename = CMGetCharPtr(data.value.string);


   _OSBASE_TRACE(1,("--- %s CMPI SetInstance() : saving new kernel parameter value in %s",_CLASSNAME,filename));

   /* First check if the kernel parameter is edittable */
   data = CMGetProperty(instance, "Edittable", &status);
   if (status.msg != NULL) {
     //      fprintf(stderr, "--- ERROR: Cannot read kernel parameter Edittable value\n");
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Cannot read kernel parameter Edittable value." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }
   if (! data.value.boolean) {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "This kernel parameter is not edittable." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }

   /* Get the kernel parameter instance's new value */
   data = CMGetProperty(instance, "Value", &status);
   if (status.msg != NULL) {
     //      fprintf(stderr, "--- ERROR: Cannot read kernel parameter Value\n");
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Cannot read kernel parameter Value." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }
   paramvalue = CMGetCharPtr(data.value.string);

   /* Write the new value into the kernel parameter file */
   if ( (paramfile = fopen(filename,"w")) != NULL) {
      fprintf(paramfile, "%s\n", paramvalue);
      fclose(paramfile);
   } else {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "Cannot write to kernel parameter file." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }

   /* Read the new value back from the kernel parameter file to confirm the changes */
   if ( (paramfile = fopen(filename,"r")) != NULL) {
      fscanf(paramfile, "%1023c", newvalue);
      fclose(paramfile);

      /* Compare the original and new value(s), token by token */

      /* Get the first pair of tokens from the new and original values respectivley */
      token1 = strtok_r(newvalue, " \t\n", &charptr1);
      token2 = strtok_r(paramvalue, " \t\n", &charptr2);
      do {
         /* Make sure the two token strings are identical */
         if (strcmp(token1, token2) != 0) {
            CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "New value of kernel parameter does not match the intended value." );
	    _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
            CMReturn(CMPI_RC_ERR_FAILED);
	    break;
         }

	 /* Get the next pair of tokens, if any */
	 token1 = strtok_r(NULL, " \t\n", &charptr1);
	 token2 = strtok_r(NULL, " \t\n", &charptr2);
      } while (token1 != NULL && token2 != NULL);
   } 

   /* Check that both values had the same number of tokens */
   if (token1 == NULL && token2 == NULL) {
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_CLASSNAME));
      CMReturn(CMPI_RC_OK);
   }
   else {
      CMSetStatusWithChars( _broker, &status, CMPI_RC_ERR_FAILED, "New value of kernel parameter does not match the intended value." );
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_CLASSNAME,CMGetCharPtr(status.msg)));
      CMReturn(CMPI_RC_ERR_FAILED);
   }
}


/*
 * DeleteInstance
 */
CMPIStatus Linux_NetworkCoreParameterDeleteInstance(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref) 
{
   _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_CLASSNAME));
   /* We cannot delete kernel parameters */
   _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_CLASSNAME));
   CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}


/*
 * ExecQuery
 */
CMPIStatus Linux_NetworkCoreParameterExecQuery(
		CMPIInstanceMI * mi, 
		const CMPIContext * context, 
		const CMPIResult * results, 
		const CMPIObjectPath * ref, 
		const char * language, 
		const char * query) 
{
   _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_CLASSNAME));
   /* We cannot execute queries against kernel parameters */
   _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_CLASSNAME));
   CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

/* ---------------------------------------------------------------------------*/

/*
 * Factory methods
 */

CMInstanceMIStub( Linux_NetworkCoreParameter,
		  Linux_NetworkCoreParameter,
		  _broker,
		  CMNoHook);

