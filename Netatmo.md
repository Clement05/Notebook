# Go to dev.netatmo.com

[dev.netatmo](https://dev.netatmo.com/)

Register or login

# Create an app

Navigate to "My apps"

![image](https://user-images.githubusercontent.com/5168811/156924808-4d839c56-5b7c-4eac-9219-6e63d46fb9e5.png)

Click on the create button

[![image](https://user-images.githubusercontent.com/5168811/156924833-9c078c18-7f58-416a-9668-a911182469eb.png)](https://dev.netatmo.com/apps/createanapp#form)

Fill the form below

![image](https://user-images.githubusercontent.com/5168811/156924958-f4d23625-0cbb-4a31-a7c3-28ff99ffd65b.png)

app name: Flask_API

description: API test with flask

data protection officer name: with your name

data protection officer email: your email

Accept terms and condition

Click Save

![image](https://user-images.githubusercontent.com/5168811/156925136-bc3ad14f-c46f-4192-ba5f-d2355fa5b077.png)

Once validated you should see the app in your dashboard

![image](https://user-images.githubusercontent.com/5168811/156925247-8245d928-044c-46c6-a985-de894f7c704f.png)

Scroll down to the App Technical Parameters

![image](https://user-images.githubusercontent.com/5168811/156925474-e1011870-0579-42f5-821d-e12ca7a7a662.png)

Verify that the App Status is Activated

Verify that the Webhooks status is checked

Copy the client id and the client secret in secret.py
```
client_id = <YOUR CLIENT ID>
client_secret = <YOUR CLIENT SECRET>
```
# Create the netatmo application
Move to our api folder and create the netatmo application 
```
cd /var/www/api/

pipenv shell

touch netatmo.py
```

write the following code in our main flask application.py file

```
from flask import Flask, redirect, url_for, render_template, request
import netatmo

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''

    return render_template("index.html")

@app.route("/auth/connect/")
def OAuth():
    if 'code' in request.args:
        res = netatmo.GetNewToken(request.args.get('code'))
    else:
        res = netatmo.GetNewToken()

    return res
```

in the netamo.py file

```
def GetNewToken(code="test"):
    return code
```

browse to your local ip

http://XXX.XXX.X.XX:8081/auth/connect

you should see "test"

http://XXX.XXX.X.XX:8081/auth/connect?code=code

you should see "code"

Now let's connect our app.

create a token.json file where to save access keys
```
touch token.json
```

Modify the templates/index.html file to create a link to connect our app

```
{% extends "base.html" %}
{% block title %}Home API{% endblock %}
{% block content %}
<h1>Welcome Home!!</h1>
<a href="https://api.netatmo.com/oauth2/authorize?client_id={{ client_id }}&redirect_uri=http://{{ ip_local }}:8081/auth&scope=write_presence read_presence&state=1">Connect Netatmo App</a>
{% endblock %}

```

Args will be passed from our application.py file from the variable client_id, client_secret, ip_local

```
from flask import Flask, redirect, url_for, render_template, request
import netatmo
import secret

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''


    return render_template("index.html", client_id=secret.client_id, client_secret=secret.client_secret, ip_local=secret.ip_local)


@app.route("/auth/")
def OAuth():
    res=""
    if 'code' in request.args:
        res = netatmo.GetNewToken(request.args.get('code'))

    return res

```

Our netatmo file will now contains code to retreive access token from netatmo and store it into token.json. But firts install requests pakage

```
pipenv install requests
```

netatmo.py
```
import requests
import json
import datetime
from datetime import date
from requests.structures import CaseInsensitiveDict
import secret

token_file = "token.json"

def LoadConfig():
    f = open(token_file, "r")
    conf = json.loads(f.read())
    return conf

def SaveConfig(json):
    f = open(token_file, "w")
    f.write(json)
    f.close()

def GetNewToken(code):
    payload = {'grant_type': 'authorization_code', 'client_id': secret.client_id, 'client_secret': secret.client_secret, 'redirect_uri': secret.ip_local+':8081/auth', 'scope': 'write_presence read_presence', 'code': code}
    r = requests.post("https://api.netatmo.com/oauth2/token", data=payload)
    errors =  "error" in r.json()
    if errors == False:
            access_token = r.json()["access_token"]
            refresh_token = r.json()["refresh_token"]
            expires_in = r.json()["expires_in"]
            SaveConfig(json.dumps(r.json()))
            print(r.json())
        #elif r.json()["error"]["code"]:
        #       print(r.json()["error"]["message"])
        #else: print(r.json())
    return json.dumps(r.json())

```
