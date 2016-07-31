## pictDB
 
This is a part of the EPFL's System programming project 2016 course. 
It aims to create a simple image store based on Facebook haystack model.

### Getting started

Install dependencies:
```shell
brew tap homebrew/science; brew install json-c vips openssl libtool
sudo apt install libssl-dev libvips-dev libjson-c
```

Download [Mongoose](https://github.com/cesanta/mongoose) `.c` and `.h` files and place them unider `pictDBM/libmongoose`.

Use basic Makefile:
```shell
make pictDBM
```

Or use CMake:
```shell
cmake .
make
```

### Command line

```shell
./pictDBM help
```

### Server mode

Run these commands given a `db` previously created with `./pictDBM create db` and go to `http://localhost:8000`:

```shell
export DYLD_FALLBACK_LIBRARY_PATH=libmongoose
./pictDB_server db
```

### LICENSE

Project is available under [CC-BY-NC-SA 4.0](http://creativecommons.org/licenses/by-nc-sa/4.0/) and provided files belong their owners under appropriate licensing.
