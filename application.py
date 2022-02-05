from flask import Flask, redirect, url_for, render_template

app = Flask(__name__)


@app.route('/')
def index():
    '''Index page route'''

    return render_template("index.html")
