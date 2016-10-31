#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>

// Gamepad block is a C library, so must use extern "C" to include from C++
extern "C" {
#include "Gamepad.h"
}

using namespace ci;
using namespace ci::app;
using namespace std;

static bool verbose = true;

void onButtonDown(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) 
{
    if (verbose) 
    {
        console() << "Button " << buttonID << " down on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
    }
}

void onButtonUp(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) 
{
    if (verbose) 
    {
        console() << "Button " << buttonID << " up on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
    }
}

void onAxisMoved(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context) 
{
    if (verbose && (value < 0.2f || value > 0.2)) // reduce the output noise by making a dead zone
    {
        console() << "Axis " << axisID << " moved from " << lastValue <<" to " << value << " on device " << device->deviceID << " at " << timestamp << " with context " << context << std::endl;
    }
}

void onDeviceAttached(struct Gamepad_device * device, void * context) 
{
    if (verbose) 
    {
        console() << "Device ID " << device->deviceID << " attached (vendor = " << device->vendorID <<"; product = " << device->productID << ") with context" << context << std::endl;
    }
}

void onDeviceRemoved(struct Gamepad_device * device, void * context) 
{
    if (verbose) 
    {
        console() << "Device ID " << device->deviceID << " removed with context " << context << std::endl;
    }
}

// We'll create a new Cinder Application by deriving from the AppBasic class
class BasicApp : public App
{
  public:
    void setup();
    void shutdown();
    void keyDown( KeyEvent event );
    void update();
    void draw();
};

void BasicApp::setup()
{
    Gamepad_deviceAttachFunc(onDeviceAttached, (void *) 0x1);
    Gamepad_deviceRemoveFunc(onDeviceRemoved, (void *) 0x2);
    Gamepad_buttonDownFunc(onButtonDown, (void *) 0x3);
    Gamepad_buttonUpFunc(onButtonUp, (void *) 0x4);
    Gamepad_axisMoveFunc(onAxisMoved, (void *) 0x5);
    Gamepad_init();
}

void BasicApp::shutdown()
{
    Gamepad_shutdown();
}

void BasicApp::keyDown( KeyEvent event )
{
    if (event.getCode() == KeyEvent::KEY_ESCAPE)
    {
        quit();
    }
}

#define POLL_ITERATION_INTERVAL 30

void BasicApp::update()
{
	static unsigned int iterationsToNextPoll = POLL_ITERATION_INTERVAL;

    iterationsToNextPoll--;
    if (iterationsToNextPoll == 0) {
        Gamepad_detectDevices();
        iterationsToNextPoll = POLL_ITERATION_INTERVAL;
    }
}

void BasicApp::draw()
{
    Gamepad_processEvents();

    gl::clear(ColorA(178/255.f,178/255.f,178/255.f,1.0f));
}

// This line tells Cinder to actually create the application
CINDER_APP( BasicApp, RendererGl )