FROM golang:1.26-alpine3.23 AS build-env

RUN apk add --no-cache tzdata

RUN mkdir -p /app
WORKDIR /app

COPY go.mod go.sum main.go /app/

RUN go mod download
RUN CGO_ENABLED=0 go build -o time-server .

FROM scratch AS prod-env

COPY --from=build-env /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/
COPY --from=build-env /usr/share/zoneinfo /usr/share/zoneinfo

COPY --from=build-env /app/time-server /time-server

USER 65532:65532

ENV TZ=Europe/Budapest

EXPOSE 1234

ENTRYPOINT [ "/time-server" ]
