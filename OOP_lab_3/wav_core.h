#pragma once

#include <vector>
#include <string>

#include "wav_header.h"


// *********************************************************************
// * Error handling
// *********************************************************************

// Possible errors
class WavGeneralErrors
{
public:
	WavGeneralErrors(const std::string& msg)
	{
		message = msg;
	}
	std::string get()
	{
		return message;
	}

private:
	std::string message;
};

// Possible header's errors
class WavHeadersErrors
{
public:
	WavHeadersErrors(const std::string& msg)
	{
		message = msg;
	}
	std::string get()
	{
		return message;
	}

private:
	std::string message;
};



class Wav
{
public:
	Wav();
	~Wav();

// ************************************************************************
// * Functions for working with WAV file
// ************************************************************************


	// Reads inFileName and puts header's data to 'header_ptr' address.
	// Also checks header validity
	void readHeader(char*);

	// Prints header's data from 'header_ptr' address.
	void printHeaderInfo();

	// Reads inFileName and puts PCM data (raw sound data) to 'channels_data'.
	// Also checks header validity
	void read16BitData();


	// Creates a new WAV file 'outFileName', using 'sample_rate' and PCM data from 'channels_data'.
	void makeWavFile(char*);

	// ************************************************************************
	// * Functions for working with sound PCM data - Digital Signal Processing
	// ************************************************************************

	// Makes mono PCM data from stereo 'source'.
	void convertToMono();

	// Adds a reverberation (it's like an echo).
	void reverbate(double delay_seconds, float decay);


private:
	// Fills header with zeroes.
	void setHeaderNull();

	// Checks header validity.
	void checkHeader(size_t file_size_bytes);

	// Fills header information, using input parameters. This function calls prefillHeader() itself.
	void fillHeader(int chan_count, int bits_per_sample, int sample_rate, int samples_count_per_chan);

	// Fills 'header_ptr' with default values.
	void prefillHeader();

	headerStruct header;

	char* inFileName;

	char* outFileName;

	std::vector<std::vector<short>> sourceChannelsData;

	std::vector<std::vector<short>> editedChannelsData;
};
