#ifndef dos_packet_h
#define dos_packet_h

#include "types.h"
#include "ports.h"

typedef struct DosPacket
{
	struct Message	dp_Message; // Std Message Header for Exec Messages
	INT32			dp_Action; 	// The dp_Action Field is for the Action being done (Open, Close, Write, Read,....)
	INT32           dp_Res1; // The Results are being presented in dp_Res1 (as the Return Result) and dp_Res2 for the 2nd Part
	INT32           dp_Res2;
	INT32           dp_Arg1; // dp_Arg1... Fits the Argument for the Action. 
	INT32			dp_Arg2;
	INT32			dp_Arg3;
	INT32			dp_Arg4;
	INT32			dp_Arg5;
	INT32			dp_Arg6;
	INT32			dp_Arg7;
	INT32			dp_Arg8;
}DosPacket;


#endif
