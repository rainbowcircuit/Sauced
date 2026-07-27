#include <JuceHeader.h>

class APF
{
public:
    /*
        This class implements the difference equation:
        y[n] = -g*x[n] + x[n-d] + g*y[n-d]
    */

    void prepareToPlay(double sampleRate, int maximumBlockSize)
    {
        this->sampleRate = sampleRate;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = maximumBlockSize;
        spec.numChannels = 1;

        dl.prepare(spec);
        dl.setMaximumDelayInSamples(sampleRate);
        dl.reset();
    }

    float setValues(float gain, float delayInMilliseconds)
    {
        this->gain = gain;
        delayInSamples = static_cast<int>(std::round((delayInMilliseconds / 1000.0) * sampleRate));
    }

    float processSample(float x)
    {
        float y = -gain * x + dl.popSample(0, delayInSamples);
        dl.pushSample(0, x + gain * y);

        return y;
    }

private:
    double sampleRate = 48000.0, phase = 0.0;
    bool isModulated = false;
    int delayInSamples = 24;
    float gain = 0.5f;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> dl;
};

class SVF
{
public:
    void prepareToPlay(float sampleRate)
    {
        this->sampleRate = sampleRate;
        reset();
    }

    void setCoefficients(float cf, float q)
    {
        g = math.tan(M_PI * cf / sampleRate);
        k = 1.0f / q;
        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    void reset()
    {
        g = 0.0f, k = 0.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;
        z1 = 0.0f, z2 = 0.0f;
    }

    float processSample(float x, int type) noexcept
    {
        float v3 = x - z2;
        float v1 = a1 * z1 + a2 * v3;
        float v2 = z2 + a2 * z1 + a3 * v3;

        z1 = 2.0f * v1 - z1;
        z2 = 2.0f * v2 - z2;

        float y;
        switch (type)
        {
        case 0: // LP
            y = v2;
            break;

        case 1: // BP
            y = v1 * k;
            break;

        case 2: // HP
            y = x - k * v1 - v2;
            break;
        }
        return y;
    }

    double sampleRate = 48000.0;
    float g = 0.0f, k = 0.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, z1 = 0.0f, z2 = 0.0f;
    juce::dsp::FastMathApproximations math;
};