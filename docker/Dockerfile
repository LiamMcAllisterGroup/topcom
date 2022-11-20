FROM debian:bullseye

RUN apt-get -yqq update
RUN apt-get -yqq install build-essential nano cmake dpkg-dev devscripts \
                         debhelper-compat libcdd-dev libgmp-dev \
                         texlive-latex-base texlive-plain-generic \
                         libbz2-dev libqsopt-ex-dev txt2man zlib1g-dev

WORKDIR /home/
