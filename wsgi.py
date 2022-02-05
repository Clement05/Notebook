from application import app
import secret

if __name__ == '__main__':
    app.run(debug=False, host=secret.ip_local, port=secret.gunicorn_port)
