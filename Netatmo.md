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

```
write a route in our application.py file:

```
@app.route("/auth")
def OAuth():
    if 'code' in request.args:
        res = netatmo.GetNewToken(request.args.get('code'))
    else:
        res = netatmo.GetNewToken()

    return res
```

The application.py file should look like below. make sure "request" has been added in the import section.
the request import will allow to filter url parameters later on.

```
from flask import Flask, redirect, url_for, render_template, request
import netatmo

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''

    return render_template("index.html")

@app.route("/auth")
def OAuth():
    if 'code' in request.args:
        res = netatmo.GetNewToken(request.args.get('code'))
    else:
        res = netatmo.GetNewToken()

    return res
```

create a netatmo.py file that will contain our logic.

```
touch netatmo.py
```

in the netamo.py file

```
def GetNewToken(code="test"):
    return code
```

browse to your local ip

http://XXX.XXX.X.XX:8081/auth

you should see "test"

http://XXX.XXX.X.XX:8081/auth?code=code

you should see "code"

# Create the App connection with Netatmo.

First modify our application.py to pass secret.py variable to our view

Args will be passed from our application.py file from the variable client_id, ip_local

```
return render_template("index.html", client_id=secret.client_id, client_secret=secret.client_secret, ip_local=secret.ip_local)
```

application.py should look like below:
```
from flask import Flask, redirect, url_for, render_template, request
import netatmo
import secret

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''


    return render_template("index.html", client_id=secret.client_id, client_secret=secret.client_secret, ip_local=secret.ip_local)


@app.route("/auth")
def OAuth():
    res=""
    if 'code' in request.args:
        res = netatmo.GetNewToken(request.args.get('code'))

    return res

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
Our home page looks like below

![image](https://user-images.githubusercontent.com/5168811/157648324-7f340d2f-b8e1-4cc6-82eb-77809c8ada28.png)

Our netatmo file will now contains code to retreive access token from netatmo and store it into token.json. But firts install requests pakage.
requests package allows us to send post request through http.

The authorization to netatmo documentation cna be found here: https://dev.netatmo.com/apidocumentation/oauth.

We will choose authorization with code instead of user login/password.

Once we have the authorization, access token will be saved locally to be reused later. Access token has validity to 108000s.

```
pipenv install requests
```
Modify the function GetNewToken with the proper netatmo calls.

```
def GetNewToken(code):
    payload = {'grant_type': 'authorization_code', 'client_id': secret.client_id, 'client_secret': secret.client_secret, 'redirect_uri':'http://'+secret.ip_local+':8081/auth', 'scope': 'write_presence read_presence', 'code': code}
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

If no error, save the access token to a token.json.

To do so create two more functions one for saving one for loading data from token.json

```
token_file = "token.json"

def LoadConfig():
    f = open(token_file, "r")
    conf = json.loads(f.read())
    return conf

def SaveConfig(json):
    f = open(token_file, "w")
    f.write(json)
    f.close()
```

netatmo.py file should look like this:
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
    payload = {'grant_type': 'authorization_code', 'client_id': secret.client_id, 'client_secret': secret.client_secret, 'redirect_uri':'http://'+secret.ip_local+':8081/auth', 'scope': 'write_presence read_presence', 'code': code}
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
Finally we need to create the token.json file where to save access keys

```
touch token.json
```

Browse to our home page

You should be redirected to the local address you specified in the redirect_uri
![image](https://user-images.githubusercontent.com/5168811/157646039-d3f0ec97-051d-4011-a7da-42455c4447e2.png)

with the following JSON
![image](https://user-images.githubusercontent.com/5168811/157646523-0acfb333-370e-4ce5-a877-2569e1f2785a.png)

if any errors, refer to the documentation
https://dev.netatmo.com/apidocumentation/oauth#errors

