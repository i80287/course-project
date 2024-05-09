# Data Structures and Algorithms Visualization

## About
Application for visualization of the suffix data structure for strings operations called DFA Aho-Corasick.

## Build and run

**Requirements:**
* OpenGL 3.0 compatible GPU
* Supported C++ compiler
* Supported OS
* git version control system
* CMake build system

**Supported OS:**
* Windows
* Debian-based Linux distributions (this application should work on the other Linux distributions, but this feature has not been tested)

**C++ Compiler:**
* GCC (13.0.0+) (gcc from mingw-w64 from msys2 was tested on Windows)
* Clang (16.0.0+) (clang from msys2 was tested on Windows)

**Version control system**
* git (2.42.0+)

**Build system**
* CMake (3.5+)

Errors may occur on other OS and/or compilers

**Build:**

First of all, you should clone this repository using git. To do so, open terminal supported in your OS 
(for example, Command Prompt on Windows) and go to the folder which you want project to be downloaded to.

Then, run the following command:

    git clone -b dev --recurse-submodules https://github.com/i80287/course-project.git

After that, run the following command to change current directory and enter the new directory called 'course-project'

On Windows:

    cd .\course-project

On Linux:

    cd ./course-project

After that run build script:

On Windows when using g++ with make:

    .\build_unix_make_release.bat

On Windows when using vc++ with Visual Studio 17 (2022):

    .\build_vs17.bat

On Windows when using vc++ with Visual Studio 16 (2019):

    .\build_vs16.bat

On Linux (g++ or clang++ depending on your CC and CXX env variables):

    ./build_release.sh

After that, run the following command to run the application:

On Windows when using g++ with make:

    .\build_release\vis_actrie_app.exe

On Linux:

    ./build_release/vis_actrie_app

If you are using vc++ with Visual Studio, build project in the Visual Studio (.sln file will be generated in the folder "build_vsXX")

## Configuration update

To change generator used by the cmake you can specify argument -G in the build scripts

For example, see build_unix_make_release.bat and build_vs17.bat

To change compiler options for each compiler (GCC, Intel, MSVC, Clang with MSVC frontend, Clang with GNU frontend, Clang with Apple frontend) you can modify target_compile_options and target_compile_definitions in the CMakeLists.txt in the appropriate section

By default, additional warnings are set in target_compile_options when compiling with GCC

Flag `VIS_ACTRIE_APP_STATIC_LINK_GCC_STD_WINPTHREAD` can be set to `ON` to statically link c and c++ standart libraries and win pthread library when compiling with GCC on Windows. This option is set to `OFF` by default but set to `ON` in `build_unix_make_debug.bat` and `build_unix_make_release.bat`

# Визуализация структур данных и алгоритмов

## О приложении
Данное приложение визуализирует работу суффиксной структуры данных для работы со строками, называемой ДКА Ахо-Корасик.

## Сборка и запуск

**Требования:**
* OpenGL 3.0 совместимая видеокарта
* Поддерживаемый компилятор для языка C++ 
* Поддерживаемая ОС
* Система контроля версий git
* Система сборки CMake

**Поддерживаемые ОС:**
* Windows
* Debian-based Linux системы (другие Linux-based ОС должны поддерживать работу приложения, но не проверялись)

**Компиляторы C++:**
* GCC (13.0.0+) (на Windows тестировался gcc из mingw-w64 из msys2)
* Clang (16.0.0+) (на Windows тестировался clang из msys2)

**Система контроля версий**
* git (2.42.0+)

**Система сборки**
* CMake (3.5+)

При использовании других компиляторов на других ОС могут возникнуть трудности со сборкой приложения

**Сборка:**

В первую очередь, вам необходимо склонировать репозиторий при помощи git. Для того, чтобы сделать это, 
откройте терминал, поддерживаемый вашей ОС (например, Command Prompt на Windows), и перейдите в папку, 
в которую будет скачан код проекта.

Далее, выполните следующую команду:

    git clone -b dev --recurse-submodules https://github.com/i80287/course-project.git

After that, run the following command to change current directory and enter the new directory called 'course-project'

После этого, выполните следующую команду, чтобы изменить текущую папку и перейти в новую папку с именем 'course-project'

На Windows:

    cd .\course-project


    cd ./course-project

После этого запустите сборочный скрипт:

На Windows при использовании g++ и make:

    .\build_unix_make_release.bat

На Windows при использовании vc++ с Visual Studio 17 (2022):

    .\build_vs17.bat

На Windows при использовании vc++ с Visual Studio 16 (2019):

    .\build_vs16.bat

На Linux: (g++ или clang++ в зависимости от ваших CC и CXX переменных окружения):

    ./build_release.sh

После этого, выполните команду для запуска собранного приложения:

На Windows при использовании g++ и make:

    .\build_release\vis_actrie_app.exe

На Linux:

    ./build_release/vis_actrie_app

Если вы используете vc++ с Visual Studio, соберите проект в Visual Studio (.sln файл будет сгенерирован в папке "build_vsXX")
