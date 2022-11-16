FROM golang:1.19.3-buster

RUN mkdir -p /app
WORKDIR /app

COPY go.mod go.sum main.go /app/
RUN go build -o time-server .

CMD ["/app/time-server"]
EXPOSE 1234