


# ![PALMS LOGO](ico.ico) PALMS

PALMS is able to integrate Petri net modelling tools with pnml export feature
with PNRD/iPNRD. In this direction, PALMS can be connected to many Arduino than 
to the available number of USB ports. Each Arduino Uno is able to
be integrated to one PN532 RFID reader while each Arduino Mega can hold up
to three readers.
As internal structure, PALMS has two modes, it means, Setup and Runtime.

## SETUP MODE

In the Setup mode, PALMS manages its connections. As ethernet connect re-
quires a huge change of the Arduino Library, it will be implemented further.
Another feature of this mode is pnml convertion and the generation of a setup
json file with pnml and PNRD/iPNRD relationship. PNRD/iPNRD initial
marking and Arduino programming is assisted manually through PALMS this
module results.

## RUNTIME MODE
In the Runtume mode PALMS can receive asynchronously several PNRD/iPNRD
next state calculus messages containing tag id, reader id, new marking vector,
transitioning id, exception infor and timestamp. Based on these informations,
marking vector is updated as well as a runtime history json. If an exception
is identified , PALMS shows it in its visual interface. Pnml is updated in
order to visualize the whoe process through any Petri net modelling tool which
is able to read this format. PALMS follows pnml format. PALMS doesn't
deal with exception treatment.

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

# License information
PALMS is licensed under The MIT License (MIT). Which means that you can use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software. But you always need to state that MAPL-UFU is the original author of this software.

Project is developed and maintained by Roger Carrijo and MAPL-UFU Team


