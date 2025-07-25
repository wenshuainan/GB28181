#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include "MANSCDP/A.2.3Control.h"

class Control
{
public:
    Control() {}
    virtual ~Control() {}

public:
    virtual bool startPanLeft(int32_t ch, int32_t speed) = 0;
    virtual bool startPanRight(int32_t ch, int32_t speed) = 0;
    virtual bool startTiltUp(int32_t ch, int32_t speed) = 0;
    virtual bool startTiltDown(int32_t ch, int32_t speed) = 0;
    virtual bool startZoomIn(int32_t ch, int32_t speed) = 0;
    virtual bool startZoomOut(int32_t ch, int32_t speed) = 0;
    virtual bool stopPanLeft(int32_t ch) = 0;
    virtual bool stopPanRight(int32_t ch) = 0;
    virtual bool stopTiltUp(int32_t ch) = 0;
    virtual bool stopTiltDown(int32_t ch) = 0;
    virtual bool stopZoomIn(int32_t ch) = 0;
    virtual bool stopZoomOut(int32_t ch) = 0;
    virtual bool startFocusNear(int32_t ch, int32_t speed) = 0;
    virtual bool startFocusFar(int32_t ch, int32_t speed) = 0;
    virtual bool startIrisSmall(int32_t ch, int32_t speed) = 0;
    virtual bool startIrisBig(int32_t ch, int32_t speed) = 0;
    virtual bool stopFocusNear(int32_t ch) = 0;
    virtual bool stopFocusFar(int32_t ch) = 0;
    virtual bool stopIrisSmall(int32_t ch) = 0;
    virtual bool stopIrisBig(int32_t ch) = 0;
    virtual bool reboot() = 0;
    virtual bool startRecord(int32_t ch, int32_t stream = 0) = 0;
    virtual bool stopRecord(int32_t ch, int32_t stream = 0) = 0;
    virtual bool setGuard(int32_t ch) = 0;
    virtual bool resetGuard(int32_t ch) = 0;
    virtual bool resetAlarm(int32_t ch, int32_t method = 0, int32_t type = 0) = 0;
};

#endif