# Wog    

[中文版](/README_zh.md)   

![/doc/screenshot/a.gif](/doc/screenshot/a.gif)    

Wog is a library for creating opengl window and handle input on Windows. 
To use this library, you can simply trop `wog.h` and `wog.c` into your 
projects and compile these two files along with your source files.       

Both windows app(i.e. non console) and console app(i.e. with console) are 
supported. To build windows app, in Visual Studio, set `subsystem` to 
`Window`. To build console app, set `subsystem` to `Console`.    

See [/test/main.c](/test/main.c) for example.    

Copyright (C) 2015~2017 chai(neonum)