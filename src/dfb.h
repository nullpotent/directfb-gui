/*
 * dfb.h
 *
 *  Created on: Feb 8, 2012
 *      Author: admin
 */

#ifndef DFB_H_
#define DFB_H_

#include "utils.h"
#include "dfbcontext.h"

void 	(*DFB_UPDATE_FCTR)(float elapsedTime);
void 	(*DFB_EVHANDLER_FCTR)(DFBInputDeviceKeySymbol* sym);
void 	(*DFB_RENDER_FCTR)(void);

int 	dfb_init(int argc, char** argv, unsigned short width, unsigned short height);
void 	dfb_loop();
void 	dfb_destroy();

#endif /* DFB_H_ */
