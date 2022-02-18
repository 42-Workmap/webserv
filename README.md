# webserv

<img width="515" alt="Screen Shot 2022-02-12 at 5 48 48 PM" src="https://user-images.githubusercontent.com/18901638/153704436-d6ae81a4-9a2d-4508-bc7e-a0020e94e7dd.png">


## Description

webserv is a I/O Multiplexing HTTP/1.1 server. 

## Usage

```shell
# Compile the sources
make
```
```shell
# Run the server (if no 
./webserv [config_file]
```

### Configuration File Example

```
server
{
	server_name default
	listen 8180 127.0.0.1			

	location /					
	{
		error_page 404 ./tests/test1/error404.html
		allow_methods GET POST DELETE
		root ./tests/test1/
		index index.html index2.html
		auto_index on
		cgi_info .php ./tests/tester_bin/php-cgi
		cgi_info .bla ./tests/tester_bin/cgi_tester
	}
	
	location /auth/
	{
		allow_methods GET POST DELETE
		request_max_body_size	42
		root ./tests/test1/
		index index.html index2.html
		auto_index on
		cgi_info .php	./tests/tester_bin/php-cgi
		auth_key selim:1234
	}
}

```


## Authors

* [selim](https://github.com/seohl16)
* [jiholee](https://github.com/zittoooo)
