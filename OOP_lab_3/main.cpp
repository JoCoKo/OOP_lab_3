#include <iostream>

#include "wav_core.h"

using namespace std;

int main()
{
    cout << "************** | WavCore | **************" << endl;


    // ################  Tests for WavCore  ################



	try
	{
		Wav wav1, wav2;
		wav1.readHeader("0.wav");
		wav1.printHeaderInfo();
		wav1.read16BitData();
		cout << endl << "********************" << endl;
		wav1.convertToMono();
		wav1.reverbate(0.5, 0.6f);
		wav1.makeWavFile("out.wav");
		wav2.readHeader("out.wav");
		wav2.printHeaderInfo();
	}
	catch (WavGeneralErrors& error)
	{
		cerr << error.get() << endl;
	}
	catch (WavHeadersErrors& error)
	{
		cerr << error.get() << endl;
	}

	getchar();
    return 0;
}
