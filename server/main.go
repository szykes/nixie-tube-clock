package main

import (
	"log"
	"net"
	"strconv"
	"time"
)

const port = "1234"
const red_ratio = 5
const green_ratio = 0
const blue_ratio = 5

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
			data_str += ";"
			data_str += convertIntToString(red_ratio)
			data_str += ";"
			data_str += convertIntToString(green_ratio)
			data_str += ";"
			data_str += convertIntToString(blue_ratio)
			data_str += ";"

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
