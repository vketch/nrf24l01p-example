#include "mbed.h"
#include "nRF24L01P.h"
#include <cstdint>
#include <stdint.h>

//#define MASTER
#define SLAVE
#define SLAVE_ID NRF24L01P_PIPE_P0

#if defined(MASTER) && defined(SLAVE)
    #error("Ony MASTER or SLAVE node has to be diefined"); 
#endif

#define TRANSFER_SIZE   4
#define PIPE_NUMBER   3
// The counting of pipes starts from 0 so lets use array index as pipe number 
unsigned long long pipe_addresses[PIPE_NUMBER] = {0xC2C2C2C2C2, 0xE2E2E2E2E2, 0xE2E2E2E2D3};

//nRF24L01P my_nrf24l01p(PA_7, PA_6, PA_5, PD_14, D8, D7);   // SPI1 
nRF24L01P my_nrf24l01p(D22/*PB_5*/, D25/*PB_4*/, D23/*PB_3*/, D10/*PD_14*/, D8/*PF12*/, D7/*PF13*/); // SPI3


// Blinking rate in milliseconds
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
#define BLINKING_RATE_MS     500ms


#if defined(MASTER)
int readOnPipe(int pipe) 
{
    int rxDataCnt = 0;    
    char rxData[TRANSFER_SIZE];    
    if ( my_nrf24l01p.readable(pipe) ) { 
        rxDataCnt = my_nrf24l01p.read( pipe, rxData, sizeof( rxData ) );                
        if(rxDataCnt > 0){
            printf("pipe %d received %d bytes: ", pipe,  rxDataCnt); 
            printf ("0x%02x", rxData[0] );
            for( uint8_t byte = 1; byte < rxDataCnt; byte++ )
                printf (",0x%02x", rxData[byte] );
            printf("\n");
        }        
    }    
    return rxDataCnt;
    //printf ("No data on pipe %d", pipe );    
}

bool init_master()
{
    // Display the (default) setup of the nRF24L01+ chip
    printf( "nRF24L01+ Frequency    : %d MHz\n",  my_nrf24l01p.getRfFrequency() );
    printf( "nRF24L01+ Output power : %d dBm\n",  my_nrf24l01p.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\n", my_nrf24l01p.getAirDataRate() );

    for(uint8_t pipe = 0; pipe<PIPE_NUMBER; pipe++ ){ 
        my_nrf24l01p.setRxAddress(pipe_addresses[pipe], DEFAULT_NRF24L01P_ADDRESS_WIDTH, pipe);     
        printf( "nRF24L01+ RX Address - PIPE%u  : 0x%010llX\r\n", pipe, my_nrf24l01p.getRxAddress(pipe) );
        my_nrf24l01p.setTransferSize(TRANSFER_SIZE, pipe); 
    } 
    printf( "Master node started.\n" );
    
    my_nrf24l01p.setReceiveMode();    

    return true;
}

#else // SLAVE
void TransmitData() 
{
    char txData[TRANSFER_SIZE] = {SLAVE_ID+5,SLAVE_ID+4, SLAVE_ID+3, SLAVE_ID+2}; // just sore pipe id increased by 5
    my_nrf24l01p.write( SLAVE_ID, txData, TRANSFER_SIZE );    

    printf("Slave node %d transmited %d bytes: ", SLAVE_ID,  TRANSFER_SIZE); 
    printf ("0x%02x", txData[0] );
    for( uint8_t byte = 1; byte < TRANSFER_SIZE; byte++ )
        printf (",0x%02x", txData[byte] );
    printf("\n");
    
}

bool init_slave(uint8_t pipe)
{
    my_nrf24l01p.setTxAddress(pipe_addresses[pipe]);    
    my_nrf24l01p.setRxAddress(pipe_addresses[pipe], DEFAULT_NRF24L01P_ADDRESS_WIDTH, pipe);
    my_nrf24l01p.setTransferSize(TRANSFER_SIZE, pipe);
    
    // Display the (default) setup of the nRF24L01+ chip
    printf( "nRF24L01+ Frequency    : %d MHz\n",  my_nrf24l01p.getRfFrequency() );
    printf( "nRF24L01+ Output power : %d dBm\n",  my_nrf24l01p.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\n", my_nrf24l01p.getAirDataRate() );
    printf( "nRF24L01+ TX Address - PIPE1   : 0x%010llX\n", my_nrf24l01p.getTxAddress() );
    
    printf( "Slave node on pipe: %d started \n", pipe);
            
    return true;
}

#endif


int main() {
        
    my_nrf24l01p.powerUp();
    my_nrf24l01p.setRfFrequency(2402);

#if defined(MASTER)
    init_master();
#else 
    init_slave( SLAVE_ID );
#endif    

    my_nrf24l01p.enable();
 
    while (1) {
#if defined(MASTER) 
        for(uint8_t pipe = 0; pipe<PIPE_NUMBER; pipe ++ ){ 
            
            int read_bytes = readOnPipe( pipe );
            if( read_bytes ) // some data is received
            {
                // lets blink by the LED depends on which node data is received 
                if( pipe == NRF24L01P_PIPE_P0 )
                    myled1 = !myled1;
                else if( pipe == NRF24L01P_PIPE_P1 )
                    myled2 = !myled2;
                else if ( pipe == NRF24L01P_PIPE_P2 )
                    myled3 = !myled3;
            }
        }
        ThisThread::sleep_for(BLINKING_RATE_MS/5);        
#else   // SLAVE     
        TransmitData();
        ThisThread::sleep_for(BLINKING_RATE_MS);
#endif
    }
}