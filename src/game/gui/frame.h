#ifndef FRAME_H
#define FRAME_H

#include <ae/gui/control.h>

using namespace ae;

class Frame : public gui::Control
{
public:
    Frame();
    ~Frame() = default;

protected:
    void drawControl(Batch2D &batch_2d);
};

#endif // FRAME_H
