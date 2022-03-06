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
