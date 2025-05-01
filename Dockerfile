FROM alpine:3.21 AS builder

RUN apk add --no-cache \
    build-base \
    autoconf \
    automake \
    linux-headers \
    zstd-dev \
    openssl-dev \
    git

COPY . /usr/src/n2n
WORKDIR /usr/src/n2n

RUN ./autogen.sh && \
    ./configure --prefix=/usr && \
    make && \
    make install

FROM alpine:3.21

RUN apk add --no-cache \
    libcap \
    zstd-libs \
    openssl

COPY --from=builder /usr/sbin/edge /usr/sbin/
COPY --from=builder /usr/sbin/supernode /usr/sbin/
COPY --from=builder /usr/bin/n2n-* /usr/bin/
    
RUN adduser -D -H -u 1000 n2nuser && \
    setcap 'cap_net_admin=+ep' /usr/sbin/edge

USER n2nuser

EXPOSE 7654

CMD ["supernode", "-f", "-p", "7654"]
