/*                         
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
*/ 

#ifndef __RGI_LICENSE_H__
#define __RGI_LICENSE_H__

#include <basic/basic.h>
#include <basic/data.h>

class FlexLMObject;

class LicenseManager
{
public:
	enum FeatureList 
	    {
		    SHAPE_FEATURE = 0,
		    WRAP_FEATURE = 1,
/*
		    EXPORT_FEATURE = 2,
		    IMPORT_FEATURE = 3,
		    DECIMATE_FEATURE = 4,
		    THICKEN_FEATURE = 5,
		    SPLINE_FEATURE = 6,
		    DECIMATOR_FEATURE = 7,
		    MAINTENANCE_FEATURE = 8,
*/
		    LAST_FEATURE = 9
	    };
	enum PolicyList 
	    {
			// always available
			EVAL_POLICY, 
			// only in purchased products
			PURCHASE_POLICY, 
			// available in free eval. copy 
			// for additional money available in purchased product
			WRAPBASIC_POLICY, 
			// we doan give nuthin fer free no moah
			MODULE_POLICY,
			LAST_POLICY
	    };
	enum {
	    Overwrite=0,
	    Append
	};

	LicenseManager (void);
	~LicenseManager (void);
	void init (const char *path=(const char *)0);
	Bool checkOut (FeatureList featureId, Bool force=FALSE);
	Bool purchased (void);
	char * getErrorMsg (void);
    char * getHostID (HostIDKey key);
    Bool getFeatureName(FeatureList featureId, char name[]);
    Bool getFeatureExpDate(FeatureList featureId, char exp_date[]);
    Bool getLicenseExpDays(FeatureList featureId, int& days);
    Bool getFeature(FeatureList featureId, FlexLMFeatureData& data);
    Bool verify (FlexLMFeatureData& data);
	Bool verifyServer (void);
    Bool writeLicenseToFile (FlexLMFeatureData arr[], int n, int openType);
    Bool setLicensePath(char lpath[]);
    Bool decode(char str[], FlexLMFeatureData& data, FeatureList& list_id);
    // Decodes for a specific feature
    Bool decodeFeature(char str[], FlexLMFeatureData& data,
		       FeatureList list_id);
    const char *getLicensePath(void);

	class LicenseFeature
    {
    public:
      LicenseFeature (void);
      ~LicenseFeature (void);
      int id;
      Bool checkedOut;
	  char * name;
	  char * error;
	};
private:
	enum PolicyRetVal 
	     { Yes, 
		   No, 
		   LookFLEXlm 
	     };
    PolicyRetVal checkPolicy_ (PolicyList policy);
	LicenseFeature * featureList_;
	Bool checkOut_ (FeatureList featureId);
	char *errorMsg_,*initPath_;
	int purchased_;
	FlexLMObject * flexlm_;
};


#endif // __LICENSE_H__
