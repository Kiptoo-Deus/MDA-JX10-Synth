/*
  ==============================================================================

    Voice.h
    Created: 24 Oct 2023 11:07:58am
    Author:  JOEL

  ==============================================================================
*/

#pragma once

struct Voice
{
    int note;
    int velocity;

    void reset()
    {
        note = 0;
        velocity = 0;
}
};