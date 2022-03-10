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
