/*
 * Receiver for data of wireless weather sensors with RX868 and Raspberry Pi.
 *
 * Implementation of decoder module.
 */
#include <stdio.h>
#include <time.h>
#include <list>
#include <vector>

#include <wiringPi.h>

#include "Decoder.h"


Decoder::Decoder(uint8_t minLen, uint8_t maxLen) :
	m_minLen(minLen),	m_maxLen(maxLen) {

	m_decoderState = WAIT;
	m_syncCount = 0;
}


bool Decoder::pulse(uint8_t len, uint8_t lo) {

	bool hasNewValue = false;
	uint8_t val = bitval(len, lo);

	switch(m_decoderState) {
		case WAIT:
			if (0 == val) { // first sync pulse
				m_syncCount = 1;
				m_decoderState = SYNC;
			} else {
				// nothing to do
			}
			break;
		case SYNC:
			if (0 == val) { // another sync pulse
				m_syncCount++;
			} else if (1 == val && m_syncCount == 9) { //TODO: clarify magic numbers
				// got the start bit
				m_syncCount = 0;
				m_decoderState = DATA;
				m_data.clear();
				//printf("\n\n-->going to data state ...\n");
				//printf("sync: val %i / len %i / lo %i \n", val, len, lo); fflush(stdout);
			} else {
				m_syncCount = 0;
				m_decoderState = WAIT;
			}
			break;
		case DATA:
			if (2 == val) { //invalid pulse
				// end of frame?
				//printf("HA: val %i / len %i / lo %i \n", val, len, lo); fflush(stdout);
				hasNewValue = decode();
				m_data.clear();
				m_decoderState = WAIT;
			} else {
				//printf("DATA: val %i / len %i / lo %i \n", val, len, lo); fflush(stdout);
				m_data.push_back(val);
			}
			break;
		default:
			m_data.clear();
			m_decoderState = WAIT;
			break;
	}

	return hasNewValue;
}


uint8_t Decoder::bitval(uint8_t len, uint8_t lo) {

	if (len >= m_minLen && len <= m_maxLen) {
		if (lo < len/2) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2; // no valid bit
	}
}


uint8_t Decoder::popbits(uint8_t num) {

	uint8_t val = 0;
	if (m_data.size() < num) {
		printf("not enough bits left ...\n");
		return 0;
	}
	for (uint8_t idx = 0; idx < num; ++idx) {
		val += m_data.front() << idx;
		m_data.pop_front();
	}

	return val;
}


bool Decoder::expectEoB() {
	// check end of byte (1)
	if (popbits(1) != 1) {
		printf("end of byte is not 1\n");
		return false;
	}

	return true;
}


bool Decoder::decode() {
#ifdef DEBUG
	printf("\n-------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("decode ");
	uint8_t nbit = 1;
	for (std::list<int>::const_iterator it = data.begin(); it != data.end(); ++it) {
		printf("%d", *it);
		if(nbit == 8){
			printf(" ");
		} else if (nbit == 9) {
			printf(" ");
			nbit=0;
		}
		nbit++;
	}
	printf("\n");
#endif

	uint8_t sum = 0;
	uint8_t byteCount = 8;

	std::vector<uint8_t> dec;
	for (uint8_t idx = 0; idx < byteCount; ++idx) {
		uint8_t byte = popbits(8);
		if (!expectEoB()) return false;
		dec.push_back(byte);
		sum += byte;
	}

	// sum
	uint8_t sumRead = popbits(8);
	sum += 5; // TODO: magic number
	sum &= 0xFF;
	if (sumRead != sum) {
		printf("Sum read is %i but computed is %i\n", sumRead, sum);
		return false;
	}

	// compute values
	piLock(1);
	m_decoderOutput.sensorType = dec.at(0);
	m_decoderOutput.address = dec[1];
	m_decoderOutput.temperature = (dec.at(2) | dec.at(3)<<8) / 100.f - 50.; //fast shit for negative numbers
	m_decoderOutput.humidity = (dec.at(4) | dec.at(5)<<8) / 100.f;
	m_decoderOutput.voltage = (dec.at(6) | dec.at(7)<<8) / 100.f;
	piUnlock(1);

	return true;
}

decoderOutput_t Decoder::getDecoderOutput() const {

	return m_decoderOutput;
}

/* 
 * default implementation to pruint8_tDecoderOutput to stdout
 */
void printDecoderOutput(const decoderOutput_t &val) {
	// print current time
	char dateTimeBuf[50];

	time_t t = time(0);
	struct tm *tmp = localtime(&t);
	if (strftime(dateTimeBuf, sizeof(dateTimeBuf), "%x %X", tmp)) {
		printf("time: %s\n", dateTimeBuf);
	}

	printf("sensor type: %i\n", val.sensorType);
	printf("address: %d\n", val.address);
	printf("temperature: %.2f\n", val.temperature);
	printf("humidity: %.2f\n", val.humidity);
	printf("voltage: %.2f\n", val.voltage);
	printf("\n");

}

void printTime() {
	// print current time
	char dateTimeBuf[50];

	time_t t = time(0);
	struct tm *tmp = localtime(&t);
	if (strftime(dateTimeBuf, sizeof(dateTimeBuf), "%x %X", tmp)) {
		printf("time: %s\n", dateTimeBuf);
	}

}
