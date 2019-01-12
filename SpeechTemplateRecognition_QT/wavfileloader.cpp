#include "wavfileloader.hpp"



//WAVFileLoader::WAVFileLoader(QUrl outputLocation)
//{
//}


int WAVFileLoader::loadFile()
{
    std::string filePathStr = qUrlToString(
                outputLocation);
    filePathStr = filePathStr.substr(1, filePathStr.length()-1);

    audioFile.load(filePathStr);
    int channel = 0;
    int numSamples = audioFile.getNumSamplesPerChannel();

    for (int i = 0; i < numSamples; i++)
    {
        signal.push_back(audioFile.samples[channel][i]);
    }
    return 0;
}


int WAVFileLoader::loadFile_deprecated()
{
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr), filelength = 0;

    const char* filePath;
    string input;

    std::string filePathStr = qUrlToString(
                outputLocation);
    filePathStr = filePathStr.substr(1, filePathStr.length()-1);

    filePath = filePathStr.c_str();//"one1.wav";

    FILE* wavFile = fopen(filePath, "r");
    if (wavFile == nullptr)
    {
        fprintf(stderr, "Unable to open wave file: %s\n", filePath);
        return -1;
    }

    //Read the header
    size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile);
    cout << "Header Read " << bytesRead << " bytes." << endl;
    if (bytesRead > 0)
    {
        //Read the data
        uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      //Number     of bytes per sample
        uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; //How many samples are in the wav file?

        static const uint64_t ALL_BYTES_IN_WAVE_FILE = wavHeader.ChunkSize + headerSize;
        int8_t* allBytesBuffer = new int8_t[ALL_BYTES_IN_WAVE_FILE];

        static const uint16_t BUFFER_SIZE = 4096;
        int8_t* buffer = new int8_t[BUFFER_SIZE];

        //auto allDataBytesReadCount = fread(allBytesBuffer, sizeof buffer[0], ALL_BYTES_IN_WAVE_FILE / (sizeof buffer[0]), wavFile);

        static const uint64_t DATA_BYTES_IN_WAVE_FILE = wavHeader.ChunkSize;
        int8_t* dataBytesBuffer = new int8_t[DATA_BYTES_IN_WAVE_FILE];

        //dataBytesBuffer = allBytesBuffer + headerSize; nie trzeba przesuwania, bo fread sam przesuwa wskaznik

        signal.clear();
        int cnt = 0;
        while ((bytesRead = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), wavFile)) > 0)
        {
            for (int i = 0; i < bytesRead; ++i)
            {
                allBytesBuffer[cnt + i] = buffer[i];
            }
            cnt += bytesRead;
            /** DO SOMETHING WITH THE WAVE DATA HERE **/
            for (int i = 0; i < bytesRead; i+=2)
            {
                double sixteenBitNumber = (double)(256 * buffer[i+1] + buffer[i]);
                signal.push_back(sixteenBitNumber);
            }
            cout << "Read " << bytesRead << " bytes." << endl;
        }
        delete [] buffer;
        buffer = nullptr;
        filelength = getFileSize(wavFile);

        cout << "File is                    :" << filelength << " bytes." << endl;
        cout << "RIFF header                :" << wavHeader.RIFF[0] << wavHeader.RIFF[1] << wavHeader.RIFF[2] << wavHeader.RIFF[3] << endl;
        cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
        cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;
        cout << "Data size                  :" << wavHeader.ChunkSize << endl;

        // Display the sampling Rate from the header
        cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
        cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
        cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
        cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
        cout << "P#cnData length                :" << wavHeader.Subchunk2Size << endl;
        cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
        // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

        cout << "Block align                :" << wavHeader.blockAlign << endl;
        cout << "Data string                :" << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
    }
    fclose(wavFile);

    return 0;
}


void WAVFileLoader::setOutputLocation(QUrl url)
{
    this->outputLocation = url;
}

// find the file size
int WAVFileLoader::getFileSize(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

const char* WAVFileLoader::qUrlToChar(QUrl location)
{
    const char* myChar = location.path().toStdString().c_str();
    return myChar;
}

std::string WAVFileLoader::qUrlToString(QUrl location)
{
    std::string myStr = location.path().toStdString().c_str();
    return myStr;
}
