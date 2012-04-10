/*
 * dfbcontext.h
 *
 *  Created on: Feb 8, 2012
 *      Author: admin
 */

#ifndef DFBCONTEXT_H_
#define DFBCONTEXT_H_

#include <stdlib.h>
#include <directfb/directfb.h>

IDirectFB* 					DFBC_INTERFACE_;
IDirectFBEventBuffer* 		DFBC_EVBUFF_;
IDirectFBSurface* 			DFBC_SURFACE_;
DFBInputDeviceKeyState 		DFBC_ESCAPECTRL_; //DIKS_UP
IDirectFBDisplayLayer* 		DFBC_LAYER_;
DFBSurfaceDescription 		DFBC_SURFACE_DESC_;

#endif /* DFBCONTEXT_H_ */
