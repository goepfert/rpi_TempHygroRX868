/*
 * Receiver for data of wireless weather sensors with RX868 and Raspberry Pi.
 *
 * Main program.
 */
#include <wiringPi.h>
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "Colorgradient.h"
#include "Decoder.h"


static volatile int keepRunning = 1;
static decoderOutput_t out;

sem_t sHasNewOutput;

void sigIntHandler(int dummy);
PI_THREAD (decoderThread);

const char* database[] = {"data_dev0.rrd"};
const char* path = "/home/pi/Projects/TempHygroRX868/";
std::ofstream outfile;

int main() {

	wiringPiSetup();
	sem_init(&sHasNewOutput,0,0);

	pinMode(3, OUTPUT);
	digitalWrite(3, 1); // enable rx
	pinMode(2, INPUT);
	pullUpDnControl(2, PUD_DOWN);

	signal(SIGINT, sigIntHandler);

	piThreadCreate(decoderThread);

	char str[256];

	while (keepRunning ) {

		while(sem_wait(&sHasNewOutput) && EINTR==errno){;}

		piLock(1);
		printDecoderOutput(out);
		outfile.open("/home/pi/Projects/TempHygroRX868/data/dev0.dat", std::ofstream::out | std::ofstream::trunc);
		outfile << "temp:" << out.temperature << " hum:" << out.humidity << " vol:" << out.voltage << std::endl;
		outfile.close();
		sprintf(str, "%s/data/scripts/update_database %s %f %f %f", path, database[out.address], out.temperature, out.humidity, out.voltage);
		system(str);
		sprintf(str, "%s/pages/rpi_SearchAndReplace %s %s %s %.2f", path, "/home/pi/Projects/TempHygroRX868/pages/test1_org.php", "/home/pi/Projects/TempHygroRX868/pages/test2.php", "YYY", out.temperature);
		system(str);
		sprintf(str, "%s/pages/rpi_SearchAndReplace %s %s %s %s", path, "/home/pi/Projects/TempHygroRX868/pages/test2.php", "/home/pi/Projects/TempHygroRX868/pages/test1.php", "XXX", getColorgradient(out.temperature).data() );
		system(str);
		piUnlock(1);
	} // end while loop

	printf("clean up and exit\n");
	digitalWrite(3, 0); // disable rx
}


/*
 * Signal handler to catch Ctrl-C to terminate the program safely.
 */
void sigIntHandler(int dummy) {
	keepRunning = 0;
} 

/*
 * Thread to read the output of the receiver module 
 * and to decode it using Decoder.
 */
PI_THREAD (decoderThread) {
	piHiPri(50);

	// init decoder for a sample rate of 1/200µs
	Decoder decoder(4,12);
	int x;
	uint8_t len = 0;
	uint8_t lo = 0;
	uint8_t px = 0;

	//printTime();
	//uint16_t counter = 0;

	while (keepRunning) {

		x = digitalRead(2);

		len++;
		if (0 == x) {
			lo++;
		}

		if (x != px && 0 != x) {
			// slope low->high
			if (decoder.pulse(len, lo)) {
				piLock(1);
				out = decoder.getDecoderOutput();
				piUnlock(1);
				sem_trywait(&sHasNewOutput);
				sem_post(&sHasNewOutput);
			}
			len = 1;
			lo = 0;
		}
		px = x;

		delayMicroseconds(200);
	} // end while loop

}
