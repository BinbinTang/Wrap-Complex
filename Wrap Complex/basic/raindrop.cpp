/*
//
// Copyright 1996-97. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.; 
// the contents of this file may not be disclosed to third parties, copied 
// or duplicated in any form, in whole or in part, without the prior written 
// permission of Raindrop Geomagic, Inc.  
//
// This copyright notices may not be removed from this file at any time.
// 
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
// 
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
// 
// 
*/

#include <basic/basic.h>
#include "versions.h"

/* set of functions, returning status of application */


static Bool isBatchMode_ = FALSE;
static char companyName_[] = "Raindrop Geomagic";
 
#ifdef WIN32
static char *WrapName_ = "Studio";
static char *WrapCapName_ = "Studio";
static char *WrapCmdName_ = "StudioCmd";
#else
static char *WrapName_ = "studio";
static char *WrapCapName_ = "Studio";
static char *WrapCmdName_ = "studio";
#endif

/* Defined in versions.h to facillitate unix lib versioning */
static char *productVersion_ = PRODUCT_MAJOR_VERSION;
static char *minorVersion_ = PRODUCT_MINOR_VERSION;
static char *betaVersion_ = PRODUCT_BETA_VERSION;
static char *FLEXlmVersion_ = PRODUCT_FLEXLM_VERSION;

/*---------------------------------------------------------------------------------------*/

const char * companyName (void)
{
  return companyName_;
}

/*---------------------------------------------------------------------------------------*/

const char * productVersion (void)
{
  return productVersion_;
}

/* In place so decimator can change */
void setProductVersion(char *ver)
{
    productVersion_= _strdup(ver);
}

/*---------------------------------------------------------------------------------------*/

const char * productName (void)
{
  return WrapName_;
}

const char * productCapName (void)
{
    return WrapCapName_;
}

/* In place so decimator can change */
void setProductName(char *name)
{
    WrapName_= _strdup(name);
    WrapCapName_= _strdup(name);
    *WrapCapName_= toupper(*WrapCapName_);
#ifdef _WIN32
	WrapCmdName_= (char *) calloc(strlen(name)+4,sizeof(char));
    //sprintf(WrapCmdName_,"%sCmd",WrapName_);
	sprintf_s(WrapCmdName_, strlen(name) + 4, "%sCmd",WrapName_);
#else
    WrapCmdName_=strdup(name);
#endif
}

/*---------------------------------------------------------------------------------------*/

const char * minorVersion (void)
{
  return minorVersion_;
}

/* In place so decimator can change */
void setMinorVersion(char *ver)
{
    minorVersion_= _strdup(ver);
}

/*---------------------------------------------------------------------------------------*/

const char * betaVersion (void)
{
  return betaVersion_;
}

/* In place so decimator can change */
void setBetaVersion(char *ver)
{
    betaVersion_= _strdup(ver);
}

/*---------------------------------------------------------------------------------------*/

const char * productFLEXlmVersion (void)
{
	return FLEXlmVersion_;
}

/* In place so decimator can change */
void setFLEXlmVersion(char *ver)
{
    FLEXlmVersion_= _strdup(ver);
}

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

Bool isBatchMode (void)
{
  return isBatchMode_;
}

void setBatchMode (Bool mode)
{
  isBatchMode_ = mode;
}

/*---------------------------------------------------------------------------------------*/
#ifdef _WINDOWS

HKEY getAppRegistryUserKey (int useVersion)
{
  HKEY hSoftware = NULL;
  HKEY hCompany = NULL;
  HKEY hProduct = NULL;
  HKEY hVersion = NULL;
  long res;
  DWORD dw;

  res = RegOpenKeyEx (HKEY_CURRENT_USER,
         (LPCTSTR) "Software", 0, KEY_WRITE|KEY_READ, &hSoftware);
  //res = RegOpenKeyEx (hSoftware,
  //       (LPCTSTR) companyName(), 0, KEY_WRITE|KEY_READ, &hCompany);
  //res = RegOpenKeyEx (hCompany,
  //       (LPCTSTR) productName(), 0, KEY_WRITE|KEY_READ, &hProduct);
  //res = RegCreateKeyEx(HKEY_CURRENT_USER, "Software", 0, REG_NONE,
  //	REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
  //	&hSoftware, &dw);
  res = RegCreateKeyEx(hSoftware, (LPCTSTR) companyName(), 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hCompany, &dw);
  res = RegCreateKeyEx(hCompany, (LPCTSTR) productCapName(), 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hProduct, &dw);
  if (useVersion)
  {
    //res = RegOpenKeyEx (hProduct,
    //     (LPCTSTR) productVersion (), 0, KEY_WRITE|KEY_READ, &hVersion);
    res = RegCreateKeyEx(hProduct, (LPCTSTR) productVersion(), 0, REG_NONE,
   		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
   		&hVersion, &dw);
  }
  RegCloseKey(hSoftware);
  RegCloseKey(hCompany);
  if (useVersion)
	RegCloseKey(hProduct);

  return useVersion? hVersion : hProduct;
}

HKEY getAppSectionUserKey (LPCTSTR lpszSection, int useVersion)
{ 
  DWORD dw;
  HKEY hSectionKey = NULL;
  HKEY hAppKey = NULL;
  if (lpszSection == NULL)
    return NULL;
  
	hAppKey = getAppRegistryUserKey(useVersion);
	if (hAppKey == NULL)
		return NULL;

	
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}





HKEY getAppRegistryInstallKey (int useVersion)
{
  HKEY hSoftware = NULL;
  HKEY hCompany = NULL;
  HKEY hProduct = NULL;
  HKEY hVersion = NULL;
  long res;
  res = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
         (LPCTSTR) "Software", 0, KEY_WRITE|KEY_READ, &hSoftware);
  res = RegOpenKeyEx (hSoftware,
         (LPCTSTR) companyName(), 0, KEY_WRITE|KEY_READ, &hCompany);
  res = RegOpenKeyEx (hCompany,
         (LPCTSTR) productName(), 0, KEY_WRITE|KEY_READ, &hProduct);

  if (useVersion)
  {
    res = RegOpenKeyEx (hProduct,
         (LPCTSTR) productVersion (), 0, KEY_WRITE|KEY_READ, &hVersion);
  }
  
  RegCloseKey(hSoftware);
  RegCloseKey(hCompany);
  if (useVersion)
	RegCloseKey(hProduct);

  return useVersion? hVersion : hProduct;
}

HKEY getAppSectionInstallKey (LPCTSTR lpszSection, int useVersion)
{ 
  DWORD dw;
  HKEY hSectionKey = NULL;
  HKEY hAppKey = NULL;
  if (lpszSection == NULL)
    return NULL;
  
	hAppKey = getAppRegistryInstallKey(useVersion);
	if (hAppKey == NULL)
		return NULL;

	
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}


int getAbsAppPath (char * buf, int bufLength)
{
  DWORD keyType;
  long res;
  HKEY hKey = getAppRegistryInstallKey(1);
  DWORD bufLen = bufLength;

  res = RegQueryValueEx (hKey,
                   "AppPath", NULL,
                   &keyType,
                   (unsigned char *) buf,
				   &bufLen);
                   //&bufLength);
  RegCloseKey(hKey);
  return !res;
}

int getAbsAppName (char * buf, int bufLength)
{
  DWORD keyType;
  long res;
  HKEY hKey = getAppRegistryInstallKey(1);
  DWORD bufLen = bufLength;

  res = RegQueryValueEx (hKey,
                   "Application", NULL,
                   &keyType,
                   (unsigned char *) buf,
                   &bufLen);
                   //&bufLength);
  RegCloseKey(hKey);
  return !res;
}


int getAbsLicenseFilePath (char * buf, int bufLength)
{
  DWORD keyType;
  long res;
  HKEY hKey = getAppRegistryInstallKey(1);
  DWORD bufLen = bufLength;

  res = RegQueryValueEx (hKey,
                   "License file", NULL,
                   &keyType,
                   (unsigned char *) buf,
                   &bufLen);
                   //&bufLength);
  RegCloseKey(hKey);
  return !res;
}

int setAbsLicenseFilePath (char * name, int nameLength)
{
  long res;
  HKEY hKey = getAppRegistryInstallKey(1);
  DWORD nameLen = nameLength;
                
  res = RegSetValueEx (hKey,
                   "License file", 0,
                    REG_SZ,
                    (BYTE *) name,
					nameLen);
                    //nameLength);
  RegCloseKey(hKey);
  return !res;
}

#else /* UNIX */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>


char *findProgram(char *path, char *program);
char *getPath(char *argv0);
void setArgv0(char *buf);
int canExecute(mode_t mode, gid_t gid, uid_t uid);
char *getRGIDir(void);
char *tryRGIDir(char *fname);
int getAbsAppPath (char * buf, int bufLength);


extern int errno;
static char *basicArgv0;


void setArgv0(char *buf)
{

    basicArgv0=strdup(buf);

    return;
}


char *getPath(char *argv0)
{
    char *ptr,*pname;
    char *epath,*token;
    int found=0;

    if (!argv0 || !(ptr=strrchr(argv0,'/'))) {
		ptr=argv0;
    } else {
		char old = *ptr, *tmp = ptr;
		*ptr='\0';
		if ((ptr=findProgram(argv0,ptr+1))) {
			found=1;
		}
		*tmp = old;
    }

    if (!found) {
	if ((epath=getenv("PATH"))) {
	    epath=strdup(epath);
	    token=strtok(epath,":");
	    pname=ptr;
	    ptr=findProgram(token,pname);
	    while (!ptr) {
		if (!(token=strtok(NULL,":"))) {
		    break;
		}
		if ((ptr=findProgram(token,pname))) {
		    found=1;
		}
	    }
	}
    }

    if (!found) {
	return(NULL);
    }

    return(ptr);
}


/*
 * Returns 1 on success. This is a modified "access" checking routine
 * which finds out if the user can actually execute the file with
 * permission.
 */
int canExecute(mode_t mode, gid_t gid, uid_t uid)
{

    /* check the easy one first (public) */
    if (mode & S_IXOTH) {
	return(1);
    }

    /* Check group */
    if (mode & S_IXGRP &&
	getgid()==gid) {
	return(1);
    }

    /* Check owner */
    if (mode & S_IXUSR &&
	getuid()==uid) {
	return(1);
    }

    /* No execute permissions for you! */
    return(0);
}


char *findProgram(char *path, char *program)
{
    int found=0;
    char *tryit;
    struct stat buf;
    char *newpath=NULL,*ptr;
    char result[MAXPATHLEN];

    tryit=(char *)calloc((FILENAME_MAX+MAXPATHLEN),sizeof(char));

    sprintf(tryit,"%s/%s",path,program);
    if (!lstat(tryit,&buf)) {	/* Catch the sym links */
	if (!S_ISLNK(buf.st_mode) &&
	    S_ISREG(buf.st_mode) &&
	    !S_ISDIR(buf.st_mode) &&
	    canExecute(buf.st_mode,buf.st_gid,buf.st_uid)) {
	    found=1;
	} else if (S_ISLNK(buf.st_mode)) {
	    if (!stat(tryit,&buf)) { /* Get what the link points at */
		if (!S_ISDIR(buf.st_mode) &&
		    S_ISREG(buf.st_mode) &&
		    canExecute(buf.st_mode,buf.st_gid,buf.st_uid)) {
		    found=1;
		}
	    }
	}
    }

    if (found) {
	/* expand the path and nix the program name */
	newpath=strdup(realpath(tryit,result));
	if ((ptr=strrchr(newpath,'/'))) {
	    *ptr='\0';
	}
    }

    free(tryit);

    return(newpath);
}


char *getRGIDir(void)
{
    char *home=NULL;
    struct passwd *pass=NULL;
    char buf[512];

    if (!(home=getenv("HOME"))) { /* fail */
        if (!(pass=getpwuid(getuid()))) { /* fail */
            fprintf(stderr,"Could not obtain home directory from $HOME or /etc/passwd.\n");
	    return(NULL);
	} else { /* succeed on pw ent */
            home=pass->pw_dir;
	}
    }

    sprintf(buf,"%s/.rgi",home);
    /* make the dir with 777 perms which will be ORed with their umask */
    if (mkdir(buf,S_IRWXU|S_IRWXG|S_IRWXO)==(-1)) {
	if (errno!=EEXIST) {
	    perror(buf);
	    return(NULL);
	}
    }

    home=strdup(buf);

    return(home);
}


char *tryRGIDir(char *fname)
{
    static int tried=0;
    char *path=NULL;
    char tryit[512];
    struct stat sbuf;

    /* If we haven't done this before and we can get the RGI dir */
    if (!tried && (path=getRGIDir())) {
	sprintf(tryit,"%s/%s",path,fname);
	/* If the file "fname"does not exist in "path", return NULL */
	if (stat(tryit,&sbuf)==(-1)) {
	    free(path);
	    path=NULL;
	}
	/* Don't do this again if we tried already */
	tried=1;
    }

    return(path);
}


int getAbsLicenseFilePath (char * buf, int bufLength)
{
    static char *path=NULL;
    char *fname="/geowatch.dat",*tmp;

    if (!path) {
		if ((path=getPath(basicArgv0))) {
			tmp=strdup(path);
			free(path);
			path=(char *)calloc(strlen(tmp)+strlen(fname)+1,sizeof(char));
			sprintf(path,"%s%s",tmp,fname);
			free(tmp);
		}
	}

	if (path) {
		/* This is a safe copy which _could_ truncate a unix path,
			although it would have to be extremely long to do so...so
			we won't worry about it. -swp */
		if (strlen(path)>bufLength-1) {
			path[bufLength-1]='\0';
		}

		strcpy(buf,path);

		return(1);
	}

	*buf='\0';

	return(0);
}


int getAbsAppPath (char * buf, int bufLength)
{
	static char *path=NULL;

	if (!path) {
		path=getPath(basicArgv0);
	}

    if (path) {
		/* This is a safe copy which _could_ truncate a unix path,
			although it would have to be extremely long to do so...so
			we won't worry about it. -swp */
		if (strlen(path)>bufLength-1) {
			path[bufLength-1]='\0';
		}

		strcpy(buf,path);

		return(1);
	}

	*buf='\0';

	return(0);
}

#endif
