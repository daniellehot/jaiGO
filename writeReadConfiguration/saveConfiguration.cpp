// *****************************************************************************
//
//     Copyright (c) 2013, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// Shows how to use PvConfigurationWriter to store a configuration of the system and PvConfigurationReader to retrieve it.
//

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>


PV_INIT_SIGNAL_HANDLER();

#define FILE_NAME ( "config.pvxml" )
#define DEVICE_CONFIGURATION_TAG ( "DeviceConfiguration" )
#define STREAM_CONFIGURAITON_TAG ( "StreamConfiguration" )


//
//  Store device and stream configuration.
//  Also store a string information.
//

bool StoreConfiguration( const PvString &aConnectionID )
{
    PvResult lResult;
    PvConfigurationWriter lWriter;

    // Connect to the GigE Vision or USB3 Vision device
    cout << "Connecting to device" << endl;
    PvDevice *lDevice = PvDevice::CreateAndConnect( aConnectionID, &lResult );
    if ( !lResult.IsOK() )
    {
        cout << "Unable to connect to device" << endl;
        PvDevice::Free( lDevice );
        return false;
    }
    // Store  with a PvDevice.
    cout << "Store device configuration" << endl;
    lWriter.Store( lDevice, DEVICE_CONFIGURATION_TAG );

    // Create and open PvStream
    cout << "Store stream configuration" << endl;
    PvStream *lStream = PvStream::CreateAndOpen( aConnectionID, &lResult );
    if ( !lResult.IsOK() )
    {
        cout << "Unable to open stream object from device" << endl;
        lDevice->Disconnect();
        PvDevice::Free( lDevice );
        return false;
    }
    // Store with a PvStream
    lWriter.Store( lStream, STREAM_CONFIGURAITON_TAG );

    // Save configuration file
    lWriter.Save( FILE_NAME );

    PvStream::Free( lStream );
    PvDevice::Free( lDevice );

    return true;
}



//
// Main function.
//

int main()
{
    PV_SAMPLE_INIT();

    // Select device
    PvString lConnectionID;
    if ( !PvSelectDevice( &lConnectionID ) )
    {
        cout << "No device selected." << endl;
        return 0;
    }

    // Create the Buffer and fill it.
    cout << "ConfigurationReader sample" << endl << endl;
    cout << "1. Store the configuration" << endl << endl;
    if ( !StoreConfiguration( lConnectionID ) )
    {
        cout << "Cannot store the configuration correctly";
        return 0;
    }

    cout << endl;
    cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    PV_SAMPLE_TERMINATE();

    return 0;
}
