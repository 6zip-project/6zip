FROM phusion/baseimage:focal-1.0.0
LABEL maintainer="Zip Developers <dev@zip.online>"
LABEL description="Dockerised ZipCore, built from Travis"

ARG USER_ID
ARG GROUP_ID

ENV HOME /home/zip

# add user with specified (or default) user/group ids
ENV USER_ID ${USER_ID:-1000}
ENV GROUP_ID ${GROUP_ID:-1000}
RUN groupadd -g ${GROUP_ID} zip && \
    useradd -u ${USER_ID} -g zip -s /bin/bash -m -d /home/zip zip && \
    mkdir /home/zip/.zipcore && \
    chown ${USER_ID}:${GROUP_ID} -R /home/zip

COPY bin/* /usr/local/bin/

RUN chmod a+x /usr/local/bin/*

USER zip

VOLUME ["/home/zip"]

EXPOSE 9998 9999 19998 19999

WORKDIR /home/zip
