Source: [techwithtim](https://www.techwithtim.net/tutorials/flask/adding-bootstrap/)
Source: [techwithtim](https://www.techwithtim.net/tutorials/flask/adding-bootstrap/)

Following Brandon Wallace tutorial

# Prepare system:
```
$ sudo apt update

$ sudo apt install python3-pip

$ pip3 install pipenv

$ sudo apt install vim
```
# Create Project Directory
```
$ sudo mkdir /var/www/api

$ ls -ld /var/www/api
drwxr-xr-x 2 root root 4096 Jan 27 12:37 /var/www/api/

$ sudo chown -R pi:www-data /var/www/api

$ ls -ld /var/www/api
drwxr-xr-x 2 pi www-data 4096 Jan 27 12:37 /var/www/api/
```
# Create .env to Manage Environmental Variables
```
$ cd /var/www/api

$ touch .env

$ vim .env
```
.env file
```
FLASK_APP=wsgi.py
FLASK_ENV=production
```
# Preprare Pipenv
```
$ pipenv shell 

[...]
✔ Successfully created virtual environment! 
Virtualenv location: /home/pi/.local/share/virtualenvs/api-AAABBBCCC
Creating a Pipfile for this project…
Launching subshell in virtual environment…
 . /home/pi/.local/share/virtualenvs/api-AAABBBCCC/.venv/bin/activate

((api)) pi@server:/var/www/api $

$ pipenv install flask gunicorn
```
# Setup Flask
```
$ touch application.py wsgi.py

$ vim application.py
```
application.py file
```
from flask import Flask

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''

    return '<h1>Hakuna Matata!</h1>'

```
```
$ vim wsgi.py
```
wsgi.py file
```
from application import app

if __name__ == '__main__':
    app.run(debug=False, host='XXX.XXX.X.XX', port=PPPP)
```
Where XXX.XXX.X.XX is your local ip

PPPP is the port you chose

By default port is 5000, port is optional.

Test the Flask app:
```
((api)) brandon@server:/var/www/api $python wsgi.py
```
browse http://XXX.XXX.X.XX:PPPP
# Setup Gunicorn
test Gunicorn
```
$ gunicorn --workers 4 --bind XXX.XXX.X.XX:PPPP wsgi:app
```
You should get something like this
```
$ gunicorn --workers 4 --bind XXX.XXX.X.XX:PPPP wsgi:app
[2021-01-27 14:28:00 -0500] [24662] [INFO] Starting gunicorn 20.0.4
[2021-01-27 14:28:00 -0500] [24662] [INFO] Listening at: http:/XXX.XXX.X.XX:PPPP (24662)
[2021-01-27 14:28:00 -0500] [24662] [INFO] Using worker: sync
[2021-01-27 14:28:00 -0500] [24695] [INFO] Booting worker with pid: 24695
```
# Install Nginx
```
$ sudo apt install nginx

$ sudo systemctl enable nginx

$ sudo systemctl start nginx

$ sudo systemctl status nginx
```
# Create Service from Flask app
```
touch api.service

$ sudo vim /etc/systemd/system/api.service
```
api.service file
```
[Unit]
Description=api.service
After=network.target

[Service]
User=pi
Group=www-data
WorkingDirectory=/var/www/api/
ExecStart=/home/pi/.local/share/virtualenvs/api-AAABBBCCC/bin/gunicorn --workers 3 \
--bind unix:/var/www/api/api.sock wsgi:app \
--user pi --group www-data

[Install]
WantedBy=multi-user.target
```

```
$ systemctl enable api

$ systemctl start api

$ systemctl status api
```
# Nginx Config for our Service
```
touch /etc/nginx/sites-available/api.conf

$ sudo vim /etc/nginx/sites-available/api.conf
```
api.conf file (I change port to 8081, because 80 was already busy by another application
```
server {
        listen 8081;
        server_name api www.api;

        access_log /var/log/nginx/api.access.log;
        error_log /var/log/nginx/api.error.log;

        location / {
                include proxy_params;
                proxy_pass http://unix:/var/www/api/api.sock;
        }
}
```
```
$ sudo ln -s /etc/nginx/sites-available/api.conf /etc/nginx/sites-enabled/

$ ls -l /etc/nginx/sites-enabled/ | grep api

$ nginx -t

$ sudo systemctl restart nginx

$ sudo systemctl status nginx
```
browse http://XXX.XXX.X.XX:8081
