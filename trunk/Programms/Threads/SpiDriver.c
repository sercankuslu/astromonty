
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "SpiDriver.h"

static SPI_DEVICE_LIST SPI_Devices[SPI_DEV_COUNT];
static BYTE DeviceBusy[SPI_PORT_COUNT];
static BYTE CurDevID[SPI_PORT_COUNT];


static WORD GetPrescale(DWORD Speed)
{
    double Div;
    WORD IntDiv;
    WORD T = 1;
    BYTE i = 0;
    BYTE PPRE = 0;
    BYTE SPRE = 0;
    BYTE Shift=0;    
    WORD SPIXCON1=0;
    
    Div=GetPeripheralClock()/((double)Speed);
    IntDiv=(WORD)Div+1;
    while(T>0){
        T = IntDiv>>Shift;
        if((T>=1)&&(T<=8)) {
            PPRE = i;
            SPRE = (~((BYTE)T-1))&0x07;
            break;
        }  
        Shift += 2; 
        i++;                  
    }    
    SPIXCON1 = (SPRE<<2)|(PPRE);

    return SPIXCON1;
}

BYTE SPI_Init(void)
{
    WORD i;	
    WORD SPIXCON1=0;
	for(i=0;i<SPI_DEV_COUNT;i++){		
		SPI_Devices[i].DevID=i;
		SPI_Devices[i].Port = 0;
		SPI_Devices[i].Status = 0;
		SPI_Devices[i].DevSelect = 0;
		SPI_Devices[i].DevDeselect = 0;
	}	
	for(i=0;i<SPI_PORT_COUNT;i++){
    	DeviceBusy[i]=0;
    	CurDevID[i]=0;
    }   
    return 0;
}

static void Call_Dev(void* Dev)
{
    asm volatile (              
        "call %0 \n": :"r"(Dev)
    ); 
}




/*
–егистраци€ нового устройства на шине SPI
возвращает DevId не требует захвата устройства
ѕараметры:
port   - номер порта SPI
DevOn  - адрес функции дл€ выбора устройства
DevOff - адрес функции дл€ освобождени€ устройства
*/
BYTE SPI_RegDevice(BYTE port, BYTE Mode, DWORD Speed, void* DevOn,void* DevOff)
{   
    //ищем свободный элемент массива 
    BYTE i;    
    BYTE j; 
    if((DevOn==0)||(DevOff==0)) return 2;//неправильные функции выбора устройства
    for(i=0;i<SPI_DEV_COUNT;i++){
        if(SPI_Devices[i].Status==0){
            SPI_Devices[i].Port=port;
            SPI_Devices[i].Status = SPI_DEV_ACTIVE;
            SPI_Devices[i].DevSelect = DevOn;
		    SPI_Devices[i].DevDeselect = DevOff;  
		    SPI_Devices[i].SPIParams = (GetPrescale(Speed)&0x001F) ;
		    Call_Dev(SPI_Devices[i].DevDeselect);
		    if(Mode&0x01){
    		    SPI_Devices[i].SPIParams|=SPI_CLK_POLAR_H;    		    
		    }
		    if(Mode&0x02){
    		    SPI_Devices[i].SPIParams|=SPI_CLK_EDGE_HL;
		    }		    
            return 0; //завершено успешно
        }         
    }
    return 1; // нет свободных сокетов дл€ устройства
}

/*
Ќачало работы с устройством
*/
BOOL SPI_Open(BYTE DevId)
{
    BYTE Port;
    if(SPI_Devices[DevId].Status&SPI_DEV_ACTIVE!=1)
        return 0;    
    Port = SPI_Devices[DevId].Port;
    //ждем пока освободитс€ устройство
    if(CurDevID[Port]!=DevId){
        while(DeviceBusy[Port]){
            Nop();       
        }
    }    
    //Ѕлокируем шину
    DeviceBusy[Port]=1;
    CurDevID[Port]=DevId;
    SPI_Devices[DevId].Status |= SPI_DEV_CURRENT;
    //устанавливаем параметры
    
    switch(Port){
        case 1:
            SPI1CON1=SPI_Devices[DevId].SPIParams;
            Call_Dev(SPI_Devices[DevId].DevSelect);
            SPI1CON2 = 0;
            SPI1CON1bits.CKE = 1;
            SPI1CON1bits.MSTEN = 1;
            SPI1STATbits.SPIEN = 1;
        break;        
        case 2:
            SPI2CON1=SPI_Devices[DevId].SPIParams;
            Call_Dev(SPI_Devices[DevId].DevSelect);
            SPI2CON2 = 0;
            SPI2CON1bits.CKE = 1;
            SPI2CON1bits.MSTEN = 1;
            SPI2STATbits.SPIEN = 1;
        break;
        default:
            return 0; //неправильный номер порта
    }
    
}
