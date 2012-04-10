/*
 * dfb.c
 *
 *  Created on: Feb 8, 2012
 *      Author: admin
 */

#include "dfb.h"

pthread_t            processingThread;

pthread_mutex_t      processingThread_mutex;
pthread_mutex_t      keyevent_mutex;

static int DFB_SCREENW  = 0;
static int DFB_SCREENH = 0;
static int  KEY_PRESSED_ = 0;
static int  WAKE_UP_ = 0;

/**************************************************************************
 *
 **************************************************************************/
void process_events()
{
	pthread_mutex_init(&keyevent_mutex, NULL);
	int quit = 0;

	if(DFBC_INTERFACE_->CreateInputEventBuffer(DFBC_INTERFACE_, DICAPS_ALL /*DICAPS_KEYS*/, DFB_TRUE, &DFBC_EVBUFF_ ))
	{
		printf("Create DFBInputEventBuffer failed\n");
		dfb_destroy();
		return;
	}

	while(quit == 0)
	{
		DFBInputEvent evt;
		DFBC_EVBUFF_->WaitForEvent(DFBC_EVBUFF_);
		while(DFBC_EVBUFF_->GetEvent(DFBC_EVBUFF_, DFB_EVENT(&evt)) == DFB_OK)
		{
			pthread_mutex_lock(&keyevent_mutex);
			if(evt.type == DIET_KEYRELEASE)
				KEY_PRESSED_ = 0;   // prevent getting events while key is pressed

			if( ((evt.type == DIET_KEYPRESS) && !KEY_PRESSED_) ||
			    ((evt.type == DIET_KEYPRESS) && WAKE_UP_))
			{
				KEY_PRESSED_ = 1;

				if(WAKE_UP_)
					WAKE_UP_ = 0;

				switch (evt.key_symbol)
				{
					case DIKS_ESCAPE:
					{
						quit = 1;
						DFBC_ESCAPECTRL_ = DIKS_DOWN;
						break;
					}
					default:
					{
						DFB_EVHANDLER_FCTR(&evt.key_symbol);
						break;
					}
				}
			}
			pthread_mutex_unlock(&keyevent_mutex);
		}
	}
	pthread_mutex_destroy(&keyevent_mutex);
}

/**************************************************************************
 *
 **************************************************************************/
void* run_event_processing(void *arg)
{
	process_events();
	pthread_mutex_destroy(&processingThread_mutex);
	return NULL;
}

/*****************************************************************************
 * Creates event processing thread that handles the keyboard and mouse input *
 *****************************************************************************/
void create_eventproc_thread()
{
	/* Init mutex */
	pthread_mutex_init(&processingThread_mutex, NULL);
	/* Create thread */
	pthread_create(&processingThread, NULL , run_event_processing, NULL);
}


/**************************************************************************
 *
 **************************************************************************/
int dfb_init(int argc, char** argv, unsigned short width, unsigned short height) {

	DFBCHECK(DirectFBInit (&argc, &argv));
	DFBCHECK(DirectFBCreate (&DFBC_INTERFACE_));
	DFBCHECK(DFBC_INTERFACE_->SetCooperativeLevel (DFBC_INTERFACE_, DFSCL_FULLSCREEN));

	DFBC_ESCAPECTRL_ = DIKS_UP;
	DFBC_SURFACE_DESC_.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
	DFBC_SURFACE_DESC_.height = height;
	DFBC_SURFACE_DESC_.width = width;
	DFBC_SURFACE_DESC_.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;

	DFBCHECK(DFBC_INTERFACE_->CreateSurface(DFBC_INTERFACE_, &DFBC_SURFACE_DESC_, &DFBC_SURFACE_));
	DFBCHECK(DFBC_SURFACE_->GetSize (DFBC_SURFACE_, &DFB_SCREENW, &DFB_SCREENH));

	//Hide the cursor
	DFBC_INTERFACE_->GetDisplayLayer(DFBC_INTERFACE_, 0x00, &DFBC_LAYER_);
	DFBC_LAYER_->SetCooperativeLevel(DFBC_LAYER_, DLSCL_ADMINISTRATIVE);
	DFBC_LAYER_->SetCursorOpacity(DFBC_LAYER_, 0);

	/*
	 * TO-DO
	 * Return code
	 */
	return 0;
}

/**************************************************************************
 *
 **************************************************************************/
void dfb_loop()
{
    create_eventproc_thread();
	while(DFBC_ESCAPECTRL_ == DIKS_UP)
	{
		DFBCHECK(DFBC_SURFACE_->SetColor(DFBC_SURFACE_,0x00,0x00,0x00,0xFF));
		DFBCHECK(DFBC_SURFACE_->FillRectangle(DFBC_SURFACE_, 0, 0, DFB_SCREENW, DFB_SCREENH));
			DFB_RENDER_FCTR();
		DFBCHECK(DFBC_SURFACE_->Flip(DFBC_SURFACE_, NULL, 0));
	}
    pthread_join(processingThread, NULL);
}

void dfb_destroy()
{
	assert(DFBC_SURFACE_ != NULL);
	assert(DFBC_INTERFACE_ != NULL);
	assert(DFBC_LAYER_ != NULL);
	assert(DFBC_EVBUFF_ != NULL);

	DFBC_EVBUFF_->Release(DFBC_EVBUFF_);
	DFBC_INTERFACE_->Release(DFBC_INTERFACE_);
	DFBC_SURFACE_->Release(DFBC_SURFACE_);
	DFBC_LAYER_->Release(DFBC_LAYER_);
}
