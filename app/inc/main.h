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
// FreeRTOS headers
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "stream_buffer.h"

//Define actuators
#define ACT_DISTRIBUTION_CARTONS 	                    1           // (1 << 0)
#define ACT_TAPIS_DISTRIBUTION_CARTONS 	                2           // (1 << 1)
#define ACT_BLOCAGE_ENTREE_PALETTISEUR  	            4           // (1 << 2)
#define ACT_PORTE 	                                    8           // (1 << 3)
#define ACT_POUSSOIR 	                                16          // (1 << 4)
#define ACT_CLAMP 	                                    32          // (1 << 5)
#define ACT_MONTER_ASCENSEUR 	                        64          // (1 << 6)
//#define skip one bit each 7 sensors or actuators (bit 7 / 128 skipped)
#define ACT_DESCENDRE_ASCENSEUR 	                    256         // (1 << 8)
#define ACT_ASCENSEUR_TO_LIMIT                          512         // (1 << 9)
#define ACT_DISTRIBUTION_PALETTE                        1024        // (1 << 10)
#define ACT_CHARGER_PALETTE		                        2048        // (1 << 11)
#define ACT_TAPIS_CARTON_VERS_PALETTISEUR		        4096        // (1 << 12)
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

/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);


#endif /* APP_INC_MAIN_H_ */
