# Framesplash - A generative art engine for the Rainbow Smoke Algorithm (C++ 17, OpenCV 4.6.0) (WIP)
## Overview

Framesplash is my own C++ implementation of Jozsef Fejes' [Rainbow Smoke](https://www.youtube.com/watch?v=OuvFsB4SLhA) algorithm. 
In this project, I built upon Fejes' original idea of 'painting' the full 8-bit colour spectrum onto a canvas by placing each colour pixel next to it's closest neighbours. 


## Features

- Custom colour palette generation via video/ image input.
- Custom colour data preprocessing options (Sequencial/ randomised/ weighted inputs). 
- Perlin noise generation and integration. 
- Origin point templating.
- 'Stacking Resolution' option.
- (wip) Multi-threaded processing.

## Demo Outputs

*Base rainbow smoke algorithm output - 8-bit colour spectrum. Shuffled colour preprocessing.*

![Base Rainbow Smoke Algorithm](./Media/Base.gif)


*Rainbow smoke using the following gif as preprocessed pixel colours with 'shuffled' colour preprocessing.*
![Output when using reference gif to generate colour dataset with shuffled colours](./Media/ReferenceOutputShuffled.gif)

*Same parameters but colour preprocessing is not shuffled.*
![Output when using reference gif to generate colour dataset with non-shuffled colours](./Media/ReferenceOutputWeighted.gif)

*The gif used as an input.*
![The gif that was used as input](./Media/Reference.gif)


*Circle geometry template - also demonstrates 'Perlin noise' capability*
![Example of circle geometry templating](./Media/Template.gif)


*Stacking Resolution effect.*
![Demonstration of stacking resolution technique](./Media/Stacking.gif)


*Multithreading example with template effect.*
![Demonstrating multithreading toggled on with the circle template](./Media/Multithread.gif)


## Limitations & Future Improvements
Currently, the project is not very easy to use or developer-friendly. It was started several years ago and incrementally added to over time. 
As a result, there are several improvements that can be made both in terms of optimization & usability.

- Replace OpenCV as the main image processing library to either OpenGL or even SFML.
- Add a command line interface so users that don't code can still interact with the software.
- Add cleaner & more consistent coding conventions.
- Separate out the different larger components into their own projects.
- Finalise single-thread performance improvements where necessary. Add <chrono> utility for measuring performance.

As for future features:
- Add more advanced templating options.
- More flexibility with manual colourset generation.
- Multithreaded file processing.
- Frame-to-frame animation output.
