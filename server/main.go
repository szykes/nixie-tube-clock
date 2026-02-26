package main

import (
	"fmt"
	"log"
	"net"
	"os"
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

func app() error {
	log.Println("Started, listening on: " + port)

	ln, err := net.Listen("tcp", ":"+port)
	if err != nil {
		return fmt.Errorf("listen: %w", err)
	}
	defer func() {
		err := ln.Close()
		if err != nil {
			log.Printf("Failed to close listener: %v", err.Error())
		}
	}()

	for {
		log.Println("Wait for accept")

		conn, err := ln.Accept()
		if err != nil {
			return fmt.Errorf("accept: %w", err)
		}

		go func(c net.Conn) {
			defer func() {
				err := c.Close()
				if err != nil {
					log.Printf("Failed to close listener: %v", err.Error())
				}
			}()

			t := time.Now()
			data_str := convertIntToString(t.Hour())
			data_str += convertIntToString(t.Minute())
			data_str += convertIntToString(t.Second())

			time_byte := []byte(data_str)

			log.Println("Send time '" + data_str + "', to: " + c.RemoteAddr().String())

			_, err := c.Write(time_byte)
			if err != nil {
				log.Printf("failed to write: %v", err.Error())
				return
			}
		}(conn)
	}
}

func main() {
	err := app()
	if err != nil {
		log.Printf("Failed to run app: %v\n", err.Error())
		os.Exit(1)
	}
}
