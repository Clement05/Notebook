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

