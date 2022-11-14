package main

import (
	"log"
	"net"
	"strconv"
	"time"
)

const port = "1234"

func convertIntToString(number int) string {
	str := strconv.Itoa(number)
	if len(str) == 1 {
		str = "0" + str
	}
	return str
}

func main() {
	log.Println("Started, listening on: " + port)

	ln, err := net.Listen("tcp", ":"+port)
	if err != nil {
		log.Fatalln(err)
	}

	for {
		log.Println("Wait for accept")
		conn, err := ln.Accept()
		if err != nil {
			log.Fatalln(err)
		}

		go func(c net.Conn) {
			t := time.Now()
			data_str := convertIntToString(t.Hour())
			data_str += convertIntToString(t.Minute())
			data_str += convertIntToString(t.Second())

			time_byte := []byte(data_str)

			log.Println("Send time '" + data_str + "', to: " + c.RemoteAddr().String())
			_, err := c.Write(time_byte)
			if err != nil {
				log.Fatalln(err)
			}

			log.Println("Close")
			c.Close()
		}(conn)
	}
}
