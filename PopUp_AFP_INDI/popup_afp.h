#pragma once

#include <libindi/defaultdevice.h>
#include <libindi/connectionplugins/connectionserial.h>
#include <libindi/indilightboxinterface.h>
#include <libindi/indidustcapinterface.h>

/**
 * INDI driver for the "PopUp AFP" auto flat panel (pop-up dust cover + dimmable
 * flat panel light), ported from the ASCOM ICoverCalibrator driver at
 * https://github.com/florindumitrescu94/PopUp_AFP.
 *
 * Hardware: an Arduino (PopUp_AFP_Arduino_V2.ino) listening on a 9600-baud
 * serial port for ">CMD#"-framed text commands and replying with "<value>#"
 * where a reply is expected. There is no acknowledgement for the fire-and-
 * forget commands (OPEN/CLOSE/LIGHTON_x/LIGHTOFF/SETOPEN_x/SETCLOSED_x), so
 * this driver polls GETSTATE on a timer to track cover motion to completion.
 */
class PopUpAFP : public INDI::DefaultDevice, public INDI::LightBoxInterface, public INDI::DustCapInterface
{
    public:
        PopUpAFP();
        virtual ~PopUpAFP() override = default;

        const char *getDefaultName() override;
        bool initProperties() override;
        bool updateProperties() override;
        void ISGetProperties(const char *dev) override;
        bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
        bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
        bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;

    protected:
        bool Handshake();
        bool Disconnect() override;
        void TimerHit() override;
        bool saveConfigItems(FILE *fp) override;

        // INDI::DustCapInterface
        IPState ParkCap() override;
        IPState UnParkCap() override;
        IPState AbortCap() override;

        // INDI::LightBoxInterface
        bool SetLightBoxBrightness(uint16_t value) override;
        bool EnableLightBox(bool enable) override;

    private:
        // Firmware's reported panel_state: 0 closed, 1 open, 2 unknown, 3 error, 4 moving.
        enum FirmwareCoverState { FW_CLOSED = 0, FW_OPEN = 1, FW_UNKNOWN = 2, FW_ERROR = 3, FW_MOVING = 4 };

        /** Send a ">CMD#" framed command. If @p response is non-null, reads back up
         *  to @p responseLen - 1 bytes up to the '#' terminator; otherwise fires and
         *  forgets (the firmware has no ack for action commands). */
        bool sendCommand(const char *cmd, char *response = nullptr, size_t responseLen = 0);

        /** Query GETSTATE and reflect it onto CAP_PARK; returns the raw firmware state
         *  (or FW_UNKNOWN on a communication failure). */
        int queryCoverState();

        bool isCoverMoving { false };

        // Options tab: the servo pulse-width positions the ASCOM driver let the user
        // calibrate per unit (firmware defaults open=950us closed=0us, Servo::write()
        // treats values >= 200 as a raw microsecond pulse width rather than degrees).
        INDI::PropertyNumber ServoPositionNP {2};
        enum { SERVO_OPEN_POSITION, SERVO_CLOSED_POSITION };

        int PortFD { -1 };
        Connection::Serial *serialConnection { nullptr };
};
