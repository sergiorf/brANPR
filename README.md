## Synopsis

Essentially started as a fork of the ANPR engine available in https://github.com/MasteringOpenCV/code
I built a Qt GUI around it and modified a bit the preprocessing module to cope with Brazilian plates, plus some other small modifications.
It comes with some sample images of front-plate Brazilian trucks.

## Installation

It uses CMAKE. You need to edit the CMakeLists.txt and change the paths to your own installation of OpenCV 2.4 and Qt5.
I have developed the program only with VS2013.

## Planned Modifications

The OCR module uses OpenCV's implementation of a multi-layer perceptron. 
I would like to compare its performance against the Tesseract OCR engine.

## License

Use the program as you wish.
You may contact me if you need help adapting it to your specific problem, I will be glad to help.

