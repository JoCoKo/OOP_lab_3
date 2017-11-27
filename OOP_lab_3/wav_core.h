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



class Wav
{
public:
	Wav();
	Wav(char*);
	~Wav();

// ************************************************************************
// * Functions for working with WAV file
// ************************************************************************

	void createFromFile(char*);


	// Prints header's data from 'header_ptr' address.
	std::string GetDescription();


	// Creates a new WAV file 'outFileName', using 'sample_rate' and PCM data from 'channels_data'.
	void saveToFile(char*);

	// ************************************************************************
	// * Functions for working with sound PCM data - Digital Signal Processing
	// ************************************************************************

	// Makes mono PCM data from stereo 'source'.
	void convertToMono();

	// Adds a reverberation (it's like an echo).
	void reverbate(double delay_seconds, float decay);

	int getSampleRate();
	int getChanCount();
	bool isStereo();

private:

	bool inited = false;

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

	std::vector<std::vector<short>> sourceChannelsData;

	std::vector<std::vector<short>> editedChannelsData;
};
