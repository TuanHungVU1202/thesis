//check running time for the whole server
const {performance} = require('perf_hooks')
var t0 = performance.now()


//program starts here
var express = require('express')		//add comment de test github update
var app = express()                 //4th time test push
var exphbs  = require('express-handlebars')

//set up for mqtt client
var mqtt = require('mqtt')
//connect directly
var mqttClient = mqtt.connect ('mqtt://localhost:3000')

//connect over Websocket
// var mqttClient = mqtt.connect ('ws://localhost:3021')
var receivedMessage
var topicStr

//vars for handleImage pages
var fs = require ('fs')
var pathToImgArray = []

//config server on port 2111
//and setup SSL cert for https
var port = process.env.PORT || 2111
app.listen(port, function() {
    console.log("App is running on port " + port)
})


assert = require('assert')
var max = 5


//for loadImgDir function
var basePathToImgDir = './public/images'


//mongodb on mlab
//var mongourl= 'mongodb://admin:admin123@ds139942.mlab.com:39942/mongotest-1';
var mongourl= 'mongodb://localhost:27017/myDatabase';
var MongoClient = require('mongodb').MongoClient;


//setup socket.io
const socketClient = require('socket.io').listen(1202).sockets;

//setup views
const path = require('path');
app.set('view engine', 'handlebars');
app.engine('handlebars', exphbs({defaultLayout: 'main'}));
app.use(express.static(path.join(__dirname, 'public')));

var bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({
    extended: true,
    limit: '50mb'
}));
app.use(bodyParser.json({limit: '50mb'}));


//Init variables for data from Sensor
var temperature = 21;
var humid = 11;
var gasDetection = "NO";
var humanDetection = "NO";
var securityStatus = "UNARMED";
var Power = 0;


/*  getDate, getMonth, getFullYear are defined methods
    Methods return number, eg. getDate returns day of the month from 1 to 31
    details at https://www.w3schools.com/jsref/jsref_obj_date.asp
    */
var d1 = new Date();
dateNow =  d1.getDate();
monthNow = d1.getMonth() + 1;	//return value from 0 so must plus 1
yearNow =  d1.getFullYear();
var dateFilter = dateNow;
var monthFilter = monthNow;
var yearFilter = yearNow;

var chartTime = [];
var chartPower = ['21', '11', '12', '02'];
var chartCount = 0;
//var chartTime2 = ["1:00","3:30","4:15","6:15","7:15","8:15"];

//Login variables for webServer
var username = "vthung";
var password = "admin";
var loginFlag = false;
var masterPass = "lifeIn5Months";
var loginCamFlag = false;

var checkChangedFlag = {};
checkChangedFlag.changedFlagStatus = "false";

//init states for devices
var deviceState = {};

deviceState.device1 = "off";
deviceState.device2 = "off";
deviceState.device3 = "off";
deviceState.device4 = "off";
deviceState.device5 = "off";

deviceState.device1TimeOn = "00:00";
deviceState.device1TimeOff = "00:00";
deviceState.device2TimeOn = "00:00";
deviceState.device2TimeOff = "00:00";
deviceState.device3TimeOn = "00:00";
deviceState.device3TimeOff = "00:00";
deviceState.device4TimeOn = "00:00";
deviceState.device4TimeOff = "00:00";
deviceState.device5TimeOn = "00:00";
deviceState.device5TimeOff = "00:00";

//init scenes
var scenes = {};
scenes.iAmHome = "off";
scenes.goodmorning = "off";
scenes.goodnight = "off";
scenes.security = "off";


//setup to get local ip
var os = require( 'os' );

//--------code start ------------------------------------------------
//Init functions to call at the beginning
//set Interval for it to rerun after some times  <<======= TOO LAGGY
//Restart server is a better solution
//load image data at the beginning
loadImgDir();
//get local IP
getLocalIp()
//-------------------------------------------------------------------

//begin
//use socket io to reaceive command from RN
socketClient.on('connection', function(socket){
    MongoClient.connect(mongourl, function(err, db) {
        var floor1 = db.collection('floor1')
        var logDeviceActivities = db.collection('logDeviceActivities')

        //fetching data to android
        socket.on('requestToFetchData', function(requestToFetch){
            // console.log(requestToFetch)
            if (requestToFetch === 'positive') {
                var cursor1 = floor1.find(
                    {_id: {$eq:"F1.1"}},
                );
                cursor1.forEach(
                    function (doc) {
                       responseState = doc.state;
                       // console.log('d1 ', responseState)
                       socket.emit('resD1', responseState)
                    }
                );
                var cursor2 = floor1.find(
                    {_id: {$eq:"F1.2"}}
                );
                cursor2.forEach(
                    function (doc) {
                        responseState = doc.state;
                        // console.log('d2 ', responseState)
                        // socket.emit('responseDevice', 2)
                        socket.emit('resD2', responseState)
                    }
                );
                var cursor3 = floor1.find(
                    {_id: {$eq:"F1.3"}}
                );
                cursor3.forEach(
                    function (doc) {
                        responseState = doc.state;
                        // console.log('d3 ', responseState)
                        // socket.emit('responseDevice', 3)
                        socket.emit('resD3', responseState)
                    }
                );
                var cursor4 = floor1.find(
                    {_id: {$eq:"F1.4"}}
                );
                cursor4.forEach(
                    function (doc) {
                        responseState = doc.state;
                        // console.log('d4 ', responseState)
                        // socket.emit('responseDevice', 4)
                        socket.emit('resD4', responseState)
                    }
                );
                var cursor5 = floor1.find(
                    {_id: {$eq:"F1.5"}}
                );
                cursor4.forEach(
                    function (doc) {
                        responseState = doc.state;
                        // console.log('d4 ', responseState)
                        // socket.emit('responseDevice', 4)
                        socket.emit('resD5', responseState)
                    }
                );
            }
        })

        //handle data from Android
        socket.on('deviceFromAndroid', function (idFromAndroid) {
            socket.on('stateFromAndroid', function (stateFromAndroid) {
                switch (idFromAndroid) {
                    case 1:
                        mqttClient.publish('toEsp/control/device/1', stateFromAndroid)
                        //log data into Mongodb
                        logDeviceActivities.insertOne({
                            "deviceId": "F1.1",
                            "state": stateFromAndroid,
                            "Timestamp": getTime(),
                            "Day": myTodayDate().myDay,
                            "Date": myTodayDate().myDate,
                            "Month": myTodayDate().myMonth,
                            "Year": myTodayDate().year,
                        })
                        floor1.updateMany(
                            {"_id": "F1.1"},
                            {$set: {state: stateFromAndroid}},
                        );
                        break;
                    case 2:
                        mqttClient.publish('toEsp/control/device/2', stateFromAndroid)
                        logDeviceActivities.insertOne({
                            "deviceId": "F1.2",
                            "state": stateFromAndroid,
                            "Timestamp": getTime(),
                            "Day": myTodayDate().myDay,
                            "Date": myTodayDate().myDate,
                            "Month": myTodayDate().myMonth,
                            "Year": myTodayDate().year,
                        })
                        floor1.updateMany(
                            {"_id": "F1.2"},
                            {$set: {state: stateFromAndroid}},
                        );
                        break;
                    case 3:
                        mqttClient.publish('toEsp/control/device/3', stateFromAndroid)
                        logDeviceActivities.insertOne({
                            "deviceId": "F1.3",
                            "state": stateFromAndroid,
                            "Timestamp": getTime(),
                            "Day": myTodayDate().myDay,
                            "Date": myTodayDate().myDate,
                            "Month": myTodayDate().myMonth,
                            "Year": myTodayDate().year,
                        })
                        floor1.updateMany(
                            {"_id": "F1.3"},
                            {$set: {state: stateFromAndroid}},
                        );
                        break;
                    case 4:
                        mqttClient.publish('toEsp/control/device/4', stateFromAndroid)
                        logDeviceActivities.insertOne({
                            "deviceId": "F1.4",
                            "state": stateFromAndroid,
                            "Timestamp": getTime(),
                            "Day": myTodayDate().myDay,
                            "Date": myTodayDate().myDate,
                            "Month": myTodayDate().myMonth,
                            "Year": myTodayDate().year,
                        })
                        floor1.updateMany(
                            {"_id": "F1.4"},
                            {$set: {state: stateFromAndroid}},
                        );
                        break;
                    case 5:
                        mqttClient.publish('toEsp/control/device/5', stateFromAndroid)
                        logDeviceActivities.insertOne({
                            "deviceId": "F1.5",
                            "state": stateFromAndroid,
                            "Timestamp": getTime(),
                            "Day": myTodayDate().myDay,
                            "Date": myTodayDate().myDate,
                            "Month": myTodayDate().myMonth,
                            "Year": myTodayDate().year,
                        })
                        floor1.updateMany(
                            {"_id": "F1.5"},
                            {$set: {state: stateFromAndroid}},
                        );
                        break;
                }
                // console.log('message from RN ', idFromAndroid)
                // console.log('state fromn android', stateFromAndroid)
            })
        })
    })
})



app.get('/', function (req, res) {
    //console.log(chartTime);
    res.redirect('/login');
});

app.get('/login', function (req, res) {
    loginFlag = false;
    res.render('login');
});


//LOGIN NodeMCU
app.get('/logincheckNodeMCU', function(req,res){
    if(req.query.username === username  && req.query.password === password){
        console.log("OK");
        loginFlag = true;
        res.redirect('/home');
    }
    else {
        console.log("Fail");
        res.redirect('/');
    }
});


//LOGIN Security Camera
app.post('/logincheckCamera', function(req,res){
    if(req.body.masterPass === masterPass){
        console.log("OK Cam");
        loginCamFlag = true;
        res.redirect('/handleImage');
    }
    else {
        console.log("Fail");
        res.redirect('/');
    }
});


//setup functions for mqtt client
mqttClient.on('connect', () => {
    console.log('mqtt client connected')
    //topic to control device
    mqttClient.subscribe('fromEsp/control/device/1', {qos: 0});
    mqttClient.subscribe('fromEsp/control/device/2', {qos: 0});
    mqttClient.subscribe('fromEsp/control/device/3', {qos: 0});
    mqttClient.subscribe('fromEsp/control/device/4', {qos: 0});
    mqttClient.subscribe('fromEsp/control/device/5', {qos: 0});
    //timer
    mqttClient.subscribe('fromEsp/timer/device/1', {qos: 0});
    mqttClient.subscribe('fromEsp/timer/device/2', {qos: 0});
    mqttClient.subscribe('fromEsp/timer/device/3', {qos: 0});
    mqttClient.subscribe('fromEsp/timer/device/4', {qos: 0});
    mqttClient.subscribe('fromEsp/timer/device/5', {qos: 0});

    //reserving for sensors topic
    mqttClient.subscribe('fromEsp/sensor/temp', {qos: 0});
    mqttClient.subscribe('fromEsp/sensor/humid', {qos: 0});
    mqttClient.subscribe('fromEsp/sensor/pressure', {qos: 0});

    //topic to receive security alert response
    mqttClient.subscribe('fromEsp/detect/human', {qos: 0});

    //subcribe to keep updating with  Raspberry Pi controlling D3
    mqttClient.subscribe('toEsp/control/device/3', {qos: 0});
})

MongoClient.connect(mongourl, function(err, db) {
    var floor1 = db.collection('floor1')
    var logDeviceActivities = db.collection('logDeviceActivities')
    mqttClient.on('message', function (topic, message) {
        receivedMessage = message.toString()
        topicStr = topic.toString()
        if(topicStr === 'toEsp/control/device/3') {
            logDeviceActivities.insertOne({
                "deviceId": "F1.3",
                "state": receivedMessage,
                "Timestamp": getTime(),
                "Day": myTodayDate().myDay,
                "Date": myTodayDate().myDate,
                "Month": myTodayDate().myMonth,
                "Year": myTodayDate().year,
            })
            floor1.updateMany(
                {"_id": "F1.3"},
                {$set: {state: receivedMessage}},
            );
        }
        loadStateFromSystem()
        //console.log('Received Topic: ', topicStr)
        //console.log('Received mess from ESP: ', receivedMessage)

    })
})

mqttClient.on('close', () => {
    console.log('MQTT Client disconnected')
})


/*
Read state SENT from System through INTERNET by nodeMCU. Then log deviceState to MongoDB
 */
function loadStateFromSystem () {
    MongoClient.connect(mongourl, function (err, db) {
        var floor1 = db.collection('floor1');

        //Receive state from System by Internet
        if (topicStr === 'fromEsp/control/device/1') {
            deviceState.device1 = receivedMessage;
            floor1.updateMany(
                {"_id": "F1.1"},
                {$set: {"_id": "F1.1", name: "Front Light", state: deviceState.device1}},
                {upsert: true}
            );
        }
        if (topicStr === 'fromEsp/control/device/2') {
            deviceState.device2 = receivedMessage;
            floor1.updateMany(
                {"_id": "F1.2"},
                {$set: {"_id": "F1.2", name: "Stair Light", state: deviceState.device2}},
                {upsert: true}
            );
        }
        if (topicStr === 'fromEsp/control/device/3') {
            deviceState.device3 = receivedMessage;
            floor1.updateMany(
                {"_id": "F1.3"},
                {$set: {"_id": "F1.3", name: "Main Door", state: deviceState.device3}},
                {upsert: true}
            );
        }
        if (topicStr === 'fromEsp/control/device/4') {
            deviceState.device4 = receivedMessage;
            floor1.updateMany(
                {"_id": "F1.4"},
                {$set: {"_id": "F1.4", name: "Dining Room Light", state: deviceState.device4}},
                {upsert: true}
            );
        }
        if (topicStr === 'fromEsp/control/device/5') {
            deviceState.device4 = receivedMessage;
            floor1.updateMany(
                {"_id": "F1.5"},
                {$set: {"_id": "F1.5", name: "Bathroom Light", state: deviceState.device5}},
                {upsert: true}
            );
        }
    });
}


//then Get handleImage page
app.get('/handleImage', function(req, res) {
    if (loginCamFlag === true) {
        MongoClient.connect(mongourl, function (err, db) {
            //access moongoDB
            var imgData = db.collection('imgData')
            //receive POST request from Client
            app.post('/imageData', function (req, res) {
                var newPerson = req.body.className
                var imgStringData = req.body.imgStr
                //get path to Imgs
                var pathToImg = getFaceImagePath(newPerson, pathToImgArray.length + 1);
                pathToImgArray.push(pathToImg);
                //get Base Url to Imgs
                var basePathToImg = path.dirname(pathToImg)
                //save to mongoDB block
                imgData.updateMany(
                    {"Registered As": newPerson},
                    {$set: {"Base path": basePathToImg}},
                    {upsert: true}
                )
                imgData.updateMany(
                    {"Registered As": newPerson},
                    {$addToSet: {"Image path": pathToImg}}
                )
                //save file to local disk block
                for (let i =1; i < max +1; i++) {
                    var buffer = new Buffer(imgStringData, 'base64');
                    fs.writeFileSync(pathToImg, buffer, 'base64', function (err) {
                        console.log(err);
                    })
                } //for bracket
                //empty array after write 5 photos
                if (pathToImgArray.length === max) {
                    pathToImgArray = []
                }
            })
        })
        res.render('handleImage');
    }
})

function getFaceImagePath(newClassName, idx) {
    //ensure directory exists
    //must have ./ before public in order for copying to another dir to work
    if (!fs.existsSync(`./public/images/${newClassName}`)) {
        fs.mkdirSync(`./public/images/${newClassName}`);
    }
    return `./public/images/${newClassName}/${newClassName}${idx}.png`
}

//check existing directory in training directory
function loadImgDir () {
    MongoClient.connect(mongourl, function (err, db) {
        var imgData = db.collection('imgData')
        //read base directory contains classes
        fs.readdir(basePathToImgDir, function (err, existedPeople) {
            //files is the return array of name of dir
            existedPeople.forEach(function (theirName) {
                var basePathToImg = basePathToImgDir +"/"+ theirName
                imgData.updateMany(
                    {"Registered As": theirName},
                    {$set: {"Base path": basePathToImg }},
                    {upsert: true}
                )
                //read sub directory
                fs.readdir(basePathToImg, function (err, toSubDir) {
                    toSubDir.forEach(function (toTheirImgPath) {
                        var pathToImg = basePathToImg + "/" + toTheirImgPath
                        imgData.updateMany(
                            {"Registered As": theirName},
                            {$addToSet: {"Image path": pathToImg}}
                        )
                        //console.log(toTheirImgPath);
                    })
                })
                //console.log(theirName);
            })
        })
    })
}


//LOGIN Page
app.post('/logincheck', function(req,res){
    if(req.body.username === username  && req.body.password === password){
        console.log("OK");
        loginFlag = true;
        res.redirect('/home');
    }
    else {
        console.log("Fail");
        res.redirect('/');
    }
});


//then get HOME page
app.get('/home', function (req, res) {
    if(loginFlag === true){
        MongoClient.connect(mongourl, function(err, db){
            chartCount = 0;
            chartTime = [];
            chartPower = [];
            assert.equal(null,err);
            var projection = {
            	"date" :1,
            	"year" :1,
            	"month" :1,
            	"time" :1,
            	"P":1,
            	"_id":0};
            var d = new Date();
            var cursor = db.collection('test2').find({
            	time: {$gt: '1.20'},
            	date: {$eq: d.getDate()},
            	month: {$eq: d.getMonth()+1},
            	year: {$eq: d.getFullYear()}
            });
            cursor.project(projection);
            cursor.forEach(
                function(doc) {
                    chartTime[chartCount] = doc.time;
                    chartPower[chartCount] = doc.P;
                    chartCount++;
                    console.log(doc.year);
                },
                function(err) {
                    assert.equal(err, null);
                    return db.close();
                }
            );
        });

        //render page with route /home
        res.render('home',{
            chartTime: chartTime,
            chartPower: chartPower,
            insideTemperature: temperature,
            insideHumidity: humid,
            gasDetection: gasDetection,
            // humanDetection: humanDetection,
            securityStatus: securityStatus,

            letterInsideGasdetectionBox : (gasDetection === "YES") ? "red": "blue",
            letterInsideHumandetectionBox: (humanDetection === "YES") ? "red": "blue",
            letterInsideSecurityBox: (securityStatus === "ARMED") ? "blue": "red",
        });

    }
    else
        res.redirect('/');
});


/*CONTROL
    Link with control.handlerbars, see device1ButtonColor and so on
    This should be the procedure in which Application server take data out from mongoDB and control devices
    by sending command directly back to NodeMCU
*/
app.get('/control', function (req, res) {
    if (loginFlag === true) {
        //declare connect mongo to use with app.post devices for control page
        MongoClient.connect(mongourl, function (err, db) {
            //config devices for floor1
            var floor1 = db.collection('floor1');
            //config collection to log device on/off time
            var logDeviceActivities = db.collection('logDeviceActivities')

            //Check current device state from MongoDB to keep the website up to date
            //setInterval to run the inside function repeatedly with Interval = 10ms to keep website up-to-date as fast as possible
            setInterval(function () {
            var cursor1 = floor1.find(
                {_id: {$eq:"F1.1"}}
            );
            cursor1.forEach(
                function (doc) {
                    deviceState.device1 = doc.state;
                }
            );
            var cursor2 = floor1.find(
                {_id: {$eq:"F1.2"}}
            );
            cursor2.forEach(
                function (doc) {
                    deviceState.device2 = doc.state;
                }
            );
            var cursor3 = floor1.find(
                {_id: {$eq:"F1.3"}}
            );
            cursor3.forEach(
                function (doc) {
                    deviceState.device3 = doc.state;
                }
            );
            var cursor4 = floor1.find(
                {_id: {$eq:"F1.4"}}
            );
            cursor4.forEach(
                function (doc) {
                    deviceState.device4 = doc.state;
                }
            );
            var cursor5 = floor1.find(
                {_id: {$eq:"F1.5"}}
            );
            cursor5.forEach(
                function (doc) {
                    deviceState.device5 = doc.state;
                }
            );
            },10);       //10 ms

            // Post state of devices to control them
            app.post('/device1', function (req, res) {
                deviceState.device1 = (deviceState.device1 === "on") ? "off" : "on";
                //newly added
                mqttClient.publish('toEsp/control/device/1', deviceState.device1)
                //log data into Mongodb
                logDeviceActivities.insertOne({
                    "deviceId": "F1.1",
                    "state": deviceState.device1,
                    "Timestamp": getTime(),
                    "Day": myTodayDate().myDay,
                    "Date": myTodayDate().myDate,
                    "Month": myTodayDate().myMonth,
                    "Year": myTodayDate().year,
                })

                if (deviceState.device1 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.1"},
                        {$set: {state: "on"}}               //without $set mongoDB won't update state field
                    )
                }
                else {
                    floor1.updateMany(
                        {"_id": "F1.1"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/control');
            });
            app.post('/device2', function (req, res) {
                deviceState.device2 = (deviceState.device2 === "on") ? "off" : "on";
                mqttClient.publish('toEsp/control/device/2', deviceState.device2)

                logDeviceActivities.insertOne({
                    "deviceId": "F1.2",
                    "state": deviceState.device2,
                    "Timestamp": getTime(),
                    "Day": myTodayDate().myDay,
                    "Date": myTodayDate().myDate,
                    "Month": myTodayDate().myMonth,
                    "Year": myTodayDate().year,
                })

                if (deviceState.device2 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.2"},
                        {$set: {state: "on"}}
                    )
                }
                else {
                    floor1.updateMany(
                        {"_id": "F1.2"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/control');
            });

            //Main door is Device 3
            app.post('/device3', function (req, res) {
                deviceState.device3 = (deviceState.device3 === "on") ? "off" : "on";
                mqttClient.publish('toEsp/control/device/3', deviceState.device3)

                logDeviceActivities.insertOne({
                    "deviceId": "F1.3",
                    "state": deviceState.device3,
                    "Timestamp": getTime(),
                    "Day": myTodayDate().myDay,
                    "Date": myTodayDate().myDate,
                    "Month": myTodayDate().myMonth,
                    "Year": myTodayDate().year,
                })

                if (deviceState.device3 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.3", state: "off"},
                        {$set: {state: "on"}}
                    )
                }
                else {
                    floor1.updateMany(
                        {"_id": "F1.3", state: "on"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/control');
            });

            app.post('/device4', function (req, res) {
                deviceState.device4 = (deviceState.device4 === "on") ? "off" : "on";
                mqttClient.publish('toEsp/control/device/4', deviceState.device4)

                logDeviceActivities.insertOne({
                    "deviceId": "F1.4",
                    "state": deviceState.device4,
                    "Timestamp": getTime(),
                    "Day": myTodayDate().myDay,
                    "Date": myTodayDate().myDate,
                    "Month": myTodayDate().myMonth,
                    "Year": myTodayDate().year,
                })

                if (deviceState.device4 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.4"},
                        {$set: {state: "on"}}
                    )
                }
                else {
                    floor1.updateMany(
                        {"_id": "F1.4"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/control');
            });

            app.post('/device5', function (req, res) {
                deviceState.device5 = (deviceState.device5 === "on") ? "off" : "on";
                mqttClient.publish('toEsp/control/device/5', deviceState.device5)

                logDeviceActivities.insertOne({
                    "deviceId": "F1.5",
                    "state": deviceState.device5,
                    "Timestamp": getTime(),
                    "Day": myTodayDate().myDay,
                    "Date": myTodayDate().myDate,
                    "Month": myTodayDate().myMonth,
                    "Year": myTodayDate().year,
                })

                if (deviceState.device5 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.5"},
                        {$set: {state: "on"}}
                    )
                }
                else {
                    floor1.updateMany(
                        {"_id": "F1.5"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/control');
            });
        });

            res.render('control', {
                device1state: (deviceState.device1 === "on") ? 'ON' : 'OFF',
                device2state: (deviceState.device2 === "on") ? 'ON' : 'OFF',
                device3state: (deviceState.device3 === "on") ? 'OPEN' : 'CLOSED',
                device4state: (deviceState.device4 === "on") ? 'ON' : 'OFF',
                device5state: (deviceState.device5 === "on") ? 'ON' : 'OFF',

                device1ButtonColor: (deviceState.device1 === "on") ? "blue" : "red",
                device2ButtonColor: (deviceState.device2 === "on") ? "blue" : "red",
                device3ButtonColor: (deviceState.device3 === "on") ? "blue" : "red",
                device4ButtonColor: (deviceState.device4 === "on") ? "blue" : "red",
                device5ButtonColor: (deviceState.device5 === "on") ? "blue" : "red",
            });

    }
    else
        res.redirect('/');
});

//Trang hẹn giờ
app.get('/submitTheTimeDevice1', function(req,res){
   deviceState.device1TimeOn = req.query.setTimeOn;
   deviceState.device1TimeOff = req.query.setTimeOff;
   mqttClient.publish('toEsp/timer/device/1/on', req.query.setTimeOn)
   mqttClient.publish('toEsp/timer/device/1/off', req.query.setTimeOff)
   checkChangedFlag.changedFlagStatus = "true";
   res.redirect('/control');
});
app.get('/submitTheTimeDevice2', function(req,res){
    deviceState.device2TimeOn = req.query.setTimeOn;
    deviceState.device2TimeOff = req.query.setTimeOff;
    mqttClient.publish('toEsp/timer/device/2/on', req.query.setTimeOn)
    mqttClient.publish('toEsp/timer/device/2/off', req.query.setTimeOff)
    checkChangedFlag.changedFlagStatus = "true";
    res.redirect('/control');
});
app.get('/submitTheTimeDevice3', function(req,res){
    deviceState.device3TimeOn = req.query.setTimeOn;
    deviceState.device3TimeOff = req.query.setTimeOff;
    mqttClient.publish('toEsp/timer/device/3/on', req.query.setTimeOn)
    mqttClient.publish('toEsp/timer/device/3/off', req.query.setTimeOff)
    checkChangedFlag.changedFlagStatus = "true";
    res.redirect('/control');
});
app.get('/submitTheTimeDevice4', function(req,res){
    deviceState.device4TimeOn = req.query.setTimeOn;
    deviceState.device4TimeOff = req.query.setTimeOff;
    mqttClient.publish('toEsp/timer/device/4/on', req.query.setTimeOn)
    mqttClient.publish('toEsp/timer/device/4/off', req.query.setTimeOff)
    checkChangedFlag.changedFlagStatus = "true";
    res.redirect('/control');
});
app.get('/submitTheTimeDevice5', function(req,res){
    deviceState.device5TimeOn = req.query.setTimeOn;
    deviceState.device5TimeOff = req.query.setTimeOff;
    mqttClient.publish('toEsp/timer/device/5/on', req.query.setTimeOn)
    mqttClient.publish('toEsp/timer/device/5/off', req.query.setTimeOff)
    checkChangedFlag.changedFlagStatus = "true";
    res.redirect('/control');
});


//SCENES
app.get('/scenesjson', function (req, res) {
    res.end(JSON.stringify(scenes));
});

app.get('/scenes', function (req, res) {
    if(loginFlag === true) {
        MongoClient.connect(mongourl, function (err, db) {
            //config devices for floor1
            var floor1 = db.collection('floor1');
            //config collection to log device on/off time
            var logDeviceActivities = db.collection('logDeviceActivities')

            //Gui scenes
            app.post('/goodmorning', function (req, res) {
                scenes.goodmorning = (scenes.goodmorning === "on") ? "off" : "on";
                scenes.iAmHome = "off";
                scenes.goodnight = "off";
                scenes.security = "off";
                //deviceState for updating button control in Control page
                deviceState.device1 = "on";
                deviceState.device2 = "off";
                deviceState.device4 = "off";
                deviceState.device5 = "on";
                //updateMany for updating database
                floor1.updateMany(
                    {"_id": "F1.1"},
                    {$set: {state: deviceState.device1}}
                );
                floor1.updateMany(
                    {"_id": "F1.2"},
                    {$set: {state: deviceState.device2}}
                );
                floor1.updateMany(
                    {"_id": "F1.4"},
                    {$set: {state: deviceState.device4}}
                );
                floor1.updateMany(
                    {"_id": "F1.5"},
                    {$set: {state: deviceState.device5}}
                );
                mqttClient.publish('toEsp/control/device/1', deviceState.device1)
                mqttClient.publish('toEsp/control/device/2', deviceState.device2)
                mqttClient.publish('toEsp/control/device/4', deviceState.device4)
                mqttClient.publish('toEsp/control/device/5', deviceState.device5)
                logDeviceActivities.insertMany([
                    {
                        "deviceId": "F1.1",
                        "state": deviceState.device1,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.2",
                        "state": deviceState.device2,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.4",
                        "state": deviceState.device4,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.5",
                        "state": deviceState.device5,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                ])
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/scenes');
            });
            app.post('/iamhome', function (req, res) {
                scenes.iAmHome = (scenes.iAmHome === "on") ? "off" : "on";
                scenes.goodmorning = "off";
                scenes.goodnight = "off";
                scenes.security = "off";
                deviceState.device1 = "on";
                deviceState.device2 = "on";
                deviceState.device4 = "off";
                deviceState.device5 = "off";
                //updateMany for updating database
                floor1.updateMany(
                    {"_id": "F1.1"},
                    {$set: {state: deviceState.device1}}
                );
                floor1.updateMany(
                    {"_id": "F1.2"},
                    {$set: {state: deviceState.device2}}
                );
                floor1.updateMany(
                    {"_id": "F1.4"},
                    {$set: {state: deviceState.device4}}
                );
                floor1.updateMany(
                    {"_id": "F1.5"},
                    {$set: {state: deviceState.device5}}
                );
                mqttClient.publish('toEsp/control/device/1', deviceState.device1)
                mqttClient.publish('toEsp/control/device/2', deviceState.device2)
                mqttClient.publish('toEsp/control/device/4', deviceState.device4)
                mqttClient.publish('toEsp/control/device/5', deviceState.device5)
                logDeviceActivities.insertMany([
                    {
                        "deviceId": "F1.1",
                        "state": deviceState.device1,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.2",
                        "state": deviceState.device2,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.4",
                        "state": deviceState.device4,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.5",
                        "state": deviceState.device5,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                ])
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/scenes');
            });
            app.post('/goodnight', function (req, res) {
                scenes.goodnight = (scenes.goodnight === "on") ? "off" : "on";
                scenes.goodmorning = "off";
                scenes.iAmHome = "off";
                scenes.security = "off";
                deviceState.device1 = "off";
                deviceState.device2 = "off";
                deviceState.device4 = "off";
                deviceState.device5 = "off";
                //updateMany for updating database
                floor1.updateMany(
                    {"_id": "F1.1"},
                    {$set: {state: deviceState.device1}}
                );
                floor1.updateMany(
                    {"_id": "F1.2"},
                    {$set: {state: deviceState.device2}}
                );
                floor1.updateMany(
                    {"_id": "F1.4"},
                    {$set: {state: deviceState.device4}}
                );
                floor1.updateMany(
                    {"_id": "F1.5"},
                    {$set: {state: deviceState.device5}}
                );
                mqttClient.publish('toEsp/control/device/1', deviceState.device1)
                mqttClient.publish('toEsp/control/device/2', deviceState.device2)
                mqttClient.publish('toEsp/control/device/4', deviceState.device4)
                mqttClient.publish('toEsp/control/device/5', deviceState.device5)
                logDeviceActivities.insertMany([
                    {
                        "deviceId": "F1.1",
                        "state": deviceState.device1,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.2",
                        "state": deviceState.device2,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.4",
                        "state": deviceState.device4,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.5",
                        "state": deviceState.device5,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                ])
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/scenes');
            });
            app.post('/security', function (req, res) {
                scenes.security = (scenes.security === "on") ? "off" : "on";
                scenes.goodmorning = "off";
                scenes.iAmHome = "off";
                scenes.goodnight = "off";
                deviceState.device1 = "off";
                deviceState.device2 = "on";
                deviceState.device4 = "off";
                deviceState.device5 = "off";
                //updateMany for updating database
                floor1.updateMany(
                    {"_id": "F1.1"},
                    {$set: {state: deviceState.device1}}
                );
                floor1.updateMany(
                    {"_id": "F1.2"},
                    {$set: {state: deviceState.device2}}
                );
                floor1.updateMany(
                    {"_id": "F1.4"},
                    {$set: {state: deviceState.device4}}
                );
                floor1.updateMany(
                    {"_id": "F1.5"},
                    {$set: {state: deviceState.device5}}
                );
                mqttClient.publish('toEsp/control/device/1', deviceState.device1)
                mqttClient.publish('toEsp/control/device/2', deviceState.device2)
                mqttClient.publish('toEsp/control/device/4', deviceState.device4)
                mqttClient.publish('toEsp/control/device/5', deviceState.device5)
                logDeviceActivities.insertMany([
                    {
                        "deviceId": "F1.1",
                        "state": deviceState.device1,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.2",
                        "state": deviceState.device2,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.4",
                        "state": deviceState.device4,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                    {
                        "deviceId": "F1.5",
                        "state": deviceState.device5,
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    },
                ])
                checkChangedFlag.changedFlagStatus = "true";
                res.redirect('/scenes');
            });
            res.render('scenes', {
                goodmorningColor: (scenes.goodmorning === "on") ? "blue" : "red",
                iAmHomeColor: (scenes.iAmHome === "on") ? "blue" : "red",
                goodnightColor: (scenes.goodnight === "on") ? "blue" : "red",
                securityColor: (scenes.security === "on") ? "blue" : "red",
            });
        });
    }
    else
        res.redirect('/');
});


//CHART
app.get('/chart', function (req, res) {
    if(loginFlag === true){
        MongoClient.connect(mongourl, function(err, db){
            assert.equal(null,err);
            chartCount = 0;
            chartTime = [];
            chartPower = [];
            var projection = {
                "date" :1,
                "year" :1,
                "month" :1,
                "time" :1,
                "P":1,
                "_id":0
            };
            var d = new Date();
            var cursor = db.collection('test2').find({
                date: {$eq: dateFilter},
                month: {$eq: monthFilter},
                year: {$eq: yearFilter}
            });
            cursor.project(projection);
            cursor.forEach(
                function(doc) {
                    chartTime[chartCount] = doc.time;
                    chartPower[chartCount] = doc.P;
                    chartCount++;
                    console.log(doc.year);
                },
                function(err) {
                    assert.equal(err, null);
                    return db.close();
                }
            );
        });
        res.render('chart',{
            date: dateFilter,
            month: monthFilter,
            year: yearFilter,
            chartTime: chartTime,
            chartPower: chartPower
        });
    }
    else
        res.redirect('/');
});

//Filter wanted values in CHART page
app.get('/filterPower', function (req, res) {
    var a = req.query.chartChooseMonth + " " + req.query.chartChooseDate + " " + req.query.chartChooseYear;
    var b = new Date(a);
    dateFilter = b.getDate();
    monthFilter = b.getMonth()+1;
    yearFilter = b.getFullYear();
    res.redirect('/chart');
});


//Outdoor CAMERA
app.get('/camera', function (req, res) {
    if(loginFlag === true){
        MongoClient.connect(mongourl, function (err, db) {
            //config devices for floor1
            var floor1 = db.collection('floor1');
            //config imgData DB to load imgData from DB
            var imgData = db.collection('imgData')
            //config collection to log time faces are detected
            var logFaceDetection = db.collection('logFaceDetection')
            //config collection to log device on/off time
            var logDeviceActivities = db.collection('logDeviceActivities')
            //read Data from DB at the beginning
            var receivedDataFromDB = imgData.distinct("Registered As")

            //config to receive message from outsider requesting for onetime password
            socketClient.once('connection', function(socket){
                socket.on('getRandomPassword', function (randomPassword) {
                    console.log('random ', randomPassword)
                    socketClient.emit('randomPassword', randomPassword)
                })
            })

            //Check current device state from MongoDB to keep the website up to date
            setInterval(function () {
                var cursor3 = floor1.find(
                    {_id: {$eq:"F1.3"}}
                );
                cursor3.forEach(
                    function (doc) {
                        deviceState.device3 = doc.state;
                    }
                );
            },0);

            // Post state of devices to control them
            app.post('/device3', function () {
                deviceState.device3 = (deviceState.device3 === "on") ? "off" : "on";
                mqttClient.publish('toEsp/control/device/3', deviceState.device3)
                console.log(deviceState.device3);
                if (deviceState.device3 === "on") {
                    floor1.updateMany(
                        {"_id": "F1.3"},
                        {$set: {state: "on"}}               //without $set mongoDB won't update state field
                    )
                    //checkChangedFlag.changedFlagStatus = "true";
                }
                else
                    if (deviceState.device3 === "off"){
                    floor1.updateMany(
                        {"_id": "F1.3"},
                        {$set: {state: "off"}},
                    )
                }
                checkChangedFlag.changedFlagStatus = "true";
                // res.redirect('/camera')
            });


            //handle post data of log face detected time
            app.post('/logDetectedFace', function (req, res) {
                var detectTime = req.body.detectTime
                var detectDate = req.body.detectDate
                var detectDay = req.body.detectDay
                var detectMonth = req.body.detectMonth
                var detectYear = req.body.detectYear
                var detectPerson = req.body.detectPerson
                var permission = req.body.permission

                logFaceDetection.insertOne({
                        "Person": detectPerson,
                        "Timestamp": detectTime,
                        "Day": detectDay,
                        "Date": detectDate,
                        "Month": detectMonth,
                        "Year": detectYear,
                        "Permission": permission,
                })
            })


            //handle post data of log face detected time
            app.post('/logDeviceActivities', function (req, res) {
                //log ON details
                var date = req.body.Date
                var day = req.body.Day
                var month = req.body.Month
                var year = req.body.Year
                var time = req.body.Time
                var deviceId = req.body.deviceId
                var logState = req.body.logState

                logDeviceActivities.insertOne({
                    "deviceId": deviceId,
                    "state": logState,
                    "Timestamp": time,
                    "Day": day,
                    "Date": date,
                    "Month": month,
                    "Year": year,
                })
            })

            receivedDataFromDB.then(function (existedPeople) {
                //console.log("kq la: ", existedPeople);
                res.render('camera', {
                    device3state: (deviceState.device3 === "on") ? 'OPEN' : 'CLOSED',
                    device3ButtonColor: (deviceState.device3 === "on") ? "blue" : "red",
                    existedPeople: JSON.stringify(existedPeople)
                })
            })
        });
    }
    else
    res.redirect('/');
});


//Welcome page for visitors
app.get('/outsider', function (req, res) {
    MongoClient.connect(mongourl, function (err, db) {
        //config devices for floor1
        var floor1 = db.collection('floor1');
        //config collection to log device on/off time
        var logDeviceActivities = db.collection('logDeviceActivities')

        //setup socket io for welcome page
        socketClient.once('connection', function(socket){
                socket.on('getMessageFromOutsider', function (data) {
                    socketClient.emit('messageFromOutsider', data)
                    mqttClient.publish('toEsp/detect/visitor', "on")
                })
        })


        setInterval(function () {
            var cursor3 = floor1.find(
                {_id: {$eq:"F1.3"}}
            );
            cursor3.forEach(
                function (doc) {
                    deviceState.device3 = doc.state;
                }
            );
        },0);       //0 ms

        app.post('/device3', function () {
            deviceState.device3 = (deviceState.device3 === "on") ? "off" : "on";
            mqttClient.publish('toEsp/control/device/3', deviceState.device3)
            console.log(deviceState.device3);
            if (deviceState.device3 === "on") {
                floor1.updateMany(
                    {"_id": "F1.3"},
                    {$set: {state: "on"}}
                )
                //checkChangedFlag.changedFlagStatus = "true";
            }
            else
            if (deviceState.device3 === "off"){
                floor1.updateMany(
                    {"_id": "F1.3"},
                    {$set: {state: "off"}},
                )
            }
            checkChangedFlag.changedFlagStatus = "true";
            // res.redirect('/camera')
        });

        //handle post data of log face detected time
        app.post('/logDeviceActivities', function (req, res) {
            //log ON details
            var date = req.body.Date
            var day = req.body.Day
            var month = req.body.Month
            var year = req.body.Year
            var time = req.body.Time
            var deviceId = req.body.deviceId
            var logState = req.body.logState

            logDeviceActivities.insertOne({
                "deviceId": deviceId,
                "state": logState,
                "Timestamp": time,
                "Day": day,
                "Date": date,
                "Month": month,
                "Year": year,
            })
        })
            res.render('outsider', {
                device3state: (deviceState.device3 === "on") ? 'OPEN' : 'CLOSED',
                device3ButtonColor: (deviceState.device3 === "on") ? "blue" : "red",
            })

    });
});


//Indoor CAMERA
app.get('/motion', function (req, res) {
    // securityStatus = "ARMED";
    if(loginFlag === true){
        MongoClient.connect(mongourl, function (err, db) {
            var logMotion = db.collection('logMotion')
            socketClient.once('connection', function(socket){
                socket.on('getMessageFromMotion', function (data) {
                    //send ALERT to System each 2s IN 5 MINUTES (300s)
                    //5 mins too long for a demo
                    var startTime = new Date().getTime()
                    var interval = setInterval(function() {
                        if (new Date().getTime() - startTime > 20000) {
                                clearInterval(interval)
                                return
                        }
                        //TODO: implement SIM module to receive alert
                        mqttClient.publish('toEsp/detect/human', "on")
                    }, 5000)


                    //send message to other clients to raise alert
                    socketClient.emit('messageFromMotion', data)

                    logMotion.insertOne({
                        "Timestamp": getTime(),
                        "Day": myTodayDate().myDay,
                        "Date": myTodayDate().myDate,
                        "Month": myTodayDate().myMonth,
                        "Year": myTodayDate().year,
                    })
                })
            })

        })

        res.render('motion',{
            //
        });
    }
    else
        res.redirect('/');
});


//CHAT page using NodeJs, MongoDB and Socket.io
app.get('/chat', function(req, res){
    if (loginFlag === true){
        //res.render('chat');
        MongoClient.connect(mongourl, function (err, db) {
            if(err){
                throw err;
            }
            //res.render('chat');
            //var chats = db.collection('chats');
            //connect to socket.io. USE client.ONCE to avoid duplicate message on client site
            socketClient.once('connection', function(socket){
                //socket.removeAllListeners();
                var chats = db.collection('chats');

                //create func to send status
                sendStatus = function(s){
                    socket.emit('status',s);
                };
                //access chats in mongoDB
                chats.find().limit(100).sort({_id:1}).toArray(function (err, res){
                    if(err){
                        throw err;
                    }
                    // Emit messages
                    socket.emit('output',res);
                });

                //handle input events
                socket.on('input', function (data) {
                  var name = data.name;
                  var message = data.message;

                  //check for name and message if they are blanked
                    if(name == '' || message == ''){
                        //send error status
                        sendStatus('Please enter at least one name or message');
                    }
                    else{
                        //insert message to mongodb or so called : send message
                        chats.insert({name: name, message: message}, function () {
                            socketClient.emit('output', [data]);

                            //send status back
                            sendStatus({
                                message: 'Message sent',
                                clear: true
                            });
                        });
                    } //else bracket
                });

                // Handle clear
                socket.on('clear', function (data) {
                   //Remove all chats from collection of mongoDB
                   chats.remove({}, function () {
                       //emit cleared
                       socket.emit('cleared');
                   });
                });
            }); //bracket of client.on
        });     //bracket of mongo
        res.render('chat');
    }
    else
        res.redirect('/');

});

/* Read value from system and return JSON page to client
*/
app.get('/temp', function (req, res) {
    res.end(JSON.stringify(temperature));		//return JSON contains value of temperature for customer (return directly on website)
});
app.get('/humid', function (req, res) {
    res.end(JSON.stringify(humid));				//return JSON contains value of humid for customer (return directly on website)
});
app.get('/gas', function (req, res) {
    res.end(JSON.stringify(gasDetection));
});


/*  route get from NodeMCU arduino code function sendDataFromSensorToInternet
    read value and save to mongoDB
    */

app.get('/readTempFromSystem', function (req, res) {
    temperature = req.query.temperature;
});
app.get('/readHumidFromSystem', function (req, res) {
    humid = req.query.humid;
});
app.get('/readGasFromSystem', function (req, res) {
    gasDetection = req.query.gasDetection;
});

/*
app.get('/readHumanFromSystem', function (req, res) {
    humanDetection = req.query.humanDetection;
});
*/
// Read Power value from System by NodeMCU through Internet, then Log data into MongoDB
app.get('/readPowerFromSystem', function (req, res) {
    var d = new Date();
    Power = req.query.Power;
    MongoClient.connect(mongourl, function(err, db){
        assert.equal(null,err);
        db.collection('test2').insertOne({
        	"deviceID": "",
        	"date": d.getDate(),
        	"month": d.getMonth()+1,
        	"year": d.getFullYear(),
        	"time": d.getHours() + "." + d.getMinutes(),
        	"P": req.query.Power
        })
    });
});

app.get('/Power', function (req, res) {
    res.end(JSON.stringify(Power));
});

//JSON page receive state from deviceState
app.get('/state', function (req, res) {
    res.end(JSON.stringify(deviceState));
});


//reset update flag in NodeMCU
app.get('/checkChangedFlag', function(req,res){
    if(req.query.device === "NodeMCU"){
        checkChangedFlag.changedFlagStatus = "false";
    }
    res.end(JSON.stringify(checkChangedFlag));
});


function getTime() {
    var date = new Date();

    var hour = date.getHours();
    hour = (hour < 10 ? "0" : "") + hour;

    var min  = date.getMinutes();
    min = (min < 10 ? "0" : "") + min;

    var sec  = date.getSeconds();
    sec = (sec < 10 ? "0" : "") + sec;

    return hour + ":" + min + ":" + sec;
}


function myTodayDate(){
    var today = new Date();
    var day = ["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
    var month = ["January","February","March","April","May","June","July","August","September","October","November","December"];
    var myTodayObj =
        {
            myDate : today.getDate(),
            myDay : day[today.getDay()],
            myMonth : month[today.getMonth()],
            year : today.getFullYear()
        }
    return myTodayObj;
}


//get local IP address to enter
function getLocalIp() {
    for(let addresses of Object.values(os.networkInterfaces())) {
        for(let add of addresses) {
            if(add.address.startsWith('192.168.')) {
                console.log('web server url to enter: ', add.address)
                // return add.address;
            }
        }
    }
}

var t1 = performance.now();
console.log('time taken to run '+ (t1-t0)+ ' ms');

/*
app.get('/process_get', function (req, res) {
   response = {
      first_name:req.query.first_name,
   };
   a = req.query.first_name;
   console.log(response);
   res.end(JSON.stringify(response));
});
*/







//var mongourl = 'mongodb://localhost:27017//video';
/*
// Fetch data
function FetchData(){
    fetch('http://192.168.122.26/mrbs_sourcecode/API/Demo/APIController.php')
        .then(function(res) {
            return res.json();
        }).then(function(json) {
            a = json.API[0].id;
            console.log(a);
        })

}
*/

//connect mongoDb using mLab
/*
MongoClient.connect(mongourl, function(err, db){
    assert.equal(null,err);
    console.log("Successfully connected to MongoDB");

//config devices for floor1
    var floor1 = db.collection('floor1');
    //below var device is useless but still keep it for clearer view of devices in DB
	var device = [
        {"_id": "F1.1", name: "Front Light", state: "off"},
        {"_id": "F1.2", name: "Stair Light", state: "off"},
        {"_id": "F1.3", name: "Air Cooler", state: "off"},
        {"_id": "F1.4", name: "Dining Room Light", state: "off"}
    ];
//upsert: true means write new document if not yet exist. otherwise update fields
   	floor1.updateMany(
    	{"_id": "F1.1"},
    	{$set: {"_id": "F1.1", name: "Front Light", state: "off"}},
    	{upsert: true}
    );
	floor1.updateMany(
    	{"_id": "F1.2"},
    	{$set: {"_id": "F1.2", name: "Stair Light", state: "off"}},
    	{upsert: true}
    );
	floor1.updateMany(
    	{"_id": "F1.3"},
    	{$set: {"_id": "F1.3", name: "Air Cooler", state: "off"}},
    	{upsert: true}
    );
    	floor1.updateMany(
    	{"_id": "F1.4"},
    	{$set: {"_id": "F1.4", name: "Dining Room Light", state: "off"}},
    	{upsert: true}
    );
/*
	projection allows to include or exclude fields in mongoDB query
	1 indicate including a field and 0 is excluding.
	=> include field "date", "year", "month", "time", "P"
	=> exclude field "_id"
	see https://stackoverflow.com/questions/19684757/mongodb-query-criterias-and-projections
	and http://mongodb.github.io/node-mongodb-native/2.2/tutorials/projections/
*/
/*    var projection = {
    	"date" :1,
    	"year" :1,
    	"month" :1,
    	"time" :1,
    	"P":1,
    	"_id":0
    	//if needed, add field "deviceID" by the following line
    	//"deviceID": 1,
    	};

    //db.collection('test2').insertOne({"deviceID": "D04", "date": d.getDate(), "month": d.getMonth(), "year": d.getFullYear(), "time": d.getHours() + "." + d.getMinutes(), "P": req.query.Power})
    var d = new Date();
    var cursor = db.collection('test2').find({
    	time: {$gt: '1.20'},			//select TIME where value of "time" is greater than "1.2"
    	date: {$eq: d.getDate()},		//select DATE where value of "date" equal to returned value from getDate method.
    	month: {$eq: d.getMonth()+1},
    	year: {$eq: d.getFullYear()}
    })

    //filtered data with needed field using projection
    cursor.project(projection)
    //for each document in the cursor, apply the function(doc)
    //use this to get the value to draw the chart
    cursor.forEach(
        function(doc) {
            chartTime[chartCount] = doc.time;	//read docs from field "time"
            chartPower[chartCount] = doc.P;		//read docs	from field "P"
            chartCount++;						//init chartCount=0
            console.log(doc.year);
        },
        function(err) {
            assert.equal(err, null);
            return db.close();
        }
    );  *
});

/*
app.get('/', function (req, res) {
    res.render('index', { title: 'Hey', message: 'Hello there!',info: a})
});
*/
