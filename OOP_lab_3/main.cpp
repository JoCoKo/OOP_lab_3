#include <iostream>

#include "wav_core.h"

using namespace std;

int main()
{
    cout << "************** | WavCore | **************" << endl;

	try
	{
		Wav wav1;
		wav1.createFromFile("0.wav");
		cout << wav1.GetDescription() << endl;
		wav1.convertToMono();
		wav1.reverbate(0.5, 0.6f);
		wav1.saveToFile("out.wav");

		Wav wav2("out.wav");
		cout << wav2.GetDescription() << endl;
		int sampleRate = wav2.getSampleRate();
		int chanCount = wav2.getChanCount();
		bool isStereo = wav2.isStereo();
	}
	catch (WavGeneralErrors& error)
	{
		cerr << error.get() << endl;
	};

	getchar();
    return 0;
}
