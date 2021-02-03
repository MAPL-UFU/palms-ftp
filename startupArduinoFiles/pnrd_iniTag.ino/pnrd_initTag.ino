
/*
* UPDATE FOR ARDUINO MEGA
* 
*
*/
        #include <Pn532NfcReader.h>                                     
        #include <PN532_HSU.h>
        #include <SPI.h>                                                
//        #include <PN532_SPI.h>                                          
        #include <PN532.h>                                              
//        #include <NfcAdapter.h>                                         
                                                                        
 //############################### FTP SERVER ###################################
        #include <FtpServer.h>
        #include <FreeStack.h>

        #define CS_SDCARD 4
        #define P_RESET 8

        FtpServer ftpSrv;

        byte mac[] = { 0x00, 0xaa, 0xbb, 0xcc, 0xde, 0xef };

        IPAddress serverIp( 0, 0, 0, 0 );

        IPAddress externalIP( 192, 168, 1, 2);
//##############################################################################

//############################## FILES SYSTEM ##################################
        File myFile;
        int lines, columns;

        int8_t mIncidenceMatrix[100];
        uint16_t mStartingTokenVector[10];
//##############################################################################

//        int8_t mIncidenceMatrix[] = {-1,0,0,0,1,-1,0,0,1,-1,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,1,-1,0,0,1,-1,-1,0,0,1,0,0,0,1,0,0,0,0};              
//        uint16_t mStartingTokenVector[] = {1,0,0,0,0,0,0};    
                                                                        
        int incomingByte = 0;                                           
                                                                        
        PN532_HSU pn532hsu(Serial1);
        NfcAdapter nfc = NfcAdapter(pn532hsu);
                                                                        
        Pn532NfcReader* reader = new Pn532NfcReader(&nfc);              
        Pnrd pnrd = Pnrd(reader,7,8, false, false, false);            
                                                                        
        unsigned long timestamp;

        void setup() 
        {                                                 
            timestamp = millis();
//Initialization of the communication with the reader and with the computer Serial bus
            Serial.begin(115200);                                             

//################################### FTP SERVER ###############################
            Serial << F( "=== Test o FTP Server ===" ) << eol;

            // Initialize SD
            Serial << F("Mount the SD card with library ");
            #if FAT_USE == FAT_SDFAT
              Serial << F("SdFat ... ");
            #else
              Serial << F("FatFs ... ");
            #endif
            if( ! FAT_FS.begin( CS_SDCARD, SD_SCK_MHZ( 50 )))
            {
              Serial << F("Unable to mount SD card") << eol;
              while( true );
            }
            pinMode( CS_SDCARD, OUTPUT );
            digitalWrite( CS_SDCARD, HIGH);
            Serial << F("ok") << eol;

            // Send reset to Ethernet module
            if( P_RESET > -1 )
            {
              pinMode( P_RESET, OUTPUT );
              digitalWrite( P_RESET, LOW);
              delay( 200 );
              digitalWrite(P_RESET, HIGH);
              delay(200);
            }

            // Initialize network
            Serial << F("Initialize ethernet module ...");
            if((uint32_t) serverIp != 0)
              Ethernet.begin(mac, serverIp);
            else if(Ethernet.begin(mac) == 0)
            {
              Serial << F("failed!") << eol;
              while(true);
            }
            uint16_t wizModule[] = { 0, 5100, 5200, 5500 };
            Serial << F("W") << wizModule[Ethernet.hardwareStatus()] << F(" ok") << eol;
            Serial << F("ÏP address of server: ") << Ethernet.localIP() << eol;

            // Initialize the FTP server
            if((uint32_t) externalIP != 0)
              ftpSrv.init(externalIP);
            else
              ftpSrv.init();
            ftpSrv.credentials("myname", "123");

            Serial << F("Free stack: ") << FreeStack() << eol;

            bool clientDisconnected = true;
//##############################################################################

//################################## FILES SYSTEM ##############################           
            while(true)
            {
                myFile = FAT_FS.open("pnrdInfo.txt");
                int fileLineIndex = 0;
                
                if(myFile)
                {
                    Serial.println("Read:");
                    // Reading the whole file
                    while(myFile.available())
                    {
//                        Serial.println("WHYYY");
                        String list = myFile.readStringUntil('\n');
//                        Serial.println("dont know");
//                Serial.println(list);

                        char buff[list.length()];
                        int buffInt[list.length()];
                        unsigned int data_num = 0;
                        list.toCharArray(buff,list.length());
                        while(list.indexOf(" ")!=-1)
                        {
                            //take the substring from the start to the first occurence of a comma, convert it to int and save it in the array
                            buffInt[ data_num ] = list.substring(0,list.indexOf(" ")).toInt();
                            Serial.print(buffInt[data_num]);

                            data_num++; // increment our data counter
                            //cut the data string after the first occurence of a comma
                            list = list.substring(list.indexOf(" ")+1);

                            Serial.print(" ");
                        }
                       // get the last value out of the string, which as no more spaces in it.
                       buffInt[ data_num ] = list.toInt();
                       Serial.print(buffInt[data_num]);

                       Serial.println();
                       switch(fileLineIndex)
                       {
                           case 0:
                            lines = buffInt[data_num];
                           break;

                           case 1:
                            columns = buffInt[data_num];
                           break;

                           case 2:
                            memcpy(mIncidenceMatrix,buffInt,size_t(buffInt));
                           break;

                           case 3:
                            memcpy(mStartingTokenVector,buffInt,size_t(buffInt));
                           break;

                           default:
                           break;
                       } 
                    }
                    myFile.close();
                }
                else
                {
                    Serial.println("Error opening pnrdInfo.txt");
                }
                
//##############################################################################
 
                reader->initialize();                                           
                                                                        
                // pnrd.setGoalToken(tokenDesejado);                            
                pnrd.setIncidenceMatrix(mIncidenceMatrix);                      
                pnrd.setTokenVector(mStartingTokenVector);                      
                                                                        
                // pnrd.setAsTaygInformation(PetriNetInformation::GOAL_TOKEN);   
                pnrd.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);    
                pnrd.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);  

                Serial.print("\n\nInitial recording of PNRD tags.");          
                                                                        
                while(true)
                {
//################################## FTP SERVER ################################
                    ftpSrv.service();

                    if(ftpSrv.client.connected()) clientDisconnected = false;

                    if(millis()-timestamp > 50)
                    {
//##############################################################################

                        if (Serial.available() > 0)
                        {                                  
                            incomingByte = Serial.read();                               
                                                                        
                            Serial.print("I received: ");                               
                            Serial.println(incomingByte, DEC);                          
                        }                                                                                     
//                        delay(1000);                                                    
                        Serial.println("Place a tag near the reader.");                 

                        if(pnrd.saveData() == WriteError::NO_ERROR)
                        {                   
                            Serial.println("Tag configurated successfully.");       

                        };                                                             
                                                                        
                        Serial.print("\n\n");                                         
//                        delay(1000);                                                    
                        timestamp = millis();
 //##################################### FTP SERVER ############################# 
                    }
                    if(!ftpSrv.client.connected() && !clientDisconnected)
                    {
                        clientDisconnected = true;
                        myFile.close();
                        break;
                    }
//##############################################################################
                }                                                              
            }
        }
                                                                        
        void loop() {}                                                              
        