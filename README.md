# The Palletizer!

It's a project that simulates a factory paletizer in a Factory IO scene. 
Develloped for the course of Real time Operating Systems @ Polytech Montpellier.
We use FreeRTOS, Percepio Tracealyzer and Factory IO as tools.

## How to run

### Dependencies
To run it, you gotta have [Factory I/O](https://factoryio.com/) running in [2.4.5](https://realgames.b-cdn.net/fio/factoryio-installer-latest.exe).

You will also need [Python 3](https://www.python.org/downloads) installed for the bridge script component allowing to comunicate with the STM board and Factory I/O.

For the board, we use the STM32F072 model, in a [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) environment.

It's also recommended to have [Percepio's Tracealyzer](https://percepio.com/tracealyzer/) installed to analyse task timing. Even though is not necessary for the project to run, running without it will demand you to comment/remove each tracealyzer line in the code.

### Steps
It's good to know that this project wasn't run on Linux yet, so the next step by step guide shows how you should configurate in Windows.

1. In Windows, open `Device Manager` and check the port your STM is connected (if it's not, connect it!)
2. Copy the port name (e.g.: `COM7`), we are using it just after.
3. Open your python bridge:
```bash
cd bridge
```
4. Edit the last line with the code you found in your peripheral manager:
```python
    EngineIO_Controller(port='COM6', baudrate=115200, timeout=.5, rate=0.016,  verbose=True)
```
5. Save it and run it.
```bash
python run.py
```

6. Download the [palettiseur scene](https://gitlab.polytech.umontpellier.fr/tp_ostr_factoryio/factoryio-scene-palettiseur) and run it in Factory I/O
7. Run the debugger in STM32CubeIDE to program your board, and run the code.
8. You should see the scene rolling in Factory I/O.



