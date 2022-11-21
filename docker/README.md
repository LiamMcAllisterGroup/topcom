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


# Different architectures

Building the package for a different archtecture is even more tricky. Here we will build for ARM64 while being on AMD64. There might be a better alternative, but this is the way that I got it to work.

First, we install and register qemu as follows.

```bash
sudo apt install -y qemu qemu-user-static qemu-user binfmt-support
docker run --rm --privileged multiarch/qemu-user-static:register --reset
```

Then copy this file (with the target arch) to the current directory.
```bash
cp /usr/bin/qemu-aarch64-static .
```
Now, add the following line in the Dockerfile after the FROM statement.
```
COPY ./qemu-aarch64-static /usr/bin/qemu-aarch64-static
```

Finally, build the image as follows.

```bash
docker buildx build -t debian-builder-arm64 --platform=linux/arm64 .
```

