/*
* UPDATE FOR ARDUINO MEGA 
* AND THREE NFC READERS
*
*/
        #include <Pn532NfcReader.h>
        #include <PN532_HSU.h>
//        #include <PN532.h>
        #include <SPI.h>
//        #include <PN532_SPI.h>
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
                               
        uint32_t tagId1 = 0xFF;
        uint32_t tagId2 = 0xFF;
        uint32_t tagId3 = 0xFF;

        uint16_t * tokenVector;             

 //       uint8_t n_places = 7; //#########################      
        uint8_t n_places; //#########################      
 //       uint8_t n_transitions = 8; //#######################              
        uint8_t n_transitions; //#######################              

        bool tagReadyToContinue = false;                
        String stringToken;             
                        
        String readerId = "Oi";
        PN532_HSU pn532hsu1(Serial1);
        NfcAdapter nfc1= NfcAdapter(pn532hsu1);
        Pn532NfcReader* reader1 = new Pn532NfcReader(&nfc1); 

        PN532_HSU pn532hsu2(Serial2);
        NfcAdapter nfc2= NfcAdapter(pn532hsu2);
        Pn532NfcReader* reader2 = new Pn532NfcReader(&nfc2); 

        PN532_HSU pn532hsu3(Serial3);
        NfcAdapter nfc3= NfcAdapter(pn532hsu3);
        Pn532NfcReader* reader3 = new Pn532NfcReader(&nfc3); 

        void serial_com(                
            uint32_t tagId,               
            String readerId,                
            int antena,              
            char ErrorType[100],     
            String tokenVector,         
            int fireVector            
            ){             
          Serial.println(String("I") + String(tagId,HEX) + String("-A") + String(1) + String("-R") + String(readerId) + String("-P")+ String(ErrorType)+ String("-T[") + String(tokenVector)+ String("]")+ String("-F")+ String(fireVector)+String("-EE"));               
        }               
        
        void(* resetFunc) (void) = 0; //reset function at address 0
 
        unsigned long timestamp; //for multithread between the runtime and the ftp server 

        void setup() 
        {            
            //Initialization of the communication with the reader and with the computer Serial bus                
            Serial.begin(115200); //Changed from 9600 to 115200 to deal with the files and the server    

            timestamp = millis();
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
            myFile = FAT_FS.open("pnrdInfo.txt");
            int fileLineIndex = 0;
            
            if(myFile)
            {
                Serial.println("Read:");
                // Reading the whole file
                while(myFile.available())
                {
//                    Serial.println("WHYYY");
                    String list = myFile.readStringUntil('\n');
//                    Serial.println("dont know");
//            Serial.println(list);

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
            
            n_places = lines;
            n_transitions = columns;
//##########################################################################
            //Pnrd pnrd1 = Pnrd(reader1,n_places,n_transitions,false,false,false);
            Pnrd pnrd1 = Pnrd(reader1,lines,columns,false,false,false);

            reader1->initialize();
            pnrd1.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
            pnrd1.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);


            //Pnrd pnrd2 = Pnrd(reader2,n_places,n_transitions,false,false,false);
            Pnrd pnrd2 = Pnrd(reader2,lines,columns,false,false,false);

            reader2->initialize();
            pnrd2.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
            pnrd2.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);


            //Pnrd pnrd3 = Pnrd(reader3,n_places,n_transitions,false,false,false);
            Pnrd pnrd3 = Pnrd(reader3,lines,columns,false,false,false);

            reader3->initialize();
            pnrd3.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
            pnrd3.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);

            while(true)
            {
//############################## FTP SERVER ################################
                ftpSrv.service();

                if(ftpSrv.client.connected()) clientDisconnected = false;

//                if(!ftpSrv.client.connected() && clientDisconnected)
                if(millis() - timestamp > 500)
                {
                    Serial.print("IM STILL RUNNING ");
                    Serial.print(timestamp);
//                    timestamp = millis();
//##########################################################################
//                    delay(50);  

                    ReadError readError1 = pnrd1.getData();
//                    delay(50);                                      
                    if(readError1 == ReadError::NO_ERROR){   
                        FireError fireError1;                            
                        if(tagId1 != pnrd1.getTagId()){             
                            tagId1 = pnrd1.getTagId();                
                            FireError fireError1 = pnrd1.fire(1);                          
                            pnrd1.getTokenVector(tokenVector);                
                            stringToken = " ";                
                            for (int32_t place = 0; place < n_places; place++) {             
                                if ((n_places -1) == place){            
                                    stringToken += String(tokenVector[place]);              
                                }             
                                else{          
                                    stringToken += String(tokenVector[place]);              
                                    stringToken += ",";             
                                }
                            }
                            //------------------------------------                
                            switch (fireError1){
                                case FireError::NO_ERROR :                     
                                    if(pnrd1.saveData() == WriteError::NO_ERROR){              
                                        serial_com(tagId1,readerId,1,"NO_ERROR",stringToken,1);             
                                        Serial.println("WANNA SEE SMTH");
                                        return;               
                                    }else{             
                                        serial_com(tagId1,readerId,1,"ERROR_TAG_UPDATE",stringToken,1);             
                                        Serial.println("WANNA SEE SMTH1");
                                        return;               
                                    }               

                                case FireError::PRODUCE_EXCEPTION :               
                                    serial_com(tagId1,readerId,1,"PRODUCE_EXCEPTION",stringToken,1);                
                                        Serial.println("WANNA SEE SMTH2");
                                return;               

                                case FireError::CONDITIONS_ARE_NOT_APPLIED :              
                                    serial_com(tagId1,readerId,1,"CONDITIONS_ARE_NOT_APPLIED",stringToken,1);               
                                        Serial.println("WANNA SEE SMTH3");
                                break;                

                                case FireError::NOT_KNOWN:                
                                    serial_com(tagId1,readerId,1,"NOT_KNOWN",stringToken,1);              
                                        Serial.println("WANNA SEE SMTH4");
                                break;                
                            }                 
                        }             
                    }
                    ReadError readError2 = pnrd2.getData();
//                     delay(50);                                      
                    if(readError2 == ReadError::NO_ERROR){   
                        FireError fireError2;                            
                        if(tagId2 != pnrd2.getTagId()){             
                            tagId2 = pnrd2.getTagId();                
                            FireError fireError2 = pnrd2.fire(2);                          
                            pnrd2.getTokenVector(tokenVector);                
                            stringToken = " ";                
                            for (int32_t place = 0; place < n_places; place++) {             
                                if ((n_places -1) == place){            
                                    stringToken += String(tokenVector[place]);              
                                }             
                                else{          
                                    stringToken += String(tokenVector[place]);              
                                    stringToken += ",";             
                                }
                            }
                            //------------------------------------                
                            switch (fireError2){
                                case FireError::NO_ERROR :                     
                                    if(pnrd2.saveData() == WriteError::NO_ERROR){              
                                        serial_com(tagId2,readerId,2,"NO_ERROR",stringToken,2);             
                                        return;               
                                    }else{             
                                        serial_com(tagId2,readerId,2,"ERROR_TAG_UPDATE",stringToken,2);             
                                        return;               
                                    }               

                                case FireError::PRODUCE_EXCEPTION :               
                                    serial_com(tagId2,readerId,2,"PRODUCE_EXCEPTION",stringToken,2);                
                                return;               

                                case FireError::CONDITIONS_ARE_NOT_APPLIED :              
                                    serial_com(tagId2,readerId,2,"CONDITIONS_ARE_NOT_APPLIED",stringToken,2);               
                                break;                

                                case FireError::NOT_KNOWN:                
                                    serial_com(tagId2,readerId,2,"NOT_KNOWN",stringToken,2);              
                                break;                
                            }                 
                        }             
                    }
                    ReadError readError3 = pnrd3.getData();
//                    delay(50);                                      
                    if(readError3 == ReadError::NO_ERROR){   
                        FireError fireError3;                            
                        if(tagId3 != pnrd3.getTagId()){             
                            tagId3 = pnrd3.getTagId();                
                            FireError fireError3 = pnrd3.fire(3);                          
                            pnrd3.getTokenVector(tokenVector);                
                            stringToken = " ";                
                            for (int32_t place = 0; place < n_places; place++) {             
                                if ((n_places -1) == place){            
                                    stringToken += String(tokenVector[place]);              
                                }             
                                else{          
                                    stringToken += String(tokenVector[place]);              
                                    stringToken += ",";             
                                }
                            }
                            //------------------------------------                
                            switch (fireError3){
                                case FireError::NO_ERROR :                     
                                    if(pnrd3.saveData() == WriteError::NO_ERROR){              
                                        serial_com(tagId3,readerId,3,"NO_ERROR",stringToken,3);             
                                        return;               
                                    }else{             
                                        serial_com(tagId3,readerId,3,"ERROR_TAG_UPDATE",stringToken,3);             
                                        return;               
                                    }               

                                case FireError::PRODUCE_EXCEPTION :               
                                    serial_com(tagId3,readerId,3,"PRODUCE_EXCEPTION",stringToken,3);                
                                return;               

                                case FireError::CONDITIONS_ARE_NOT_APPLIED :              
                                    serial_com(tagId3,readerId,3,"CONDITIONS_ARE_NOT_APPLIED",stringToken,3);               
                                break;                

                                case FireError::NOT_KNOWN:                
                                    serial_com(tagId3,readerId,3,"NOT_KNOWN",stringToken,3);              
                                break;                
                            }                 
                        }             
                    }
                    //Serial.flush();
                    timestamp = millis();
                    Serial.print("       ");
                    Serial.println(timestamp);
//################################# FTP SERVER ############################# 
                }
                if(!ftpSrv.client.connected() && !clientDisconnected)
                {
                    clientDisconnected = true;
                    myFile.close();
                    resetFunc();
                }
//##########################################################################
            }
        }
                       
        void loop() {}