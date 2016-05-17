/*
 * Receiver for data of wireless weather sensors with RX868 and Raspberry Pi.
 *
 * Definition of decoder module.
 */

#include <stdio.h>
#include <list>
#include <stdint.h>

//#define DEBUG

struct decoderOutput_t {
		uint8_t sensorType;
		uint8_t address;
		float temperature;
		float humidity;
		float voltage;
};

/*
 * The decoder.
 */
class Decoder {

	public:
		Decoder(uint8_t minLen = 5, uint8_t maxLen = 14);
		bool pulse(uint8_t len, uint8_t lo);
		decoderOutput_t getDecoderOutput() const;

	private:
		uint8_t bitval(uint8_t len, uint8_t lo);
		uint8_t popbits(uint8_t num);
		bool expectEoB();
		bool decode();

		enum decoderState_t {WAIT, SYNC, DATA, INVALID} m_decoderState;
		decoderOutput_t m_decoderOutput;

		uint8_t m_syncCount;
		const uint8_t m_minLen;
		const uint8_t m_maxLen;
		std::list<uint8_t> m_data;
};

/*
 * Utility function to print DecoderOuput to standard output.
 */
extern void printDecoderOutput(const decoderOutput_t &val);
extern void printTime();
