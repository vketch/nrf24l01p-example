#include "mbed.h"
#include "nRF24L01P.h"

#define TRANSFER_SIZE   1
nRF24L01P my_nrf24l01p(PA_7, PA_6, PA_5, PD_14, D8, D7);
//static BufferedSerial bs(USBTX, USBRX);

// Blinking rate in milliseconds
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
#define BLINKING_RATE_MS     100ms


void readOnPipe(int pipe) 
{
    int rxDataCnt = 0;    
    char rxData[TRANSFER_SIZE];    
    if ( my_nrf24l01p.readable(pipe) ) { 
        rxDataCnt = my_nrf24l01p.read( pipe, rxData, sizeof( rxData ) );                
        if(rxDataCnt){
            printf("pipe %d received:%d\n", pipe, rxData[0]);
            if( pipe == NRF24L01P_PIPE_P0 )
                myled1 = !myled1;
            else if( pipe == NRF24L01P_PIPE_P1 )
                myled2 = !myled2;
            else if ( pipe == NRF24L01P_PIPE_P2 )
                myled3 = !myled3;
       }                
    }
}


int main() {
    
    char count[1];
    int RxDataCnt = 1;
    char temp;
    char pot1Val, pot2Val;
    
    //specifying address same as transmitter for pipe0 and pipe1
    long long RxAddress_PIPE0 = 0xC2C2C2C2C2;    
    long long RxAddress_PIPE1 = 0xE2E2E2E2E2;
    long long RxAddress_PIPE2 = 0xE2E2E2E2D3;    
    
    my_nrf24l01p.powerUp();
    my_nrf24l01p.setRfFrequency(2410);
    
    //set rx address with default address and for specified pipe
    my_nrf24l01p.setRxAddress(RxAddress_PIPE0, DEFAULT_NRF24L01P_ADDRESS_WIDTH, NRF24L01P_PIPE_P0);    
    my_nrf24l01p.setRxAddress(RxAddress_PIPE1, DEFAULT_NRF24L01P_ADDRESS_WIDTH, NRF24L01P_PIPE_P1);
    my_nrf24l01p.setRxAddress(RxAddress_PIPE2, DEFAULT_NRF24L01P_ADDRESS_WIDTH, NRF24L01P_PIPE_P2);    
    
 
    // Display the (default) setup of the nRF24L01+ chip
    printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    
    //display rx address for both pipes
    printf( "nRF24L01+ RX Address - PIPE0  : 0x%010llX\r\n", my_nrf24l01p.getRxAddress(NRF24L01P_PIPE_P0) );
    printf( "nRF24L01+ RX Address - PIPE1  : 0x%010llX\r\n", my_nrf24l01p.getRxAddress(NRF24L01P_PIPE_P1) );
    printf( "nRF24L01+ RX Address - PIPE2  : 0x%010llX\r\n", my_nrf24l01p.getRxAddress(NRF24L01P_PIPE_P2) );
 
    printf( "Wireless Sensor Network - Multiceiver\r\n" );
    
    
    //set transfer size explicitly for both pipes
    my_nrf24l01p.setTransferSize(RxDataCnt, NRF24L01P_PIPE_P2);
    my_nrf24l01p.setTransferSize(RxDataCnt, NRF24L01P_PIPE_P1);
    my_nrf24l01p.setTransferSize(RxDataCnt, NRF24L01P_PIPE_P0);
    
    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();
 
    while (1) {
        
        readOnPipe(NRF24L01P_PIPE_P0);
        readOnPipe(NRF24L01P_PIPE_P1);
        readOnPipe(NRF24L01P_PIPE_P2);        

        ThisThread::sleep_for(BLINKING_RATE_MS);
    }
}