#include <cstdio>
#include <cstring>

#include "wav_header.h"
#include "wav_core.h"


// TODO: Remove all 'magic' numbers



void Wav::readHeader(char* filename)
{
	inFileName = filename;
    printf( ">>>> readHeader( %s )\n", inFileName );
    setHeaderNull(); // Fill header with zeroes.

	FILE* f;
	fopen_s(&f,inFileName, "rb");
    if ( !f ) 
	{
        throw WavGeneralErrors("IO_ERROR");
    }

    size_t blocks_read = fread(&header, sizeof(headerStruct), 1, f);
    if ( blocks_read != 1 ) 
	{
        // can't read header, because the file is too small.
		throw WavGeneralErrors("BAD_FORMAT");
    }

    fseek( f, 0, SEEK_END ); // seek to the end of the file
    size_t file_size = ftell( f ); // current position is a file size!
    fclose( f );

	checkHeader(file_size);
    
}

void Wav::printHeaderInfo()
{
    printf( "-------------------------\n" );
    printf( " audioFormat   %u\n", header.audioFormat );
    printf( " numChannels   %u\n", header.numChannels );
    printf( " sampleRate    %u\n", header.sampleRate );
    printf( " bitsPerSample %u\n", header.bitsPerSample );
    printf( " byteRate      %u\n", header.byteRate );
    printf( " blockAlign    %u\n", header.blockAlign );
    printf( " chunkSize     %u\n", header.chunkSize );
    printf( " subchunk1Size %u\n", header.subchunk1Size );
    printf( " subchunk2Size %u\n", header.subchunk2Size );
    printf( "-------------------------\n" );
}


void Wav::read16BitData()
{
    printf( ">>>> read16BitData( %s )\n", inFileName );
    readHeader(inFileName);

    if ( header.bitsPerSample != 16 ) 
	{
        // Only 16-bit samples is supported.
		throw WavGeneralErrors("UNSUPPORTED_FORMAT");
    }

	FILE* f;
	fopen_s(&f, inFileName, "rb");
    if ( !f ) 
	{
		throw WavGeneralErrors("IO_ERROR");
    }
    fseek( f, 44, SEEK_SET ); // Seek to the begining of PCM data.

    int chan_count = header.numChannels;
    int samples_per_chan = ( header.subchunk2Size / sizeof(short) ) / chan_count;

    // 1. Reading all PCM data from file to a single vector.
    std::vector<short> all_channels;
    all_channels.resize( chan_count * samples_per_chan );
    size_t read_bytes = fread( all_channels.data(), 1, header.subchunk2Size, f );
    if ( read_bytes != header.subchunk2Size ) 
	{
        printf( "read16BitData() read only %zu of %u\n", read_bytes, header.subchunk2Size );
		throw WavGeneralErrors("IO_ERROR");
    }
    fclose( f );


    // 2. Put all channels to its own vector.
    sourceChannelsData.resize( chan_count );
    for ( size_t ch = 0; ch < sourceChannelsData.size(); ch++ ) 
	{
        sourceChannelsData[ ch ].resize( samples_per_chan );
    }

    for ( int ch = 0; ch < chan_count; ch++ ) 
	{
        std::vector<short>& chdata = sourceChannelsData[ ch ];
        for ( size_t i = 0; i < samples_per_chan; i++ ) 
		{
            chdata[ i ] = all_channels[ chan_count * i + ch ];
        }
    }
}

void Wav::checkHeader(size_t file_size_bytes )
{
    // Go to wav_header.h for details

    if ( header.chunkId[0] != 0x52 ||
         header.chunkId[1] != 0x49 ||
         header.chunkId[2] != 0x46 ||
         header.chunkId[3] != 0x46 )
    {
		throw WavHeadersErrors("HEADER_RIFF_ERROR");
    }

    if ( header.chunkSize != file_size_bytes - 8 ) 
	{
		throw WavHeadersErrors("HEADER_FILE_SIZE_ERROR");
    }

    if ( header.format[0] != 0x57 ||
         header.format[1] != 0x41 ||
         header.format[2] != 0x56 ||
         header.format[3] != 0x45 )
    {
		throw WavHeadersErrors("HEADER_WAVE_ERROR");
    }

    if ( header.subchunk1Id[0] != 0x66 ||
         header.subchunk1Id[1] != 0x6d ||
         header.subchunk1Id[2] != 0x74 ||
         header.subchunk1Id[3] != 0x20 )
    {
		throw WavHeadersErrors("HEADER_FMT_ERROR");
    }

    if ( header.audioFormat != 1 ) 
	{
		throw WavHeadersErrors("HEADER_NOT_PCM");
    }

    if ( header.subchunk1Size != 16 ) 
	{
		throw WavHeadersErrors("HEADER_SUBCHUNK1_ERROR");
    }

    if ( header.byteRate != header.sampleRate * header.numChannels * header.bitsPerSample/8 ) 
	{
		throw WavHeadersErrors("HEADER_BYTES_RATE_ERROR");
    }

    if ( header.blockAlign != header.numChannels * header.bitsPerSample/8 ) 
	{
		throw WavHeadersErrors("HEADER_BLOCK_ALIGN_ERROR");
    }

    if ( header.subchunk2Id[0] != 0x64 ||
         header.subchunk2Id[1] != 0x61 ||
         header.subchunk2Id[2] != 0x74 ||
         header.subchunk2Id[3] != 0x61 )
    {
		throw WavHeadersErrors("HEADER_FMT_ERROR");
    }

    if ( header.subchunk2Size != file_size_bytes - 44 )
    {
		throw WavHeadersErrors("HEADER_SUBCHUNK2_SIZE_ERROR");
    }
}

void Wav::prefillHeader()
{
    // Go to wav_header.h for details

    header.chunkId[0] = 0x52;
    header.chunkId[1] = 0x49;
    header.chunkId[2] = 0x46;
    header.chunkId[3] = 0x46;

    header.format[0] = 0x57;
    header.format[1] = 0x41;
    header.format[2] = 0x56;
    header.format[3] = 0x45;

    header.subchunk1Id[0] = 0x66;
    header.subchunk1Id[1] = 0x6d;
    header.subchunk1Id[2] = 0x74;
    header.subchunk1Id[3] = 0x20;

    header.subchunk2Id[0] = 0x64;
    header.subchunk2Id[1] = 0x61;
    header.subchunk2Id[2] = 0x74;
    header.subchunk2Id[3] = 0x61;

    header.audioFormat   = 1;
    header.subchunk1Size = 16;
    header.bitsPerSample = 16;

}

void Wav::fillHeader(int chan_count, int bits_per_sample, int sample_rate, int samples_count_per_chan)
{
    if ( bits_per_sample != 16 ) 
	{
		throw WavGeneralErrors("UNSUPPORTED_FORMAT");
    }

    if ( chan_count < 1 ) 
	{
		throw WavGeneralErrors("BAD_PARAMS");
    }
    prefillHeader();

    int file_size_bytes = 44 + chan_count * (bits_per_sample/8) * samples_count_per_chan;

    header.sampleRate    = sample_rate;
    header.numChannels   = chan_count;
    header.bitsPerSample = 16;

    header.chunkSize     = file_size_bytes - 8;
    header.subchunk2Size = file_size_bytes - 44;

    header.byteRate      = header.sampleRate * header.numChannels * header.bitsPerSample/8;
    header.blockAlign    = header.numChannels * header.bitsPerSample/8;
}

void Wav::makeWavFile(char* filename)
{
	outFileName = filename;
    printf( ">>>> makeWavFile( %s )\n", outFileName );

    int chan_count = (int)editedChannelsData.size();

    if ( chan_count < 1 ) 
	{
		throw WavGeneralErrors("BAD_PARAMS");
    }

    int samples_count_per_chan = (int)editedChannelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chan_count; ch++ ) 
	{
        if ( editedChannelsData[ ch ].size() != (size_t) samples_count_per_chan ) 
		{
			throw WavGeneralErrors("BAD_PARAMS");
        }
    }

	fillHeader(chan_count, 16, header.sampleRate, samples_count_per_chan );

    std::vector<short> all_channels;
    all_channels.resize( chan_count * samples_count_per_chan );

    for ( int ch = 0; ch < chan_count; ch++ ) 
	{
        const std::vector<short>& chdata = editedChannelsData[ ch ];
        for ( size_t i = 0; i < samples_count_per_chan; i++ ) 
		{
            all_channels[ chan_count * i + ch ] = chdata[ i ];
        }
    }

	FILE* f;
	fopen_s(&f, outFileName, "wb");
    fwrite( &header, sizeof(headerStruct), 1, f );
    fwrite( all_channels.data(), sizeof(short), all_channels.size(), f );
    if ( !f ) 
	{
		throw WavGeneralErrors("IO_ERROR");
    }

    fclose( f );
}

void Wav::setHeaderNull()
{
    memset( &header, 0, sizeof(headerStruct) );
}

void Wav::convertToMono()
{
    int chan_count = (int)sourceChannelsData.size();

    if ( chan_count != 2 ) 
	{
		throw WavGeneralErrors("BAD_PARAMS");
    }

    int samples_count_per_chan = (int)sourceChannelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chan_count; ch++ ) 
	{
        if ( sourceChannelsData[ ch ].size() != (size_t) samples_count_per_chan ) 
		{
			throw WavGeneralErrors("BAD_PARAMS");
        }
    }

    editedChannelsData.resize( 1 );
    std::vector<short>& mono = editedChannelsData[ 0 ];
    mono.resize( samples_count_per_chan );

    // Mono channel is an arithmetic mean of all (two) channels.
    for ( size_t i = 0; i < samples_count_per_chan; i++ ) 
	{
        mono[ i ] = ( sourceChannelsData[0][i] + sourceChannelsData[1][i] ) / 2;
    }
}

void Wav::reverbate(double delay_seconds, float decay)
{
    int chan_count = (int)editedChannelsData.size();

    if ( chan_count < 1 ) 
	{
		throw WavGeneralErrors("BAD_PARAMS");
    }

    int samples_count_per_chan = (int)editedChannelsData[0].size();

    // Verify that all channels have the same number of samples.
    for ( size_t ch = 0; ch < chan_count; ch++ ) 
	{
        if ( editedChannelsData[ ch ].size() != (size_t) samples_count_per_chan ) 
		{
			throw WavGeneralErrors("BAD_PARAMS");
        }
    }

    int delay_samples = (int)(delay_seconds * header.sampleRate);


    for ( size_t ch = 0; ch < chan_count; ch++ ) 
	{
        std::vector<float> tmp;
        tmp.resize(editedChannelsData[ch].size());

        // Convert signal from short to float
        for ( size_t i = 0; i < samples_count_per_chan; i++ ) 
		{
            tmp[ i ] = editedChannelsData[ ch ][ i ];
        }

        // Add a reverb
        for ( size_t i = 0; i < samples_count_per_chan - delay_samples; i++ ) 
		{
            tmp[ i + delay_samples ] += decay * tmp[ i ];
        }

        // Find maximum signal's magnitude
        float max_magnitude = 0.0f;
        for ( size_t i = 0; i < samples_count_per_chan - delay_samples; i++ ) 
		{
            if ( abs(tmp[ i ]) > max_magnitude ) 
			{
                max_magnitude = abs(tmp[ i ]);
            }
        }

        // Signed short can keep values from -32768 to +32767,
        // After reverb, usually there are values large 32000.
        // So we must scale all values back to [ -32768 ... 32768 ]
        float norm_coef = 30000.0f / max_magnitude;
        printf( "max_magnitude = %.1f, coef = %.3f\n", max_magnitude, norm_coef );

        // Scale back and transform floats to shorts.
        for ( size_t i = 0; i < samples_count_per_chan; i++ ) 
		{
            editedChannelsData[ ch ][ i ] = (short)(norm_coef * tmp[ i ]);
        }
    }
}

Wav::Wav()
{
}

Wav::~Wav()
{
}