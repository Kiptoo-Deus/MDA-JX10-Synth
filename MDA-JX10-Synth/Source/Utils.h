/*
  ==============================================================================

    Utils.h
    Created: 30 Oct 2023 11:49:47am
    Author:  JOEL

  ==============================================================================
*/

#pragma once

inline void protectYourEars(float* buffer, int sampleCount)
{
    if (buffer == nullptr) { return; }
    bool firstWarning = true;
    for (int i; i < sampleCount; ++i) {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x)) {
            DBG("!!!WARNING: nan detected in audio buffer, silencing!!!");
            silence = true;
        }
        else if (std::isinf(x)) {
            DBG("!!!WARNG: inf dected in audio buffer, silencing!!!");
            silence = true;
        }
        else if (x < -2.0f || x>2.0f) {
            DBG("!!!WARNING:: sample out of range, silencing");
            silence = true;
        }
        else if (x < -1.0f) {
            if (firstWarning) {
                
                DBG("!!!WARNING: sample out of range, clamping !!! ");
                firstWarning = false;
            }
            buffer[i] = -1.0f;
        }else if (x > 1.0f){
            if (firstWarning) {
                DBG("!!!WARNING: sample out of range, clamping!!!");
                firstWarning = false;
            }
            buffer[i] = 1.0f;
           }
        if (silence) {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
}