#include <Audio.h>
#include <AudioStream.h>

class AudioCrossfader : public AudioStream {
  public:
    AudioCrossfader(int pin) :
      AudioStream(4, inputQueueArray),
      pin(pin) {};
    void init();
    virtual void update();

  private:
    const int pin;
	  audio_block_t* inputQueueArray[4];
};
