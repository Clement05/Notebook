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
            json_data = r.json()
            json_data.update({"at": int(round(datetime.datetime.now().timestamp()))})
            SaveConfig(json.dumps(json_data))
            print(json_data)
    
    return json.dumps(json_data)

def GetRefreshToken():
    payload = {"grant_type": "refresh_token", "client_id": secret.client_id, "client_secret": secret.client_secret, "refresh_token": LoadConfig()["refresh_token"]}
    r = requests.post("https://api.netatmo.com/oauth2/token", data=payload)
    data = r.json()
    data["at"] = datetime.datetime.now().timestamp()
    SaveConfig(json.dumps(data))

def GenerateHeader():
    data = LoadConfig()
    if datetime.datetime.now().timestamp() > data["at"] + data["expires_in"]:
        print("ask for refresh token")
        GetRefreshToken()
    else:
        expire_in = int(data["at"] + data["expires_in"] - datetime.datetime.now().timestamp())
        print ("access token still valid {}".format(expire_in))

    headers = CaseInsensitiveDict()
    headers["Content-Type"] = "application/json"
    headers["Authorization"] = "Bearer " + LoadConfig()["access_token"]
    return headers

def GetHome():
    headers = GenerateHeader()
    r = requests.get("https://api.netatmo.com/api/homesdata", headers=headers)
    return json.dumps(r.json())

