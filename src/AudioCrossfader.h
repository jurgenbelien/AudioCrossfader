#include <Audio.h>
#include <AudioStream.h>

class AudioCrossfader : public AudioStream {
  public:
    AudioCrossfader() : AudioStream(4, inputQueueArray) {};
    virtual void update();
    // -1.0f for A, +1.0f for B, 0.0f is equal mix
    void set(float balance);

  private:
    float gainOffset;
	  audio_block_t* inputQueueArray[4];
};
