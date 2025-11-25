package main

import "strconv"

type Converter interface {
	FormString(StrValue string)
	ToString() string
}

type Endpoint struct {
	Ip   string
	Port int
}

func (Data *Endpoint) FormString(StrValue string) {
	Data.Ip = StrValue[:len(StrValue)-5]
	Data.Port, _ = strconv.Atoi(StrValue[len(StrValue)-4:])
}

func (Data *Endpoint) ToString() string {
	return Data.Ip + ":" + strconv.Itoa(Data.Port)
}

type Message struct {
	Len  int
	Data []byte
}

func (Data *Message) FormString(StrValue string) {
	Data.Len = len(StrValue)
	Data.Data = []byte(StrValue)
}

func (Data *Message) ToString() string {
	return string(Data.Data[0:Data.Len])
}

type Server struct {
	ClientEndpoint chan Endpoint
}
