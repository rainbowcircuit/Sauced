class Phasor {
    void prepare(double sampleRate) {
        this->sampleRate = sampleRate;
    }

    void setRate(float rateInHz) {
        this->angle = rateInHz / sampleRate;
    }

    void reset(){
        this->phase = 0.0f;
        this->angle = 0.0f;
    }

    float process() {
        phase += angle; if (phase >= 1.0f) { angle -= 1.0f; }
        return phase;
    }

    float process(bool start){
        if (!isActive) { isActive = true; }
        while (isActive) {
            phase += angle;
            if (phase >= 1.0f) { 
                angle = 1.0f; 
                isActive = false;
            }
        }

        return phase;
    }

    double sampleRate = 48000.0;
    float angle = 0, phase = 0;
    bool isActive = false;
};

