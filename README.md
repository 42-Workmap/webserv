<img width="800" alt="redirection" src="https://user-images.githubusercontent.com/18901638/155832146-811d8d83-968d-4af6-826d-118471362f41.gif">

# webserv

webserv is a I/O Multiplexing HTTP/1.1 server. 

## Authors

* [selim](https://github.com/seohl16)
* [jiholee](https://github.com/zittoooo)
[노션 페이지](https://excited-kilogram-5f6.notion.site/2893d5376f1d4bdb98d8fddca3048dc8?v=3a0e8b56a903489fb4c82e2a21c7e5b7)

## 1. Build & Usage

```shell
# Compile the sources
make
```
```shell
# Run the server (if no 
./webserv [config_file]
```


## 2. Features 
- Nginx-like configuration file 
- HTTP Protocol (ex. status line, request line...)
- HTTP 1.1 Header
- I/O multiplexing (kqueue)
- GET, POST, DELETE HTTP methods 
- HTTP redirections 
- HTTP response status codes (ex. 200, 301...)
- CGI implementation (ex. bla, php...)
- Auto Index on/off


## 3. Configuration File Example

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
- `server` : server block 
- `listen` : port and ip
- `location` : set for specific route
- `error_page` : path for custom error page 
- `allow_method` : allowed methods for HTTP requests
- `index` : default file for the requested URI
- `cgi_info` : cgi execute for specific file extension
- `request_max_body_size` : max limit for request body size
- `auth_key` : authorization needed

## 4. Test
- curl 
```
curl -X POST -H “Content-Type: plain/text” –data “BODY is here and write something longer or shorter than limit”
```
- Postman 

```
[POST] localhost:8179/index.html 
```
<img width="1000" alt="post" src="https://user-images.githubusercontent.com/18901638/155831992-8e27d06b-1a31-454e-8675-3c2371981d91.gif">

```
[GET/DELETE] localhost:8179/index
```
<img width="1000" alt="get,delete" src="https://user-images.githubusercontent.com/18901638/155831487-ccacbba3-032f-4adc-851f-6b45a6983ac1.gif">

<!-- ![post__640](https://user-images.githubusercontent.com/18901638/155831268-e4afb468-2e02-41c0-aae0-aba2b0a2fde0.gif) -->

<!-- ![get,delete](https://user-images.githubusercontent.com/18901638/155831487-ccacbba3-032f-4adc-851f-6b45a6983ac1.gif) -->

<!-- ![post!!!!!](https://user-images.githubusercontent.com/18901638/155831992-8e27d06b-1a31-454e-8675-3c2371981d91.gif) -->

- Browser 
```
localhost:8182
```
<img width="1000" alt="redirection" src="https://user-images.githubusercontent.com/18901638/155832146-811d8d83-968d-4af6-826d-118471362f41.gif">
<!-- ![redirection](https://user-images.githubusercontent.com/18901638/155832146-811d8d83-968d-4af6-826d-118471362f41.gif) -->


```
localhost:8180/youpi.php
```
<img width="1000" alt="cgi" src="https://user-images.githubusercontent.com/68208055/155832473-5836add7-de0f-4770-a7d5-bdefc9fb0096.gif">


- Siege
```shell
# 소켓 제한을 풀어주는 명령어 
sudo sysctl -w net.inet.tcp.msl=100

# seige 명령어
siege -b -R <(echo connection = keep-alive) -c100 127.0.0.1:8180
```
<img width="1000" alt="seige" src="https://user-images.githubusercontent.com/18901638/155832738-35b79463-070a-4c44-9331-0fa319b8ba44.gif">

<!-- ![siege](https://user-images.githubusercontent.com/18901638/155832738-35b79463-070a-4c44-9331-0fa319b8ba44.gif) -->

## 5. HTTP Method function explained with flowchart 

### 5.1. GET
<img maxwidth='700' src='./flowchart/GET.png'>
<br>


### 5.2. POST
<img maxwidth='700' src='./flowchart/POST.png'>
<br>


### 5.3. DELETE
<img maxwidth='700' src='./flowchart/DELETE.png'>
<br>

### 5.4. REDIRECTION
<img width='400' src='./flowchart/REDIRECTION.png'>
<br>


### 5.5. CGI
<img width='500' src='./flowchart/CGI.png'>
<br>

