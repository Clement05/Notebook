from flask import Flask, redirect, url_for, render_template, request
from markupsafe import escape
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

@app.route("/homedata")
def GetHome():
    res = netatmo.GetHome()
    return res

@app.route("/turnlight/<status>")
def TurnLight(status):
    if escape(status) == "on":
        netatmo.TurnLight(status)
        return status
    elif escape(status) == "off":
        netatmo.TurnLight(status)
        return status
    elif escape(status) == "auto":
        netatmo.TurnLight(status)
        return status
    elif escape(status)== "refreshtokenmanually":
        netatmo.LoadConfig()
        netatmo.GetRefreshToken()
        return "ask for new token"
    else:
        return "error args, should be on, off, auto"

