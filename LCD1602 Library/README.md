# Library to Interface LCD1602 with STM32F407 DISC Board
This library uses STM HAL layer to interface the LCD1602 (16x2 LCD character display) with the STM32F407 Discovery Board. To use this library include the lcd1602.h & lcd1602.c in your project and use the library functions as shown in the main.c source file.

# Library Overview
The library aims at abstracting all the lower level details required to drive the LCD and provide top level functions to initialize and run the LCD from the application code. The library contains a base structure: **struct lcd1602_BaseTemplate** which aims at holding all the required information for a given LCD module connected to the micro-controller. Multiple instances of the **lcd1602_BaseTemplate** can be created to represent multiple LCD hardware modules connected to the board. 

An instance of the **lcd1602_BaseTemplate** contains the follwoing information:
1. LCD1602 configuration data using the struct **lcd1602_ConfigurationData** 
2. LCD1602 pin connection details using the struct **lcd1602_PinDefinition**
3. A number of interfaces which can be used by the instance to configure, initialize and use the LCD module

To use the library correctly, follow the following steps in sequence:
1. Include lcd1602.h and lcd1602.c files in your project
2. Create an instance of type **lcd1602_BaseTemplate** , **lcd1602_ConfigurationData** and **lcd1602_PinDefinition**
3. Initialize all the parameters of the various instances and then call **LCD1602_Init**. This is very important as it ensures that all the function pointers in the Base Template struct are initialized correctly.
4. Post this, call ***lcd_begin*** to execute the initialization sequence of the LCD module. This is important to ensure proper functioning of the LCD module

Now, the LCD is ready to use. You can use the ***print*** function to print any string on the LCD module. You can refer to the main.c file to get a better understanding of the above mentioned steps

# Hardware Implementation
The microcontroller used during the development of this library is STM32F407 DISC Board. However, the library can be ported to other platforms with minimal modifications

A pre-soldered LCD1602 module can be found here : [LCD1602 Module](https://www.roboelements.com/product/16x2-lcd-display-arduino-presoldered-male-header-pins/)

# External References
1. [LCD Initialization Sequence](http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html)
2. [LCD Controller Datasheet](https://www.sparkfun.com/datasheets/LCD/ADM1602K-NSA-FBS-3.3v.pdf)
