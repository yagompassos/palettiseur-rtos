/*
 * main.h
 *
 *  Created on: 16 ao�t 2018
 *      Author: Laurent
 */

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

// Device header
#include "stm32f0xx.h"
// BSP functions
#include "bsp.h"
#include "factory_io.h"
// FreeRTOS headers
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "stream_buffer.h"
#include "event_groups.h"
// Developped Components
#include "blocker.h"
#include "distributor.h"

#define SUBSCRIPTION_TABLE_SIZE 13
#define SENSOR_TABLE_SIZE 13
#define ONE_SHOT 0
#define PERMANENT 1

typedef struct
{
	uint8_t sub_mode;		 // Permanent (1) or one-shot (0)
	uint8_t semaph_id; 		 // Semaphore ID to use for publication
	uint8_t sensor_id; 		 // Awaited sensor ID
	uint8_t sensor_state;	 // Awaited sensor State
} sensor_sub_msg_t;

typedef struct
{
	uint8_t actuator_id; 	 // Awaited sensor ID
	uint8_t actuator_state;	 // Awaited sensor State
} actuator_cmd_msg_t;

//typedef uint8_t msg_t[sizeof(sensor_sub_msg_t)];


//Define actuators
#define ACT_DISTRIBUTION_CARTONS 	                    1           // (1 << 0)
#define ACT_TAPIS_DISTRIBUTION_CARTONS 	                2           // (1 << 1)
#define ACT_BLOCAGE_ENTREE_PALETTISEUR  	            4           // (1 << 2)
#define ACT_PORTE 	                                    8           // (1 << 3)
#define ACT_POUSSOIR 	                                (1 << 4)    // 16
#define ACT_CLAMP 	                                    32          // (1 << 5)
#define ACT_MONTER_ASCENSEUR 	                        64          // (1 << 6)
//#define skip one bit each 7 sensors or actuators (bit 7 / 128 skipped)
#define ACT_DESCENDRE_ASCENSEUR 	                    256         // (1 << 8)
#define ACT_ASCENSEUR_TO_LIMIT                          512         // (1 << 9)
#define ACT_DISTRIBUTION_PALETTE                        1024        // (1 << 10)
#define ACT_CHARGER_PALETTE		                        2048        // (1 << 11)
#define ACT_TAPIS_CARTON_VERS_PALETTISEUR		        (1 << 12)   // 4096
#define ACT_TOURNER_CARTON		                        8192        // (1 << 13)
#define ACT_DECHARGER_PALETTISEUR		                16384       // (1 << 14)
//#define skip one bit each 7 sensors or actuators (bit 15 / 32768 skipped)
#define ACT_CHARGER_PALETTISEUR                         65536       // (1 << 16)
#define ACT_DECHARGER_PALETTE                           131072      // (1 << 17)
#define ACT_TAPIS_PALETTE_VERS_ASCENSEUR                262144      // (1 << 18)
#define ACT_TAPIS_DISTRIBUTION_PALETTE                  524288      // (1 << 19)
#define ACT_TAPIS_FIN                                   1048576     // (1 << 20)
#define ACT_REMOVER                                     2097152     // (1 << 21)    //(19)


//Define sensors
#define SEN_CARTON_DISTRIBUE 	                        1           //(0)		//0 quand carton
#define SEN_CARTON_ENVOYE 	                            (1 << 1)				//0 quand carton
#define SEN_ENTREE_PALETTISEUR  	                    (1 << 2)				//0 quand carton
#define SEN_PORTE_OUVERTE 	                            (1 << 3)				//1 quand ouvert
#define SEN_LIMITE_POUSSOIR 	                        (1 << 4)				//1 quand a la limite
#define SEN_CLAMPED 	                                (1 << 5)				//1 quand a clamp
#define SEN_ASCENSEUR_ETAGE_RDC 	                    (1 << 6)      //(6)		//1 quand rdc
//#define skip one bit each 7 sensors or actuators
#define SEN_ASCENSEUR_ETAGE_1 	                        (1 << 8)      //(7)		//1 quand ascenseur
#define SEN_ASCENSEUR_ETAGE_2                           (1 << 9)				//1 quand ascenseur
#define SEN_SORTIE_PALETTE                              (1 << 10)				//1 quand palette
#define SEN_LIMITE_PORTE		                        (1 << 11)				//1 quand a la limite
#define SEN_ASCENSEUR_EN_MOUVEMENT		                (1 << 12)				//1 quand mouvement
#define SEN_ENTREE_PALETTE		                        (1 << 13)				//1 quand palette
#define SEN_BUTEE_CARTON		                        (1 << 14)     //(13)	//1 quand carton

#define ID_SEMAPH_DISTRIBUTOR		1
#define ID_SEMAPH_BLOCKER			2
#define ID_SEMAPH_PUSHER			3


#define ACTIVE_HIGH 1
#define IDLE_HIGH 0
#define ACTIVE_LOW 0
#define IDLE_LOW 1


#define	EVENT_SEN_CARTON_DISTRIBUE			( (EventBits_t)( 0x01 <<0) )
#define EVENT_SEN_CARTON_ENVOYE 	    	( (EventBits_t)( 0x01 <<1) )
#define EVENT_SEN_ENTREE_PALETTISEUR  		( (EventBits_t)( 0x01 <<2) )
#define EVENT_SEN_PORTE_OUVERTE 	    	( (EventBits_t)( 0x01 <<3) )
#define EVENT_SEN_LIMITE_POUSSOIR 	    	( (EventBits_t)( 0x01 <<4) )
#define EVENT_SEN_CLAMPED 	            	( (EventBits_t)( 0x01 <<5) )
#define EVENT_SEN_ASCENSEUR_ETAGE_RDC 		( (EventBits_t)( 0x01 <<6) )
#define EVENT_SEN_ASCENSEUR_ETAGE_1 		( (EventBits_t)( 0x01 <<7) )
#define EVENT_SEN_ASCENSEUR_ETAGE_2     	( (EventBits_t)( 0x01 <<8) )
#define EVENT_SEN_SORTIE_PALETTE        	( (EventBits_t)( 0x01 <<9) )
#define EVENT_SEN_LIMITE_PORTE		    	( (EventBits_t)( 0x01 <<10) )
#define EVENT_SEN_ASCENSEUR_EN_MOUVEMENT 	( (EventBits_t)( 0x01 <<11) )
#define EVENT_SEN_ENTREE_PALETTE			( (EventBits_t)( 0x01 <<12) )
#define EVENT_SEN_BUTEE_CARTON		    	( (EventBits_t)( 0x01 <<13) )

/* Global handles */
extern EventGroupHandle_t	sensorsEventGroup;

/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);


#endif /* APP_INC_MAIN_H_ */
