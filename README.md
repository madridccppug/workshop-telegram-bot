# Madrid C/C++ User Group

## Workshop: How to build a Telegram Bot

In these examples we are demonstrating how to build a Telegram bot using the library
developed by @reo7sp: [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)

Slides corresponding to the [workshop on March 21th](https://docs.google.com/presentation/d/1B5pPftL06dW1k87M5eyMk-MwfkqXWhzXmXrN0kKP0dk/edit?usp=sharing)
by Madrid C/C++ User Group 

### Prerrequisites

We are using [Conan](https://conan.io/) to handle the dependencies in these examples, it
requires a minimum configuration to run:

```
$> pip install conan
$> conan profile show default
```

Some dependencies are not in the official repositories, so we will need to add
unofficial ones (or create packages using the recipes):

```
$> conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
$> conan remote add jgsogo https://api.bintray.com/conan/jgsogo/conan-packages
```

### Running the examples

Just enter the directory containing the example and build as usual:

```
$> cd <path/to/example>
$> mkdir build && cd build
$> conan install .. --build=missing
$> cmake .. -DCMAKE_BUILD_TYPE=Release
$> cmake --build .
```

You will also need to [create the bot itself in Telegram and get a token](https://core.telegram.org/bots).
