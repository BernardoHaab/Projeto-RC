FROM debian:12

RUN apt-get update && apt-get install -y \
	    gcc \
	    gdb \
	    iputils-ping \
	    make \
	    netcat-traditional \
	    net-tools \
	    nmap \
	    traceroute

RUN apt-get update \
	    && apt-get install -y locales \
	    && rm -rf /var/lib/apt/lists/* \
	&& localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG en_US.utf8

RUN chmod 777 /home
WORKDIR /gns3volumes/home
