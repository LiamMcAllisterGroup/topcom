# Building the TOPCOM Debian package
Building the debian package can be a little annoying the first time, so I collected the instructions and tools I used to build it. There is a Dockerfile that is used to produce a Docker image with all of the necessary dependencies. In the Docker container the command used to build the package is either
```bash
dpkg-buildpackage -rfakeroot -b -uc -us
```
or
```bash
debuild -b -uc -us
```
The full instructions to build a Debian package were originally taken from this [website](https://ostechnix.com/how-to-build-debian-packages-from-source/).

### Note
When you build the package it will change a bunch of the files in the repository and create new ones. Therefore, you should make a temporary copy of the directory to be used for this. Otherwise, committing new changes to the repository will become a mess.
