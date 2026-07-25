#pragma once
#include <JuceHeader.h>

class Plugin {
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        this->sampleRate = sampleRate;
        this->bufferSize = sampleRate * 64; 
        memory.setSize(2, bufferSize);
        memory.clear();
        reset();
    }

    void reset()
    {
        // clean this up later
        readIndex[0].startIndex = (int)(bufferSize * 0.25);
        readIndex[0].endIndex = (int)(bufferSize * 0.5);
        readIndex[0].gain.reset(sampleRate, 0.005);

        readIndex[1].startIndex = (int)(bufferSize * 0.5);
        readIndex[1].endIndex = (int)(bufferSize * 0.75);
        readIndex[1].gain.reset(sampleRate, 0.005);

        readIndex[2].startIndex = (int)(bufferSize * 0.75);
        readIndex[2].endIndex = (int)(bufferSize);
        readIndex[2].gain.reset(sampleRate, 0.005);  
    }
    
    void advanceReadIndex(bool clock){
        for(int i = 0; i < 3; i++) {
            if (clock) { 
                bool randIndex = rd.nextInt(3);

                readIndex[randIndex].isActive = true;   
                readIndex[randIndex].index = readIndex[i].startIndex;
                readIndex[randIndex].gain.setTargetValue(1.0f);

            }

            if (readIndex[i].isActive){
                if (readIndex[i].isActive && ++readIndex[i].index >= readIndex[i].endIndex) {
                    readIndex[i].index = readIndex[i].startIndex;
                    readIndex[i].gain.setTargetValue(0.0f);

                    if (!readIndex[i].gain.isSmoothing()) readIndex[i].isActive = false;
                }
            }
        }
    }

    bool processClock(juce::AudioPlayHead* playhead){
        if (playhead == nullptr) return false;

        auto position = playhead->getPosition();
        if (!position) return false;
        
        auto ppqOpt = position->getPpqPosition();
        if (! ppqOpt) return false;
        const double ppq = *ppqOpt;

        double quartersPerMeasure = 4.0;
        if (auto timeSignature = position->getTimeSignature())
        quartersPerMeasure = timeSignature->numerator * (4.0 / timeSignature->denominator);

        const int currentMeasure = (int) std::floor (ppq / quartersPerMeasure);
        const bool isDownbeat = (currentMeasure != lastMeasure);
        lastMeasure = currentMeasure;

        return isDownbeat;
    }

    
    void processBlock(juce::AudioBuffer<float> &buffer, juce::AudioPlayHead* playhead) noexcept
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            writeIndex++;
            if (writeIndex >= bufferSize) { writeIndex -= bufferSize; }
            advanceReadIndex(processClock(playhead));

            for(int channel = 0; channel < buffer.getNumChannels(); channel++)
            {
                auto *bufferData = buffer.getWritePointer(channel);
                auto *memoryData = memory.getWritePointer(channel);

                // write into buffer
                memoryData[writeIndex] = bufferData[sample];

                // read from buffer and add to output buffer
                bufferData[sample] += (memoryData[readIndex[0].index] * readIndex[0].gain.getNextValue())
                                    + (memoryData[readIndex[1].index] * readIndex[1].gain.getNextValue())
                                    + (memoryData[readIndex[2].index] * readIndex[2].gain.getNextValue());

            }
        }
    }
    
private:
    double sampleRate = 48000.0;
    juce::AudioBuffer<float> memory;
    double tempo = 120.0;
    int lastMeasure = -1;

    int bufferSize;
    int writeIndex = 0;

    juce::Random rd;

    struct readValues {
        bool isActive = false;
        int index = 0, startIndex = 0, endIndex = 0;
        juce::SmoothedValue<float> gain { 0.0f };
    };

    std::array<readValues, 3> readIndex;
};