
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

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <assert.h>

#include "basic.h"
#include "flexlm.h"
#include "license.h"
#include "versions.h"

class LicenseTable
{
public:
	LicenseManager::FeatureList id;
	LicenseManager::PolicyList  policy;
	char name[50];
};

#define DefFeature(name,policy,wrapName) \
	{LicenseManager::name,LicenseManager::policy,wrapName}
// The concept here is to have "" as the name...nothing else matters -swp
#define LastFeature() \
	{LicenseManager::LAST_FEATURE,LicenseManager::EVAL_POLICY,""}

/*
Before, all but the first two items were PURCHASE_POLICY types.

But we don't give anything away for free now. You have to have
a valid license to use it.
*/
static LicenseTable licenseTable [] = 
{
    DefFeature(SHAPE_FEATURE, PURCHASE_POLICY, "studioShape"),
    DefFeature(WRAP_FEATURE, PURCHASE_POLICY, "studioWrap"),
/*
    DefFeature(EXPORT_FEATURE, MODULE_POLICY, "studioExport"),
    DefFeature(IMPORT_FEATURE, MODULE_POLICY, "studioImport"),
    DefFeature(DECIMATE_FEATURE, MODULE_POLICY, "studioDecimate"),
    DefFeature(THICKEN_FEATURE, MODULE_POLICY, "studioShell"),
    DefFeature(DECIMATOR_FEATURE, MODULE_POLICY, "decimator"),
    DefFeature(MAINTENANCE_FEATURE, MODULE_POLICY, "studioMaintenance"),
*/
    LastFeature()
};
static int changed=0;

extern "C"
{
  const char * productFLEXlmVersion (void);
  int getAbsLicenseFilePath (char * buf, int bufLength);
};

//-----------------------------------------------------------------------------

LicenseManager::PolicyRetVal 
LicenseManager::checkPolicy_ (LicenseManager::PolicyList policy)
{

	switch (policy) {
		case MODULE_POLICY:
		case EVAL_POLICY:
		case PURCHASE_POLICY:
			return LookFLEXlm;
		case WRAPBASIC_POLICY:
			// allow users to preview feature in demo mode
			return (purchased())? LookFLEXlm : Yes;
		default:
			break;
	}
	return No;
}

LicenseManager::LicenseFeature::
LicenseFeature (void)
: id(0), 
  checkedOut(FALSE),
  name(NULL), 
  error(NULL)
{
}

LicenseManager::LicenseFeature::
~LicenseFeature (void)
{
  if (name) free(name);
  if (error) free(error);
}

LicenseManager::
LicenseManager (void)
{
    char feature[50];
    int i;

    featureList_ = NULL;
    purchased_ = 0;
    errorMsg_ = NULL;
    flexlm_ = NULL;

    if (strlen(PRODUCT_BETA_VERSION) && !changed) {
	changed=1;
	for (i=0; licenseTable[i].name[0]; i++) {
	    //sprintf(feature,"%sBeta",licenseTable[i].name);
		sprintf_s(feature, 50, "%sBeta",licenseTable[i].name);
	    //strcpy(licenseTable[i].name,feature);
		strcpy_s(licenseTable[i].name, 50, feature);
	}
    }
}

LicenseManager::
~LicenseManager (void)
{
	delete []featureList_;
	delete flexlm_;
	if (initPath_) {
		free(initPath_);
		initPath_=(char *)0;
	}
}


void LicenseManager::
init (const char *path)
{
	delete [] featureList_;
	delete flexlm_;
	featureList_ = new LicenseFeature[LAST_FEATURE];
	int i;
	for (i=0; i<LAST_FEATURE; i++) {
		LicenseFeature& feature = featureList_[i];
		feature.id = i;
		feature.checkedOut=FALSE;
	}
	purchased_ = 0;
	errorMsg_ = NULL;
	flexlm_ = NULL;

	if (path) {
		initPath_= _strdup(path);
	} else {
		initPath_=(char *)0;
	}

	char lpath[512];
	flexlm_ = new FlexLMObject;
	flexlm_->init();
	if (!initPath_ && getAbsLicenseFilePath (lpath, 512)) {
		flexlm_->setLicensePath(lpath);
	} else if (initPath_) {
		flexlm_->setLicensePath(initPath_);
	}
	flexlm_->setVersion(productFLEXlmVersion());

	// find first entry in table with purchase policy
	FeatureList purchasedId = LAST_FEATURE;

	// find purchased policy first
	for (i=0; i<LAST_FEATURE; i++) {
		int tableId = featureList_[i].id;
		if (licenseTable[tableId].policy == PURCHASE_POLICY) {
			purchasedId = (FeatureList) i;
			featureList_[i].name = _strdup(licenseTable[i].name);
			checkOut_ (purchasedId);
			purchased_ = checkOut (purchasedId);
			if (purchased_) { // already purchased
				break;
			}
		}
	}

	// find all other features
	for (i=0; i<LAST_FEATURE; i++) {
		if (purchasedId == (FeatureList) i) { // purchasing defined already
			continue;
		}
		featureList_[i].name = _strdup(licenseTable[i].name);
		checkOut_ ((FeatureList) i);
	}
}

//-------------------------------------------------------------------------

Bool LicenseManager::
purchased (void)
{
  return purchased_;
}

char * LicenseManager::
getErrorMsg (void)
{
  return errorMsg_;
}

//-------------------------------------------------------------------------

Bool LicenseManager::
checkOut_ (FeatureList featureId)
{
  LicenseFeature& feature = featureList_[featureId];
  if (feature.checkedOut)
	return TRUE;
  PolicyRetVal res = checkPolicy_ (licenseTable[featureId].policy);
  switch (res)
  {
  case Yes:
	return feature.checkedOut = TRUE;
  case No:
	if(feature.error) free(feature.error);
	char buf[256];
	 //sprintf(buf, "The policy does not allow to use feature %s.", feature.name);
	 sprintf_s(buf, 256, "The policy does not allow to use feature %s.", feature.name);
	 feature.error = _strdup(buf);
	return feature.checkedOut = FALSE;
  case LookFLEXlm:
    if(!flexlm_)
    return FALSE;
	  break;
  }

  feature.checkedOut = flexlm_->checkOut(feature.name);

  if (!feature.checkedOut)
  {
    char *ptr,*msg;
    if(feature.error) free(feature.error);
    ptr=flexlm_->errorMsg();
    msg=(char *)calloc(((ptr?strlen(ptr):0)+strlen(feature.name)+50),
		       sizeof(char));
    //sprintf(msg, "Error in feature %s:\n %s",
	//    feature.name,
	//    (ptr?ptr:"Unknown"));
	sprintf_s(msg, (ptr?strlen(ptr):0)+strlen(feature.name)+50,
		    "Error in feature %s:\n %s", feature.name,
	        (ptr?ptr:"Unknown"));
    feature.error=msg;
  }
  return feature.checkedOut;
}

Bool LicenseManager::
checkOut (FeatureList featureId, Bool force)
{
  LicenseFeature& feature = featureList_[featureId];
  errorMsg_ = feature.error;
  if (force) {
    return flexlm_->checkOut(feature.name);
  } else {
    return feature.checkedOut;
  }
}

char * LicenseManager::
getHostID (HostIDKey key)
{
  return flexlm_? flexlm_->getHostID(key) : NULL;
}

Bool LicenseManager::
getFeatureName (LicenseManager::FeatureList featureId, char name[])
{
  LicenseFeature& feature = featureList_[featureId];
  //strcpy(name, feature.name);
  strcpy_s (name, strlen(feature.name) + 1, feature.name);
  return TRUE;
}

Bool LicenseManager::
getFeatureExpDate (LicenseManager::FeatureList featureId, char exp_date[])
{
  LicenseFeature& feature = featureList_[featureId];

  if (!flexlm_)
    return FALSE;

  if(!feature.checkedOut)
  {
    //strcpy(exp_date, "none");
	strcpy_s(exp_date, 5, "none");
    return TRUE;
  }

  return flexlm_->getExpDate(feature.name, exp_date);
}


Bool LicenseManager::
getLicenseExpDays (LicenseManager::FeatureList featureId, int& days)
{
	time_t curtime, exp_time;
	char buf[20];
	char *year_ptr = NULL;
	char *month_ptr = NULL;
	int i, day, month, year;
	struct tm tm;
	double diff;
	char *months[]={"jan",
					"feb",
					"mar",
					"apr",
					"may",
					"jun",
					"jul",
					"aug",
					"sep",
					"oct",
					"nov",
					"dec",
				};

	if (getFeatureExpDate(featureId, buf))
	{
		// parse buf string to extract date;
		// buf should be of the form 1-jul-1999;
		// it could also be "none" or "permanent"

		// if buf = "none", that means there is no license
		if (!strcmp(buf,"none"))
		{
			days = 10000;
#ifdef __NO_FLEXLM__
			return TRUE;
#else
			return FALSE;
#endif
		}
		// if buf = "permanent" give days an arbitrarily large value
		if (!strcmp(buf, "permanent"))
		{
			days = 10000;
			return TRUE;
		}

		// get year from string
		year_ptr = strrchr(buf, '-');
		*year_ptr = '\0';
		year_ptr++;

		year = atoi(year_ptr);

		// get month from string
		month_ptr = strrchr(buf, '-');
		*month_ptr = '\0';
		month_ptr++;

        for (i=0; i<12; i++)
		{
			if (!strcasecmp(month_ptr, months[i]))
			{
				break;
			}
        }

		// if invalid month, abort
        assert(i!=12);
        month = i;

		// now get day from string
		day = atoi(buf);

		// store expiration date in tm structure
		tm.tm_year = year - 1900;
		tm.tm_mon = month;
		tm.tm_mday = day;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;

		// get expiration time since the epoch in seconds
		exp_time = mktime(&tm);

		// get the current time
		curtime = time(NULL);

		// get time diff between current and expiration times in seconds
		diff = difftime(exp_time, curtime);

		// convert these seconds to days
		days = (int) (diff/(3600*24));

		// license valid only if days > 0
		return TRUE;
	}

	// license not valid
	return FALSE;
}

Bool LicenseManager::
getFeature (LicenseManager::FeatureList featureId, FlexLMFeatureData& data)
{
  LicenseFeature& feature = featureList_[featureId];
  if (!feature.checkedOut)
    return FALSE;
  return flexlm_? flexlm_->getFeature(feature.name, data) : FALSE;
}

Bool LicenseManager::
verify (FlexLMFeatureData& data)
{
  return flexlm_? flexlm_->verify(data) : FALSE;
}

Bool LicenseManager::
verifyServer ()
{
  return flexlm_? flexlm_->verifyServer() : FALSE;
}

Bool LicenseManager::
setLicensePath(char lpath[])
{

    return(flexlm_?flexlm_->setLicensePath(lpath):FALSE);
}

const char *LicenseManager::
getLicensePath(void)
{
    return(flexlm_?flexlm_->licensePath():NULL);
}

Bool LicenseManager::
writeLicenseToFile(FlexLMFeatureData arr[], int n, int openType)
{
  return flexlm_? flexlm_->writeLicenseToFile(arr, n, openType) : FALSE;
}

Bool LicenseManager::
decode(char str[], FlexLMFeatureData& data, FeatureList& list_id)
{
  if (!flexlm_ || 
      !flexlm_->decodeString(str, data)) // fill in code, and expiration date. Key should be entered by caller.
    return FALSE;
  for (int i = 0; i<LAST_FEATURE; i++)
  {
    //strcpy(data.feature, featureList_[i].name);
	strcpy_s(data.feature, strlen(featureList_[i].name) + 1, featureList_[i].name);
    if(flexlm_->verify(data))
    {
      list_id = (FeatureList) i;
      return TRUE;
    }
  }
  return FALSE;
}

Bool LicenseManager::
decodeFeature(char str[], FlexLMFeatureData& data, FeatureList list_id)
{
  if (!flexlm_ || 
      !flexlm_->decodeString(str, data)) // fill in code, and expiration date. Key should be entered by caller.
    return FALSE;

    //strcpy(data.feature, featureList_[list_id].name);
	strcpy_s(data.feature, strlen(featureList_[list_id].name) + 1, featureList_[list_id].name);
    if(flexlm_->verify(data))
    {
      return TRUE;
    }

  return FALSE;
}
