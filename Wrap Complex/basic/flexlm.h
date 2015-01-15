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

#ifndef __RGI_FLEXLM_H__
#define __RGI_FLEXLM_H__

/* WINNT is required to quiet down visual c++ when compiling in 95 mode on nt */
#ifdef _WINDOWS
#ifndef WINNT
#define WINNT 
#endif
#endif

#include <basic/lmpolicy.h>
#include <basic/lm_attr.h>
#include <basic/basic.h>
#ifdef _WINDOWS
#include <io.h>
#endif

#include <basic/license.h>
#include <basic/data.h>


class FlexLMObject
{
public:
  FlexLMObject (void);
  ~FlexLMObject (void);
  Bool init (void);
  Bool setLicensePath (char * licensePath);
  const char * licensePath (void);
  Bool setVersion (const char * version);
  char * version (void) const;
  char * getHostID (HostIDKey hostIDKey);
  Bool checkOut (char * featureName);
  Bool checkIn (char * featureName);
  char * errorMsg (void);
  Bool getFeature (char * featureName, FlexLMFeatureData& data);
  Bool convertToString (FlexLMFeatureData& data, char buf[]);
  Bool getExpDate (char * featureName, char exp_date[]);
  Bool verifyStrict (FlexLMFeatureData& data);
  Bool verify (FlexLMFeatureData& data);
  Bool verifyServer(void);
  Bool writeLicenseToFile (FlexLMFeatureData arr[], int n, int openType);
  Bool writeValidLicenseToFile (FlexLMFeatureData arr[], int n, int openType);
  Bool decodeString(char str[], FlexLMFeatureData& data);
private:
  void initConfig_ (void);
  void setConfig_ (FlexLMFeatureData& data);
  LM_HANDLE * job_;
  char * hostid_;
  char * licensePath_;
  static struct config config_;
  static struct hostid id_;
};

#endif // __FLEXLM_H__
