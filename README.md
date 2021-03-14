# CG

## Fase 1
Commands:

    ./generator plane "size" plane.3d

    ./generator box x y z slices box.3d
    
    ./generator sphere 1 10 10 sphere.3d
    
    ./generator cone 3 8 20 20 cone.3d
    
Keyboard Command:

    As teclas 'W','A','S','D' permitem-nos movimentar em câmara para qualquer direção.
    A tecla 'R' permite-nos ter o programa a correr e adicionar um nova primitiva a partir do terminal.


Para funcionar no Mac, adicionar estas três linhas à CMakeList:

    set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-10" CACHE STRING "C compiler" FORCE)
    set(CMAKE_C_COMPILER "/usr/local/bin/gcc-10" CACHE STRING "C++ compiler" FORCE)
    set(CMAKE_CXX_STANDARD 17)
#### E, meter a CMakeList fora do engine, na pasta Fase1.



    
