# Framesplash
## Overview

Framesplash is my own (work in progress) C++ implementation of Jozsef Fejes' [Rainbow Smoke](https://www.youtube.com/watch?v=OuvFsB4SLhA) algorithm. 
In this project, I built upon Fejes' original idea of 'painting' the full 8-bit colour spectrum onto a canvas by placing each colour pixel next to it's closest neighbours. 


## Features

- Custom colour palette generation via video/ image input.
- Custom colour data preprocessing options (Sequencial/ randomised/ weighted inputs). 
- Perlin noise generation and integration. 
- Origin point templating.
- 'Stacking Resolution' option.
- (wip) Multi-threaded processing.

## Demo Outputs

*Base rainbow smoke algorithm output - 8-bit colour spectrum. Preprocessing: Shuffled.*

![Base Rainbow Smoke Algorithm](./Media/Base.gif)


*Rainbow smoke using the following gif as preprocessed pixel colours. Preprocessing: Shuffled.*

![Output when using reference gif to generate colour dataset](./Media/ReferenceOutput.gif)
![The gif that was used as input](./Media/Reference.gif)


*Circle geometry template - also demonstrates 'Perlin noise' capability*

![Example of circle geometry templating](./Media/Template.gif)


*Stacking Resolution effect.*

![Demonstration of stacking resolution technique](./Media/Stacking.gif)


*Multithreading example with template effect. *

![Demonstrating multithreading toggled on with the circle template](./Media/Multithread.gif)


## Usage
To do: Start using the application straight away via command line interface.
To do: Working with the project code.


## Limitations & Future Improvements
To do.

### Potential improvements:
To do.


## License
To do.

