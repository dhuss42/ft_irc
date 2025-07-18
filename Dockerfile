FROM debian:latest

RUN apt-get update && apt-get upgrade -y && \
	apt-get install -y \
		g++ \
		build-essential \
		valgrind \
		make \
	&& apt-get clean && \
	rm -rf /var/lib/apt/lists/*

EXPOSE 12345

CMD ["bash"]
