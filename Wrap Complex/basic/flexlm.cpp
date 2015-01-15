/*
// Copyright 1996-98. Raindrop Geomagic, Inc.
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
*/

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <basic/flexlm.h>
#include <basic/license.h>

//#define __NO_FLEXLM__

#define LICENSE_HEADER \
"# License file name:		geowatch.dat\n\
#\n\
# Any questions regarding this file should be directed to:\n\
#			license@geomagic.com\n\
#\n"


#ifndef __NO_FLEXLM__
LM_CODE(lm_code, ENCRYPTION_SEED1, ENCRYPTION_SEED2, 
        VENDOR_KEY1, VENDOR_KEY2, VENDOR_KEY3, VENDOR_KEY4, VENDOR_KEY5);

static char vendor_info[] = "Shape";
static char issuer[] = "RGI";


// conversion routines
static int fromKey (HostIDKey key)
{
  switch (key)
  {
  case EthernetID:
    return HOSTID_ETHER;
  case DiscID:
    return HOSTID_DISK_SERIAL_NUM;
    break;
  case FlexRainbowID:
    return HOSTID_FLEXID1_KEY;
    break;
  case FlexDallasID:
    return HOSTID_FLEXID2_KEY;
  default:
    break;
  }
  return HOSTID_DEMO;
}

static HostIDKey toKey (int type)
{
  switch(type)
  {
  case HOSTID_ETHER:
    return EthernetID;
  case HOSTID_DISK_SERIAL_NUM:
    return DiscID;
  case HOSTID_FLEXID1_KEY:
    return FlexRainbowID;
  case HOSTID_FLEXID2_KEY:
    return FlexDallasID;
  }
  return DemoID;
}
#endif

struct config FlexLMObject::config_;
struct hostid FlexLMObject::id_;


FlexLMObject::
FlexLMObject (void)
: job_(NULL),
  hostid_(NULL),
  licensePath_(NULL)
{
}

static int validServer=1;

static int
exitSignal(void)
{

	validServer=0;

	return(0);
}

static int
reconnectSignal(void)
{

	validServer=0;

	return(0);
}

static int
reconnectDoneSignal(void)
{

	validServer=1;

	return(0);
}

Bool FlexLMObject::
init (void)
{
#ifdef __NO_FLEXLM__
	return TRUE;
#else
	if (job_) {
		return TRUE;
	}

	int status = lc_init(NULL, VENDOR_NAME, &lm_code, &job_);
	if (!status) {
		status=lc_set_attr(job_,LM_A_CHECK_INTERVAL,(LM_A_VAL_TYPE)30);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_RETRY_CHECKOUT,(LM_A_VAL_TYPE)1);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_RETRY_COUNT,(LM_A_VAL_TYPE)-1);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_RETRY_INTERVAL,(LM_A_VAL_TYPE)30);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_USER_EXITCALL,(LM_A_VAL_TYPE)exitSignal);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_USER_RECONNECT,(LM_A_VAL_TYPE)reconnectSignal);
	}
	if (!status) {
		status=lc_set_attr(job_,LM_A_USER_RECONNECT_DONE,(LM_A_VAL_TYPE)reconnectDoneSignal);
	}

	initConfig_();

	return !status;
#endif
}

FlexLMObject::
~FlexLMObject (void)
{
#ifndef __NO_FLEXLM__
  if (job_) 
  {
    lc_checkin(job_, LM_CI_ALL_FEATURES, 0);
    lc_free_job(job_);
  }
  delete [] licensePath_;
  delete [] hostid_;
#endif
}

//------------------------------------------------------------------------------------------

Bool FlexLMObject::
setLicensePath (char * licensePath)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!job_)
    return FALSE;
  delete [] licensePath_;
  licensePath_ = new char[strlen(licensePath) + 1];
  //strcpy(licensePath_, licensePath);
  strcpy_s(licensePath_, strlen(licensePath) + 1, licensePath);
  int status = lc_set_attr(job_, LM_A_LICENSE_DEFAULT, (LM_A_VAL_TYPE) licensePath_);
  return !status;
#endif
}

const char * FlexLMObject::
licensePath (void)
{
#ifdef __NO_FLEXLM__
  return "FlexLM disabled";
#else
  return licensePath_;
#endif
}


//----------------------------------------------------------------------------------------

Bool FlexLMObject::
setVersion (const char * version)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  //strcpy(config_.version, version);
  strcpy_s(config_.version, strlen(version) + 1, version);
  return TRUE;
#endif
}

char * FlexLMObject::
version (void) const
{
#ifdef __NO_FLEXLM__
  return "FlexLM disabled";
#else
  return config_.version;
#endif
}

//-----------------------------------------------------------------------------------------

char * FlexLMObject::
getHostID (HostIDKey hostIDKey)
{
#ifdef __NO_FLEXLM__
  return "FlexLM disabled";
#else
  delete [] hostid_;
  if (hostIDKey == DemoID)
  {
    hostid_ = new char[5];
    //strcpy(hostid_, "DEMO");
	strcpy_s(hostid_, 5, "DEMO");
    return hostid_;
  }
  int type = fromKey(hostIDKey); 
  hostid_ = new char[MAX_CONFIG_LINE];
  int status = lc_hostid(job_, type, hostid_);
  if (status) hostid_[0] = '\0';
  return hostid_;
#endif
}

Bool FlexLMObject::
verifyServer(void)
{
#ifdef __NO_FLEXLM__
	return TRUE;
#else
	if (!job_) {
		return(FALSE);
	}

	return(validServer);
#endif
}

Bool FlexLMObject::
verify (FlexLMFeatureData& data)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
/* -- Try it the old way -- swp
  return TRUE;
  for(int i=NoID + 1; i<MaxID; i++)
  {
    data.hostIDKey = (HostIDKey) i;
    if(verifyStrict(data))
      return TRUE;
  }
  return FALSE;
*/
	setConfig_(data);
	int status = lc_check_key(job_, &config_, &lm_code);
	return !status;
#endif
}

Bool FlexLMObject::
checkOut (char * featureName)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
/* XXXX: return the actual checkout val now --swp
  return TRUE;
*/
  struct config * conf = lc_get_config(job_, featureName);
  if (!conf) {
    return FALSE;
  }
  int status = lc_checkout(job_, featureName, version(), 1,
           LM_CO_NOWAIT, &lm_code, LM_DUP_NONE);
  return !status;
#endif
}

Bool FlexLMObject::
checkIn (char * featureName)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!job_)
    return FALSE;
  lc_checkin(job_, featureName, 0);
  return TRUE;
#endif
}

char * FlexLMObject::
errorMsg (void)
{
#ifdef __NO_FLEXLM__
  return "FlexLM disabled";
#else
  return lc_errstring (job_);
#endif
}



Bool FlexLMObject::
getFeature (char * featureName, FlexLMFeatureData& data)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!job_)
    return FALSE;
  struct config * conf = lc_get_config(job_, featureName);
  if (!conf)
    return FALSE;
  
  //strcpy(data.feature, conf->feature);
  strcpy_s(data.feature, strlen(conf->feature) + 1, conf->feature);
  //strcpy(data.code, conf->code);
  strcpy_s(data.code, strlen(conf->code) + 1, conf->code);
  if (!conf->idptr && conf->server) {
	data.hostIDKey=FloatID;
  } else if (conf->idptr) {
	data.hostIDKey = toKey(conf->idptr->type);
  } else {
	return FALSE;
  }

  int l = strlen(conf->date);
  if (l < 3)
  {
    //strcpy(data.exp_date, "none");
	strcpy_s(data.exp_date, 5, "none");
  }
  else if ((conf->date[l-1] == '0') && (conf->date[l-2] == '-')) // permanent
  {
    //strcpy(data.exp_date, "permanent");
	strcpy_s(data.exp_date, 10, "permanent");
  }
  else
  {
    //strcpy(data.exp_date, conf->date);
	strcpy_s(data.exp_date, strlen(conf->date) + 1, conf->date);
  }
  
  return TRUE;
#endif
}


Bool FlexLMObject::
getExpDate (char * featureName, char exp_date[])
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!job_)
  {
    //strcpy(exp_date, "none");
	strcpy_s(exp_date, 4, "none");
    return TRUE;
  }
  struct config * conf = lc_get_config(job_, featureName);

  int l = strlen(conf->date);
  if (l < 3)
  {
    //strcpy(exp_date, "none");
	strcpy_s(exp_date, 5, "none");
  }
  else if ((conf->date[l-1] == '0') && (conf->date[l-2] == '-')) // permanent
  {
    //strcpy(exp_date, "permanent");
	strcpy_s(exp_date, 10, "permanent");
  }
  else
  {
    //strcpy(exp_date, conf->date);
	strcpy_s(exp_date, strlen(conf->date), conf->date);
  }
  return TRUE;
#endif
}

Bool FlexLMObject::
convertToString (FlexLMFeatureData& data, char buf[])
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!job_)
    return FALSE;
  char * return_str, * errors;
  getHostID(data.hostIDKey);
  ostringstream oss;
  oss << "FEATURE " << data.feature << " geowatch " << version() << " " << data.exp_date << " uncounted " << data.code << " HOSTID=" << hostid_;
  oss << " vendor_info=" << vendor_info << " ISSUER=" << issuer;
  buf = (char*) oss.str().c_str();
  //sprintf(buf, "FEATURE %s geowatch %s %s uncounted %s HOSTID=%s vendor_info=%s ISSUER=%s",
  //        data.feature, version(), data.exp_date, data.code, hostid_, vendor_info, issuer);
  int status = lc_convert(job_, buf, &return_str, &errors, LM_CONVERT_TO_READABLE);
  if (return_str)
  {
    //strcpy(buf, return_str);
	strcpy_s(buf, strlen(return_str) + 1, return_str);
  }
  else
  {
    buf[0] = '\0';
  }
  lc_free_mem(job_, return_str);
  lc_free_mem(job_, errors);
  return !status;
#endif
}

// write valid license to file
Bool FlexLMObject::
writeValidLicenseToFile (FlexLMFeatureData * arr, int n, int openType)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!arr)
    return FALSE;
  char feature_str[512];
  FILE * f = rgi_fopen(licensePath_, (openType==LicenseManager::Append?
				  "a":
				  "w"));
  if (!f)
    return FALSE;
  fprintf(f, LICENSE_HEADER);
  Bool res = FALSE;
  for (int i=0; i<n; i++)
  {
    FlexLMFeatureData& data = arr[i];
    if (!strlen(data.feature) || 
        !convertToString(data, feature_str))
      continue;
    fprintf(f, feature_str);
    res = TRUE;
  }

  fclose(f);
  return res;
#endif
}

// lookup for hostids
Bool FlexLMObject::
writeLicenseToFile (FlexLMFeatureData * arr, int n, int openType)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (!arr || !licensePath_)
    return FALSE;
  char feature_str[512];
  FILE * f = rgi_fopen(licensePath_, (openType==LicenseManager::Append?
				  "a":
				  "w"));
  if (!f)
    return FALSE;
  fprintf(f, LICENSE_HEADER);
  Bool res = FALSE;
  for (int i=0; i<n; i++)
  {
    FlexLMFeatureData& data = arr[i];
    if (!strlen(data.feature) || 
        !verify(data) || 
        !convertToString(data, feature_str))
      continue;
    fprintf(f, feature_str);
    res = TRUE;
  }

  fclose(f);
  return res;
#endif
}

// do not adjust HostIDKey
Bool FlexLMObject::
verifyStrict (FlexLMFeatureData& data)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  setConfig_(data);
  int status = lc_check_key(job_, &config_, &lm_code);
  return !status;
#endif
}

void FlexLMObject::initConfig_ (void)
{
#ifndef __NO_FLEXLM__
  BZERO(&config_, struct config, 1);
  //strcpy(config_.daemon, "geowatch");
  strcpy_s(config_.daemon, 9, "geowatch");
  config_.lc_vendor_info = vendor_info;
  config_.lc_issuer = issuer;
  BZERO(&id_, struct hostid, 1);
  id_.type = HOSTID_DEMO;
  config_.idptr = &id_;
#endif
}

void FlexLMObject::
setConfig_(FlexLMFeatureData& data)
{
#ifndef __NO_FLEXLM__
  if(strcmp(data.exp_date, "permanent"))
  {
    //strcpy(config_.date, data.exp_date);
	strcpy_s(config_.date, strlen(data.exp_date) + 1, data.exp_date);
  }
  else
  {
    //strcpy(config_.date, "1-jan-0");
	strcpy_s(config_.date, 8, "1-jan-0");
  }
  //strcpy(config_.feature, data.feature);
  strcpy_s(config_.feature, strlen(data.feature) + 1, data.feature);
  //strcpy(config_.code, data.code);
  strcpy_s(config_.code, strlen(data.code) + 1, data.code);
  int type = fromKey(data.hostIDKey);
  
  if (type == HOSTID_DEMO)
  {
    BZERO(&id_, struct hostid, 1);
    id_.type = HOSTID_DEMO;
  }
  else
  {
    struct hostid * hid = lc_getid_type(job_, type);
    if (hid)
      memcpy(&id_, hid, sizeof(struct hostid));
    else
    {
      BZERO(&id_, struct hostid, 1);
      id_.type = HOSTID_DEMO;
    }
  }
#endif
}

//------------------------------------------------------------------------------------

Bool FlexLMObject::
decodeString (char str[], FlexLMFeatureData& data)
{
#ifdef __NO_FLEXLM__
  return TRUE;
#else
  if (strlen(str) != 16)
    return FALSE;

  //strcpy(data.code, str+4);
  strcpy_s(data.code, strlen(str+4) + 1, str+4);
  char * exp_date = data.exp_date;
  BZERO(exp_date, char, FlexLMFeatureData::EXP_DATE_SIZE+1);
  
  // look up month first
  char * month_ptr = exp_date+3;
  switch(str[2])
  {
  case '1': strcpy_s(month_ptr, 4, "jan"); break;
  case '2': strcpy_s(month_ptr, 4, "feb"); break;
  case '3': strcpy_s(month_ptr, 4, "mar"); break;
  case '4': strcpy_s(month_ptr, 4, "apr"); break;
  case '5': strcpy_s(month_ptr, 4, "may"); break;
  case '6': strcpy_s(month_ptr, 4, "jun"); break;
  case '7': strcpy_s(month_ptr, 4, "jul"); break;
  case '8': strcpy_s(month_ptr, 4, "aug"); break;
  case '9': strcpy_s(month_ptr, 4, "sep"); break;
  case 'A':
  case 'a': strcpy_s(month_ptr, 4, "oct"); break;
  case 'B':
  case 'b': strcpy_s(month_ptr, 4, "nov"); break;
  case 'C':
  case 'c': strcpy_s(month_ptr, 4, "dec"); break;
  default: // permanent
    //strcpy(exp_date, "permanent");
	strcpy_s(exp_date, 10, "permanent");
    return TRUE;
  }
  exp_date[2] = exp_date[6] = '-';
  // date
  exp_date[0] = str[0];
  exp_date[1] = str[1];
  // year
  int year = 1998;
  char * exp_year = exp_date+7;
  if ('0' <= str[3] && str[3] <= '9')
    year += str[3] - '0';
  else if ('A' <= str[3] && str[3] <= 'F')
    year += 10 + str[3] - 'A';
  else if ('a' <= str[3] && str[3] <= 'f')
    year += 10 + str[3] - 'a';
  //sprintf(exp_year, "%d", year);
  sprintf_s(exp_year, 5, "%d", year);
  return TRUE;
#endif
}

