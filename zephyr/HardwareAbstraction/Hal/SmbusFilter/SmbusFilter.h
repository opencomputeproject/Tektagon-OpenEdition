//***********************************************************************
//*                                                                     *
//*                  Copyright (c) 1985-2022, AMI.                      *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************
/**@file
 * This file contains the Smbus Filter functions
 */

#ifndef SMBUSFILTER_H_
#define SMBUSFILTER_H_

#include <stdio.h>

/**
 * SmbusFilter Interface
 */
struct SmbusFilterInterface {
	/**
	 * Function to SmbusFilter Enable.
	 *
	 * @param  SmbusFilter Interface
	 *
	 * @return 0 
	 */
	int (*SmbusFilterEnable  ) ();
	
	/**
	 * Function to SmbusFilter Disable.
	 *
	 * @param  SmbusFilter Interface
	 *
	 * @return 0 
	 */
	int (*SmbusFilterDisable  ) ();
};

int SmbusFilterInit (struct SmbusFilterInterface *SmbusFilter);

/**
 * Error codes that can be generated by a Smbus FILTER.
 */
enum {
	SMBUSFILTER_INVALID_ARGUMENT = 0x00		/**< Input parameter is null or not valid. */
};
#endif /* SMBUSFILTER_H_ */
