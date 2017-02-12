#!/bin/bash

declare -A arr=([GET / HTTP/1.0]=200
		[GET /http.request HTTP/1.0]=200
		[GET /cantRead HTTP/1.0]=404
		[GET /nofile HTTP/1.0]=404
		[GET / HTTP/1.1]=400
		[GET /gnu/main.html HTTP/1.0]="</html>"
		[GET /gnu/nofile HTTP/1.0]=404
		[get / http/1.0]=200
		[GET /gnu/ HTTP/1.1]=400
		[GET /../ HTTP/1.0]=404
		[got / http/1.0]=400
		[GET /Index.html HTTP/1.0]=404		
		)

for key in "${!arr[@]}"; do
	echo "Testing " $key
	if echo -e -n $key"\r\n\r\n" | nc -u -w1 127.0.0.1 8080 | grep -q "${arr[$key]}"; then
		echo "Passed."
	else
		echo "Failed."
	fi
	sleep 3
done 

