#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvBuffer.h>
#include <PvPipeline.h>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

PV_INIT_SIGNAL_HANDLER();

#define BUFFER_COUNT ( 16 )

///
/// Function Prototypes
///
bool SelectDevice( PvString *aConnectionID, PvDeviceInfoType *aType = NULL );
PvDevice *ConnectToDevice( const PvString &aConnectionID );
PvStream *OpenStream( const PvString &aConnectionID );
void ConfigureStream( PvDevice *aDevice, PvStream *aStream );
PvPipeline *CreatePipeline( PvDevice *aDevice, PvStream *aStream );
void AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline );

//
// Main function
//
int main()
{
    cout<<"stream.cpp"<<endl;
    
    PvString lConnectionID;
    if (SelectDevice(&lConnectionID))
    {   
        PvDevice *lDevice = NULL;
        lDevice = ConnectToDevice(lConnectionID);
        if (lDevice != NULL)
        {
            PvStream *lStream = NULL;
            lStream = OpenStream(lConnectionID);
            
            if ( lStream != NULL )
            {
                ConfigureStream( lDevice, lStream );
                PvPipeline *lPipeline = NULL;
                lPipeline = CreatePipeline( lDevice, lStream );
                if( lPipeline )
                {
                    AcquireImages( lDevice, lStream, lPipeline );
                    delete lPipeline;
                }
                
                // Close the stream
                cout << "Closing stream" << endl;
                lStream->Close();
                PvStream::Free( lStream );
            }

            // Disconnect the device
            cout << "Disconnecting device" << endl;
            lDevice->Disconnect();
            PvDevice::Free( lDevice );
        }
    } 
    return 1;
}

bool SelectDevice( PvString *aConnectionID, PvDeviceInfoType *aType)
{
    PvResult lResult;
    const PvDeviceInfo *lSelectedDI = NULL;
    PvSystem lSystem;

    cout << endl << "Detecting devices." << endl;
    
    lSystem.Find();
    vector<const PvDeviceInfo *> lDIVector;
    for ( uint32_t i = 0; i < lSystem.GetInterfaceCount(); i++ )
    {
        const PvInterface *lInterface = dynamic_cast<const PvInterface *>( lSystem.GetInterface( i ) );
        if ( lInterface != NULL )
        {
            //cout << "   " << lInterface->GetDisplayID().GetAscii() << endl;
            for ( uint32_t j = 0; j < lInterface->GetDeviceCount(); j++ )
            {
                const PvDeviceInfo *lDI = dynamic_cast<const PvDeviceInfo *>( lInterface->GetDeviceInfo( j ) );
                if ( lDI != NULL )
                {
                    lDIVector.push_back( lDI );
                    //cout << "[" << ( lDIVector.size() - 1 ) << "]" << "\t" << lDI->GetDisplayID().GetAscii() << endl;
                }					
            }
        }
    }
    
    if ( lDIVector.size() == 0)
    {
        cout << "No device found!" << endl;
        return false;
    } else 
    {
        cout << "Found "<< lDIVector.size() << " devices. \n";
        for (uint i = 0; i < lDIVector.size(); i++)
        {
            cout << i << ") " <<lDIVector[i]->GetDisplayID().GetAscii()<<endl;
        }
        cout << "Connecting to " << lDIVector.back()->GetDisplayID().GetAscii()<<endl;
        *aConnectionID = lDIVector.back()->GetConnectionID();
        return true;
    }
}

PvDevice *ConnectToDevice( const PvString &aConnectionID )
{
    PvDevice *lDevice;
    PvResult lResult;
    lDevice = PvDevice::CreateAndConnect( aConnectionID, &lResult );
    if ( lDevice == NULL )
    {
        cout << "Unable to connect to device: "
        << lResult.GetCodeString().GetAscii()
        << " ("
        << lResult.GetDescription().GetAscii()
        << ")" << endl;
    }

    if ( lResult.HasDescription() ) 
    {
        cout << lResult.GetDescription().GetAscii() << endl;
    }
 
    return lDevice;
}

PvStream *OpenStream( const PvString &aConnectionID )
{
    PvStream *lStream;
    PvResult lResult;

    // Open stream to the GigE Vision or USB3 Vision device
    cout << "Opening stream from device." << endl;
    lStream = PvStream::CreateAndOpen( aConnectionID, &lResult );
    if ( lStream == NULL )
    {
        cout << "Unable to stream from device." << endl;
    }

    return lStream;
}

void ConfigureStream( PvDevice *aDevice, PvStream *aStream )
{
    // If this is a GigE Vision device, configure GigE Vision specific streaming parameters
    PvDeviceGEV* lDeviceGEV = dynamic_cast<PvDeviceGEV *>( aDevice );
    if ( lDeviceGEV != NULL )
    {
        cout << "GigE Vision device detected, configuring GigE Vision specific streaming parameters.\n";
        PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>( aStream );

        // Negotiate packet size
        lDeviceGEV->NegotiatePacketSize();

        // Configure device streaming destination
        lDeviceGEV->SetStreamDestination( lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort() );
    }
}

PvPipeline *CreatePipeline( PvDevice *aDevice, PvStream *aStream )
{
    // Create the PvPipeline object
    PvPipeline* lPipeline = new PvPipeline( aStream );

    if ( lPipeline != NULL )
    {        
        // Reading payload size from device
        uint32_t lSize = aDevice->GetPayloadSize();
    
        // Set the Buffer count and the Buffer size
        lPipeline->SetBufferCount( BUFFER_COUNT );
        lPipeline->SetBufferSize( lSize );
    }
    
    return lPipeline;
}

void AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline )
{
    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = aDevice->GetParameters();

    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Note: the pipeline must be initialized before we start acquisition
    cout << "Starting pipeline" << endl;
    aPipeline->Start();

    // Get stream parameters
    PvGenParameterArray *lStreamParams = aStream->GetParameters();

    // Map a few GenICam stream stats counters
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRate" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "Bandwidth" ) );

    // Enable streaming and send the AcquisitionStart command
    cout << "Enabling streaming and sending AcquisitionStart command." << endl;
    aDevice->StreamEnable();
    lStart->Execute();

    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    // Acquire images until the user instructs us to stop.
    cout << endl << "<press a key to stop streaming>" << endl;
    while ( !PvKbHit() )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        // Retrieve next buffer
        PvResult lResult = aPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...
                lFrameRate->GetValue( lFrameRateVal );
                lBandwidth->GetValue( lBandwidthVal );


                if (lBuffer->GetPayloadType() == PvPayloadTypeImage)
                {
                    cout << "W: " << dec << lBuffer->GetImage()->GetWidth() << "H: " << lBuffer->GetImage()->GetHeight();
                    cout << "  " << lFrameRateVal << " FPS  " << ( lBandwidthVal / 1000000.0 ) << " Mb/s   \r";
                    cv::Mat openCvImage = cv::Mat(lBuffer->GetImage()->GetHeight(), lBuffer->GetImage()->GetWidth(), CV_8U, lBuffer->GetDataPointer());
                    cv::Mat colorImg;
                    cvtColor(openCvImage, colorImg, cv::COLOR_BayerRG2RGB);
                    imshow("Img", colorImg);
                    cv::waitKey(1);
                } 
                else
                {
                    cout<< "Incorrect payload type. Start stream with PvPayloadTypeImage. Current type is " << lBuffer->GetPayloadType();
                }
            }
            else
            {
                // Non OK operational result
                cout << lOperationResult.GetCodeString().GetAscii() << "\r";
            }

            // Release the buffer back to the pipeline
            aPipeline->ReleaseBuffer( lBuffer );
        }
        else
        {
            // Retrieve buffer failure
            cout << lResult.GetCodeString().GetAscii() << "\r";
        }
    }

    PvGetChar(); // Flush key buffer for next stop.
    cout << endl << endl;

    // Tell the device to stop sending images.
    cout << "Sending AcquisitionStop command to the device" << endl;
    lStop->Execute();

    // Disable streaming on the device
    cout << "Disable streaming on the controller." << endl;
    aDevice->StreamDisable();

    // Stop the pipeline
    cout << "Stop pipeline" << endl;
    aPipeline->Stop();
}
