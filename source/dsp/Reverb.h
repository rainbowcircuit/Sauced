#pragma once
#include <JuceHeader.h>
#include "Filters.h"

class Reverb
{
public:
    Reverb() {}

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        this->sampleRate = sampleRate;

        // diffuser allpass filters
        difAp1.prepareToPlay(sampleRate, samplesPerBlock);
        difAp2.prepareToPlay(sampleRate, samplesPerBlock);
        difAp3.prepareToPlay(sampleRate, samplesPerBlock);
        difAp4.prepareToPlay(sampleRate, samplesPerBlock);
        difAp1.setValues(0.75, 5);
        difAp2.setValues(0.75, 11);
        difAp3.setValues(0.625, 17);
        difAp4.setValues(0.625, 23);

        // modulated allpass filters
        modAp1.prepareToPlay(sampleRate, samplesPerBlock);
        modAp2.prepareToPlay(sampleRate, samplesPerBlock);
        modAp3.prepareToPlay(sampleRate, samplesPerBlock);
        modAp4.prepareToPlay(sampleRate, samplesPerBlock);

        // state variable filters
        lpL.prepareToPlay(sampleRate);
        lpR.prepareToPlay(sampleRate);
        hpL.prepareToPlay(sampleRate);
        hpR.prepareToPlay(sampleRate);
        hpL.setCoefficients(120, 0.707);
        hpR.setCoefficients(120, 0.707);

        // delay line specs:
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.numChannels = 1;
        spec.maximumBlockSize = samplesPerBlock;
        dlL.prepare(spec);
        dlR.prepare(spec);

        modAngle1 = 0.015 / sampleRate;
        modAngle2 = 0.01 / sampleRate;
    }

    void setValues(float outputLevel, float decayTimeInMs, float dampFreqInHz, float sizeScaling)
    {
        level = outputLevel;
        size = 1.0f + sizeScaling * 4.0f;

        // dampening one pole
        lpL.setCoefficients(dampFreqInHz, 0.707);
        lpR.setCoefficients(dampFreqInHz, 0.707);

        // decay time
        float decayInSamples = (decayTimeInMs / 1000.0f) * sampleRate;
        float loopLengthL = 313.0f + (7.0f + 11.0f) * size;
        float loopLengthR = 251.0f + (13.0f + 19.0f) * size;
        float loopIterations = decayInSamples / ((loopLengthL + loopLengthR) * 0.5f);
        feedBackAmount = std::exp(-6.9077552789821f / loopIterations);
    }

    void processBlock(juce::AudioBuffer<float> &buffer)
    {
        auto dataL = buffer.getReadPointer(0);
        auto dataR = buffer.getReadPointer(1);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float x = (dataL[sample] + dataR[sample]) * 0.5f;
            float diffusion = difAp4.processSample(difAp3.processSample(difAp2.processSample(difAp1.processSample(x))));

            // calculate modulation
            float lfo1 = std::abs(modPhase1 - 0.5f) * 4.0 - 1; // polarity
            float lfo2 = std::abs(modPhase2 - 0.5f) * 4.0 - 1;

            modAp1.setValues(0.5, 7 * size + lfo1 * 5);
            modAp2.setValues(0.5, 11 * size + lfo2 * 6);
            modAp3.setValues(0.5, 13 * size + lfo1 * 5);
            modAp4.setValues(0.5, 19 * size + lfo2 * 9);

            // left
            float allPassL1 = modAp1.processSample(diffusion + feedbackR);
            float allPassL2 = modAp2.processSample(allPassL1);
            float lowPassL = lpL.processSample(allPassL2, 0);
            float highPassL = hpL.processSample(lowPassL, 2);
            dlL.pushSample(0, lowPassL * feedBackAmount);
            feedbackL = dlL.popSample(0, 313);

            // right
            float allPassR1 = modAp3.processSample(diffusion + feedbackL);
            float allPassR2 = modAp4.processSample(allPassR1);
            float lowPassR = lpR.processSample(allPassR2, 0);
            float highPassR = hpR.processSample(lowPassR, 2);
            dlR.pushSample(0, lowPassR * feedBackAmount);
            feedbackR = dlR.popSample(0, 251);

            // increment lfo
            modPhase1 += modAngle1;
            if (modPhase1 >= 1.0)
                modPhase1 -= 1.0;
            modPhase2 += modAngle2;
            if (modPhase2 >= 1.0)
                modPhase2 -= 1.0;

            // write to outgoing buffer
            float outL = highPassL * level;
            float outR = highPassR * level;
            buffer.addSample(0, sample, outL);
            buffer.addSample(1, sample, outR);
        }
    }

private:
    double sampleRate = 48000.0;
    float level = 1.0f, damping = 0.5, decay = 0.9f, size = 1.0f, feedBackAmount = 0.95f;
    float feedbackL = 0.0f, feedbackR = 0.0f;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> dlL, dlR;
    SVF lpL, lpR, hpL, hpR;
    APF difAp1, difAp2, difAp3, difAp4,
        modAp1, modAp2, modAp3, modAp4;

    double modPhase1 = 0.0,
           modAngle1 = 0.0,
           modPhase2 = 0.0,
           modAngle2 = 0.0;
};

