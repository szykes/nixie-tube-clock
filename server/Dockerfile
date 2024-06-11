FROM golang:1.22.4-alpine3.19 AS build-env

RUN mkdir -p /app
WORKDIR /app

COPY go.mod go.sum main.go /app/
RUN go build -o time-server .

FROM alpine:3.19.1 AS prod-env

COPY --from=build-env --chown=root:root --chmod=001 /app/time-server /time-server

RUN addgroup -g 10001 app && \
   adduser --disabled-password -u 10000 -G app app

USER app:app

ENTRYPOINT [ "/time-server" ]
EXPOSE 1234
