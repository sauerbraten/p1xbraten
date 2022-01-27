FROM alpine:latest as builder

RUN apk add --upgrade \
    make \
    build-base \
    zlib-dev

# make sure to `make clean` in src/ and src/enet/ before
COPY ./src /src

RUN cd src && \
    make server && \
    strip sauer_server


FROM alpine:latest

RUN apk add --upgrade libstdc++

COPY --from=builder /src/sauer_server /server

COPY ./Docker.server-init.cfg /server-init.cfg

# remove the next line when you don't use p1xbraten sources or `/checkmaps` will fail
RUN wget -O - https://static.p1x.pw/slim_ogzs.tar.gz | tar -xzof -

# remove -i part when not running on fly.io
CMD [ "/server", "-ifly-global-services" ]
