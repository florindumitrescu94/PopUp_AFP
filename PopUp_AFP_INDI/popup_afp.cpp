#include "popup_afp.h"

#include <libindi/indicom.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <termios.h>
#include <unistd.h>

static std::unique_ptr<PopUpAFP> device(new PopUpAFP());

// ─────────────────────────────────────────────────────────────────────────────
//  Construction / identification
// ─────────────────────────────────────────────────────────────────────────────

PopUpAFP::PopUpAFP() : INDI::LightBoxInterface(this), INDI::DustCapInterface(this)
{
    setVersion(1, 0);
}

const char *PopUpAFP::getDefaultName()
{
    return "PopUp AFP";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Property definitions
// ─────────────────────────────────────────────────────────────────────────────

bool PopUpAFP::initProperties()
{
    INDI::DefaultDevice::initProperties();

    LI::initProperties(MAIN_CONTROL_TAB, LightBoxInterface::CAN_DIM);
    DI::initProperties(MAIN_CONTROL_TAB);

    LightIntensityNP[0].setMin(0);
    LightIntensityNP[0].setMax(255);
    LightIntensityNP[0].setStep(1);

    // Options tab: the servo angle calibration the ASCOM SetupDialog exposed as
    // "Open position" / "Closed position". The build doc (README.md) has the
    // user set these in degrees on the TD-7120MG's 0-270 range — e.g. fully
    // open = 270, closed = 0 — matching the firmware's own panel_open_pos=270 /
    // panel_closed_pos=0 defaults, so that's what this defaults to too (the
    // ASCOM driver's unused "950" profile constant is not a real-world value;
    // nothing in the project ever sets it).
    ServoPositionNP[SERVO_OPEN_POSITION].fill("OPEN_POSITION", "Open (deg)", "%.0f", 0, 270, 1, 270);
    ServoPositionNP[SERVO_CLOSED_POSITION].fill("CLOSED_POSITION", "Closed (deg)", "%.0f", 0, 270, 1, 0);
    ServoPositionNP.fill(getDeviceName(), "SERVO_POSITIONS", "Servo Calibration", OPTIONS_TAB, IP_RW, 60, IPS_IDLE);

    serialConnection = new Connection::Serial(this);
    serialConnection->registerHandshake([&]()
    {
        return Handshake();
    });
    serialConnection->setDefaultBaudRate(Connection::Serial::B_9600);
    registerConnection(serialConnection);

    setDriverInterface(AUX_INTERFACE | LIGHTBOX_INTERFACE | DUSTCAP_INTERFACE);

    addDebugControl();
    setDefaultPollingPeriod(1000);

    return true;
}

bool PopUpAFP::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    LI::updateProperties();
    DI::updateProperties();

    if (isConnected())
        defineProperty(ServoPositionNP);
    else
        deleteProperty(ServoPositionNP);

    return true;
}

void PopUpAFP::ISGetProperties(const char *dev)
{
    INDI::DefaultDevice::ISGetProperties(dev);
    LI::ISGetProperties(dev);
}

bool PopUpAFP::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (DI::processSwitch(dev, name, states, names, n))
            return true;

        if (LI::processSwitch(dev, name, states, names, n))
            return true;
    }

    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool PopUpAFP::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (ServoPositionNP.isNameMatch(name))
        {
            ServoPositionNP.update(values, names, n);
            ServoPositionNP.setState(IPS_OK);

            if (isConnected())
            {
                char cmd[32];
                snprintf(cmd, sizeof(cmd), ">SETOPEN_%d#",
                         static_cast<int>(ServoPositionNP[SERVO_OPEN_POSITION].getValue()));
                sendCommand(cmd);
                snprintf(cmd, sizeof(cmd), ">SETCLOSED_%d#",
                         static_cast<int>(ServoPositionNP[SERVO_CLOSED_POSITION].getValue()));
                sendCommand(cmd);
            }

            ServoPositionNP.apply();
            return true;
        }

        if (LI::processNumber(dev, name, values, names, n))
            return true;
    }

    return INDI::DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool PopUpAFP::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (LI::processText(dev, name, texts, names, n))
            return true;
    }

    return INDI::DefaultDevice::ISNewText(dev, name, texts, names, n);
}

bool PopUpAFP::saveConfigItems(FILE *fp)
{
    INDI::DefaultDevice::saveConfigItems(fp);
    LI::saveConfigItems(fp);
    ServoPositionNP.save(fp);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Connection / disconnection
// ─────────────────────────────────────────────────────────────────────────────

bool PopUpAFP::Handshake()
{
    PortFD = serialConnection->getPortFD();

    // Nothing like the ESP32 boot-noise problem sv241 has — this is a bare
    // Arduino sketch with no bootloader chatter on the serial line — but a
    // short settle + flush before the first command is cheap insurance.
    usleep(200000);
    tcflush(PortFD, TCIOFLUSH);

    char resp[16] = {0};
    if (!sendCommand(">GETSTATE#", resp, sizeof(resp)))
    {
        LOG_ERROR("No response from PopUp AFP — check the serial port and that the "
                   "Arduino is running PopUp_AFP_Arduino_V2.ino.");
        return false;
    }

    LOGF_INFO("PopUp AFP connected. Cover state code: %s", resp);

    // Push the configured servo calibration to the device, mirroring the ASCOM
    // driver's own connect handshake (it is held in RAM on the Arduino, not
    // EEPROM, so it has to be re-sent on every connection).
    char cmd[32];
    snprintf(cmd, sizeof(cmd), ">SETOPEN_%d#", static_cast<int>(ServoPositionNP[SERVO_OPEN_POSITION].getValue()));
    sendCommand(cmd);
    snprintf(cmd, sizeof(cmd), ">SETCLOSED_%d#", static_cast<int>(ServoPositionNP[SERVO_CLOSED_POSITION].getValue()));
    sendCommand(cmd);

    // Sync the light's actual on/off + brightness state into the GUI. (The ASCOM
    // driver never actually does this — its CalibratorState getter reads a
    // `lightState` field that nothing ever assigns, so it always reports Off
    // regardless of the real panel. Fixed here rather than carried over.)
    char light[16] = {0};
    if (sendCommand(">GETLIGHTSTATE#", light, sizeof(light)))
    {
        bool lightOn = (atoi(light) != 0);
        LightSP[LightBoxInterface::FLAT_LIGHT_ON].setState(lightOn ? ISS_ON : ISS_OFF);
        LightSP[LightBoxInterface::FLAT_LIGHT_OFF].setState(lightOn ? ISS_OFF : ISS_ON);
        LightSP.setState(IPS_OK);
    }

    char brightness[16] = {0};
    if (sendCommand(">GETLIGHT#", brightness, sizeof(brightness)))
    {
        LightIntensityNP[0].setValue(atoi(brightness));
        LightIntensityNP.setState(IPS_OK);
    }

    queryCoverState();

    return true;
}

bool PopUpAFP::Disconnect()
{
    LOG_INFO("PopUp AFP disconnected.");
    PortFD = -1;
    return true;
}

void PopUpAFP::TimerHit()
{
    if (!isConnected())
        return;

    // Only poll while a park/unpark is thought to be in progress — a healthy
    // idle cover doesn't need a 1 Hz GETSTATE round-trip.
    if (isCoverMoving)
        queryCoverState();

    SetTimer(getCurrentPollingPeriod());
}

// ─────────────────────────────────────────────────────────────────────────────
//  DustCapInterface
// ─────────────────────────────────────────────────────────────────────────────

IPState PopUpAFP::ParkCap()
{
    if (!sendCommand(">CLOSE#"))
        return IPS_ALERT;

    isCoverMoving = true;
    SetTimer(getCurrentPollingPeriod());
    return IPS_BUSY;
}

IPState PopUpAFP::UnParkCap()
{
    if (!sendCommand(">OPEN#"))
        return IPS_ALERT;

    isCoverMoving = true;
    SetTimer(getCurrentPollingPeriod());
    return IPS_BUSY;
}

IPState PopUpAFP::AbortCap()
{
    // NOTE (firmware gap): PopUp_AFP_Arduino_V2.ino's processSerialCommand() has
    // no "HALT" branch today — this command is a no-op on the device, exactly as
    // it already is in the ASCOM driver's HaltCover(). Sent anyway so Abort
    // starts working for free the moment the firmware grows a case for it.
    sendCommand(">HALT#");
    isCoverMoving = false;
    return IPS_OK;
}

int PopUpAFP::queryCoverState()
{
    char resp[16] = {0};
    if (!sendCommand(">GETSTATE#", resp, sizeof(resp)))
        return FW_UNKNOWN;

    int state = atoi(resp);

    switch (state)
    {
        case FW_CLOSED:
            isCoverMoving = false;
            ParkCapSP.reset();
            ParkCapSP[CAP_PARK].setState(ISS_ON);
            ParkCapSP.setState(IPS_OK);
            ParkCapSP.apply();
            break;

        case FW_OPEN:
            isCoverMoving = false;
            ParkCapSP.reset();
            ParkCapSP[CAP_UNPARK].setState(ISS_ON);
            ParkCapSP.setState(IPS_OK);
            ParkCapSP.apply();
            break;

        case FW_MOVING:
            isCoverMoving = true;
            ParkCapSP.setState(IPS_BUSY);
            ParkCapSP.apply();
            break;

        case FW_ERROR:
            isCoverMoving = false;
            ParkCapSP.setState(IPS_ALERT);
            ParkCapSP.apply();
            LOG_ERROR("PopUp AFP firmware reports a cover error (GETSTATE = 3).");
            break;

        default: // FW_UNKNOWN, or anything the firmware hasn't told us about yet
            isCoverMoving = false;
            ParkCapSP.setState(IPS_IDLE);
            ParkCapSP.apply();
            break;
    }

    return state;
}

// ─────────────────────────────────────────────────────────────────────────────
//  LightBoxInterface
// ─────────────────────────────────────────────────────────────────────────────

bool PopUpAFP::SetLightBoxBrightness(uint16_t value)
{
    // Only take effect immediately if the light is already on — matches the
    // ASCOM driver's CalibratorOn(Brightness), which is the only place it ever
    // sends LIGHTON_. Setting brightness while off just stages the value for
    // the next EnableLightBox(true).
    if (LightSP.findOnSwitchIndex() != LightBoxInterface::FLAT_LIGHT_ON)
        return true;

    char cmd[32];
    snprintf(cmd, sizeof(cmd), ">LIGHTON_%u#", value);
    return sendCommand(cmd);
}

bool PopUpAFP::EnableLightBox(bool enable)
{
    if (enable)
    {
        char cmd[32];
        snprintf(cmd, sizeof(cmd), ">LIGHTON_%d#", static_cast<int>(LightIntensityNP[0].getValue()));
        return sendCommand(cmd);
    }

    return sendCommand(">LIGHTOFF#");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Serial communication
// ─────────────────────────────────────────────────────────────────────────────

bool PopUpAFP::sendCommand(const char *cmd, char *response, size_t responseLen)
{
    if (PortFD < 0)
        return false;

    tcflush(PortFD, TCIFLUSH);

    int nbytesWritten = 0;
    int rc = tty_write_string(PortFD, cmd, &nbytesWritten);
    if (rc != TTY_OK)
    {
        char errMsg[MAXRBUF];
        tty_error_msg(rc, errMsg, MAXRBUF);
        LOGF_ERROR("Serial write error on '%s': %s", cmd, errMsg);
        return false;
    }

    if (response == nullptr || responseLen == 0)
        return true;

    int nbytesRead = 0;
    rc = tty_nread_section(PortFD, response, static_cast<int>(responseLen) - 1, '#', 3, &nbytesRead);
    if (rc != TTY_OK)
    {
        char errMsg[MAXRBUF];
        tty_error_msg(rc, errMsg, MAXRBUF);
        LOGF_ERROR("Serial read error after '%s': %s", cmd, errMsg);
        return false;
    }

    response[nbytesRead] = '\0';
    return true;
}
