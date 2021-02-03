# ![PALMS LOGO](ico.ico) PALMS

PALMS is a control software able to integrate Petri net modelling tools (through the use of pnml exported file) with PNRD/iPNRD. In this version of PALMS, it can be connected to as many Arduinos as available in different IPs in a local net. Each Arduino Uno is able to be integrated with up to three PN532 RFID readers. As internal structure, PALMS has two modes, it means, Setup and Runtime.

## SETUP MODE

In the Setup mode, PALMS manages its FTP connections, and receive the IPs of the arduinos connected. Another feature of this mode is pnml convertion and the generation of a 'setup.palms' file as intermediary of PNML and PNRD/iPNRD relationship. Arduino's standard files are stored in startupArduinoFiles directory. Attention: There are two distincts Arduino's files in startupArduinoFiles directory, one for tag initial marking (pnrd_iniTag.ino) and another for reader setup (pnrd_reader), and both must be installed by Arduino IDE manually in advance. PALMS create automaticaly '.pnrd' files with PNRD data structure and this file allows PALMS update PNRD information in "real-time". Future version of PALMS will use MQTT protocol.

## RUNTIME MODE
In the Runtume mode PALMS transfer the Petri Net information to the connected arduinos via FTP, and receive the data generated from the readers with next state calculus files containing tag id, reader id, new marking vector, transitioning id, exception infor and timestamp. Based on these informations, marking vector is updated as well as a runtime history json. If an exception is identified, PALMS shows it in its visual interface. Pnml is updated in order to visualize the whole process through any Petri net modelling tool which is able to read this format. PALMS follows pnml format. PALMS does not deal with exception treatment.

## USING PALMS

1. Install Python3
2. Install PIP
3. Install pip requirements
    ```python
    pip install -r requirements.txt
    ```
4. Execute script
    ```python
    python main.py
    ```

## COMPILING EXECUTABLE ON WINDOWS
Although the method above is preferable and also applicable for windows OS, a second way to run the software is building the executable .exe:

1. Install Pyinstaller
    ```python
    pip install pyinstaller
    ```
2. Execute script
    ```python
    pyinstaller --onefile -w main.py
    ```
The execuble built is found in the dist directory.

# License information
PALMS is licensed under The MIT License (MIT). Which means that you can use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software. But you always need to state that MAPL-UFU is the original author of this software.

This project was started by Roger Carrijo, is developed and maintained by Gabriel de Brito Silva and the MAPL-UFU Team.
