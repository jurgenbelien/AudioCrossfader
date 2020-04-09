#include "AudioCrossfader.h"

/**
 * Minimally modified versions of static functions found in
 * Teensy Audio Library's AudioMixer4
 * @see: https://github.com/PaulStoffregen/Audio/blob/master/mixer.cpp
 */
static void applyGain(int16_t* data, float gain) {
  int32_t multiplier = gain * 65536.0f;
	uint32_t* p = (uint32_t*) data;
	const uint32_t* end = (uint32_t*) (data + AUDIO_BLOCK_SAMPLES);
 	do {
		uint32_t tmp32 = *p;
		int32_t val1 = signed_multiply_32x16b(multiplier, tmp32);
		int32_t val2 = signed_multiply_32x16t(multiplier, tmp32);
		val1 = signed_saturate_rshift(val1, 16, 0);
		val2 = signed_saturate_rshift(val2, 16, 0);
		*p++ = pack_16b_16b(val2, val1);
	} while (p < end);
}
static void applyGainThenAdd(int16_t* data, const int16_t* in, float gain) {
  int32_t multiplier = gain * 65536.0f;
	uint32_t* dst = (uint32_t*) data;
	const uint32_t* src = (uint32_t*) in;
	const uint32_t* end = (uint32_t*) (data + AUDIO_BLOCK_SAMPLES);

  do {
    uint32_t tmp32 = *src++; // read 2 samples from *data
    int32_t val1 = signed_multiply_32x16b(multiplier, tmp32);
    int32_t val2 = signed_multiply_32x16t(multiplier, tmp32);
    val1 = signed_saturate_rshift(val1, 16, 0);
    val2 = signed_saturate_rshift(val2, 16, 0);
    tmp32 = pack_16b_16b(val2, val1);
    uint32_t tmp32b = *dst;
    *dst++ = signed_add_16_and_16(tmp32, tmp32b);
  } while (dst < end);
}

void AudioCrossfader::update() {
  audio_block_t* in;
  audio_block_t* outLeft = NULL;
  audio_block_t* outRight = NULL;

  for (uint8_t channel = 0; channel < 4; channel++) {
    audio_block_t*& out = (channel % 2) ? outLeft : outRight;
    if (!out) {
      out = receiveWritable(channel);
      if (out) {
        applyGain(out->data, 0.5f - gainOffset);
      }
    } else {
      in = receiveReadOnly(channel);
      if (in) {
        applyGainThenAdd(out->data, in->data, 0.5f + gainOffset);
        release(in);
      }
    }
  }

  if (outLeft) {
    if(outRight) {
      transmit(outRight, 1);
      release(outRight);
    } else {
      transmit(outLeft, 1);
    }
    transmit(outLeft, 0);
    release(outLeft);
  }
}

void AudioCrossfader::set(float balance) {
  gainOffset = balance / 2.0f;
}
