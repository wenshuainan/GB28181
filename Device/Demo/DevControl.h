#ifndef DEV_CONTROL_H
#define DEV_CONTROL_H

#include "Interface/9.3Control.h"

class DevControl : public Control
{
public:
    DevControl();
    ~DevControl();

public:
    virtual bool startPanLeft(int32_t ch, int32_t speed);
    virtual bool startPanRight(int32_t ch, int32_t speed);
    virtual bool startTiltUp(int32_t ch, int32_t speed);
    virtual bool startTiltDown(int32_t ch, int32_t speed);
    virtual bool startZoomIn(int32_t ch, int32_t speed);
    virtual bool startZoomOut(int32_t ch, int32_t speed);
    virtual bool stopPanLeft(int32_t ch);
    virtual bool stopPanRight(int32_t ch);
    virtual bool stopTiltUp(int32_t ch);
    virtual bool stopTiltDown(int32_t ch);
    virtual bool stopZoomIn(int32_t ch);
    virtual bool stopZoomOut(int32_t ch);
    virtual bool startFocusNear(int32_t ch, int32_t speed);
    virtual bool startFocusFar(int32_t ch, int32_t speed);
    virtual bool startIrisSmall(int32_t ch, int32_t speed);
    virtual bool startIrisBig(int32_t ch, int32_t speed);
    virtual bool stopFocusNear(int32_t ch);
    virtual bool stopFocusFar(int32_t ch);
    virtual bool stopIrisSmall(int32_t ch);
    virtual bool stopIrisBig(int32_t ch);
    virtual bool reboot();
    virtual bool startRecord(int32_t ch, int32_t stream = 0);
    virtual bool stopRecord(int32_t ch, int32_t stream = 0);
    virtual bool setGuard(int32_t ch);
    virtual bool resetGuard(int32_t ch);
    virtual bool resetAlarm(int32_t ch, int32_t method = 0, int32_t type = 0);
};

#endif