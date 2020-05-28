from flask import Flask
from flask import request, redirect, url_for, render_template
from flask_sqlalchemy import SQLAlchemy
import paho.mqtt.client as mqtt


app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres:gutter123@localhost/detgutternesdb'
db = SQLAlchemy(app)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    
    client.subscribe("gpstest/#")


def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    if ("gpstest" in msg.topic):
        if ("hej" in str(msg.payload)):
            print("CEO")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("test.mosquitto.org", 1883, 60)


class Iotdata(db.Model):
   id = db.Column('id', db.Integer, primary_key = True)
   gpslon = db.Column(db.Numeric(20,6)) 
   gpslat = db.Column(db.Numeric(20,6))   
   temp = db.Column(db.String(50))
   ts =  db.Column(db.TIMESTAMP)             

   def __init__(self, gpslon, gpslat, temp, ts):
      self.gpslon = gpslon
      self.gpslat = gpslat
      self.temp = temp
      self.ts = ts

   def __repr__(self):
       return self.temp

@app.route('/')
def hello():
    return "Hello World!"

@app.route('/input')
def input():
    return render_template('adddata.html')

@app.route('/saveddata')
def saveddata():
    return render_template('saveddata.html')

@app.route('/showdata')
def showdata():
    myIotdata = Iotdata.query.all()
    return render_template('showdata.html', myIotdata=myIotdata)


@app.route('/post_data')
def post_data():
    iotdata = Iotdata(gpslon = 55.866163, gpslat = 9.843507, temp = '25', ts = '2020-05-25 19:10:25-07')
    db.session.add(iotdata) 
    db.session.commit() 
    return "<h1 style='color: cyan'>hello flask - saved some data</h1>"

@app.route('/post_iotdata', methods=['POST']) 
def post_iotdata():
    iotdata = Iotdata(gpslon = 55.866163, gpslat = 9.843507, temp = 25, ts = '2020-05-28 19:10:25-07')
    db.session.add(iotdata) 
    db.session.commit() 
    return redirect(url_for('saveddata'))

@app.route('/<name>')
def hello_name(name):
    return "Hello {}!".format(name)

if __name__ == '__main__':
    app.run(debug=True)
client.loop_forever()
