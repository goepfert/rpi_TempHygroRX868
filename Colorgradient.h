/*
 * colorgradient.h
 *
 *  Created on: Jan 18, 2016
 *      Author: goepfert
 */

#ifndef colorgradient_H_
#define colorgradient_H_

#include <stdint.h>
#include <string>

#define NENTRIES 90
#define MIN_TEMP -100 //scaled by 10
#define STEP_TEMP 5   //scaled by 10

std::string getColorgradient(int16_t temp);
int16_t getArrayIdx(int16_t temp);

/*--------------------------------------------------------------------------------*/

/*
 * NENTRIES number of entries 90
 * NENTRIES idx 89
 */
std::string LUT_color[] {
	"0500ff",
	"0400ff",
	"0300ff",
	"0200ff",
	"0100ff",
	"0000ff",
	"0002ff",
	"0012ff",
	"0022ff",
	"0032ff",
	"0044ff",
	"0054ff",
	"0064ff",
	"0074ff",
	"0084ff",
	"0094ff",
	"00a4ff",
	"00b4ff",
	"00c4ff",
	"00d4ff",
	"00e4ff",
	"00fff4",
	"00ffd0",
	"00ffa8",
	"00ff83",
	"00ff5c",
	"00ff36",
	"00ff10",
	"17ff00",
	"3eff00",
	"65ff00",
	"8aff00",
	"b0ff00",
	"d7ff00",
	"fdff00",
	"FFfa00",
	"FFf000",
	"FFe600",
	"FFdc00",
	"FFd200",
	"FFc800",
	"FFbe00",
	"FFb400",
	"FFaa00",
	"FFa000",
	"FF9600",
	"FF8c00",
	"FF8200",
	"FF7800",
	"FF6e00",
	"FF6400",
	"FF5a00",
	"FF5000",
	"FF4600",
	"FF3c00",
	"FF3200",
	"FF2800",
	"FF1e00",
	"FF1400",
	"FF0a00",
	"FF0000",
	"FF0010",
	"FF0020",
	"FF0030",
	"FF0040",
	"FF0050",
	"FF0060",
	"FF0070",
	"FF0080",
	"FF0090",
	"FF00A0",
	"FF00B0",
	"FF00C0",
	"FF00D0",
	"FF00E0",
	"FF00F0",
	"FF01F0",
	"FF02F0",
	"FF03F0",
	"FF04F0",
	"FF05F0",
	"FF06F0",
	"FF07F0",
	"FF08F0",
	"FF09F0",
	"FF0AF0",
	"FF0BF0",
	"FF0CF0",
	"FF0DF0",
	"FF0EF0"
};

std::string getColorgradient(int16_t temp) {

	return LUT_color[ getArrayIdx(temp) ];
}

/*
 * minimum = MIN_TEMP
 * maximum = MAX*STEP_TEMP + MIN_TEMP
 */
int16_t getArrayIdx(int16_t temp) {

	int16_t idx = -1;
	int16_t temp_shift = (int16_t)(temp*10)-MIN_TEMP;

	for(int16_t loopIdx=0; loopIdx<NENTRIES; ++loopIdx){
		if(temp_shift <= STEP_TEMP*loopIdx) {
			idx = loopIdx;
			break;
		}
	}

	if(idx == -1) idx = NENTRIES-1;

	return idx;
}


#endif /* colorgradient_H_ */
