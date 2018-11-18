var mongoUrl = 'mongodb://localhost:27017';
var databaseName = 'myHome';
var mongoClient = require('mongodb').MongoClient;

mongoClient.connect(mongoUrl+ '/' +databaseName, function(err, db){
    if (err) throw err;
/*  Create collection, then no needed anymore
    db.createCollection('floor1', function (err, res) {
        if (err) throw err;
        console.log('Created');
        db.close();

    });
    */

var floor1 = db.collection('floor1');
var powerTracker = db.collection('powerTracker');

var device = [
        {"_id": "F1.1", name: "Front Light", state: "ON"},
        {"_id": "F1.2", name: "Stair Light", state: "ON"},
        {"_id": "F1.3", name: "Air Cooler", state: "ON"}
    ];
var d = new Date();
var data = {
        time: d.getHours() + ":" + d.getMinutes(),
        date: d.getDate(),
        month: d.getMonth()+1,
        year: d.getFullYear()
    };

/*floor1.createIndex(
    {"_id": "F1.1"},
    {unique: true}
    );
*/
/*floor1.insert(device, function(err, res){
    if (err) throw err;
    console.log('Added Devices for 1st Floor');
    });
*/
floor1.updateMany(
    {"_id": "F1.1"},
    {$set: {"_id": "F1.1", name: "Front Light", state: "ON"}},
    {upsert: true}
    );
floor1.updateMany(
    {"_id": "F1.2"},
    {$set: {"_id": "F1.2", name: "Stair Light", state: "ON"}},
    {upsert: true}
    );
floor1.updateMany(
    {"_id": "F1.3"},
    {$set: {"_id": "F1.3", name: "Air Cooler", state: "ON"}},
    {upsert: true}
    );
powerTracker.insert(data, function(err, res){
    if (err) throw err;
    console.log('Added init data for power tracking');
    });

var condMid = 1;
//1st method to replace, not working well because still duplicates docs
/*if (condMid <3) {
    var bulk = floor1.initializeUnorderedBulkOp();
    bulk.find({deviceF: "F1", "_id": "01", state: "ON"}).upsert().replaceOne(
        {deviceF: "F1", "_id": "01",name: "Front Light",state: "OFF"}
    );
    bulk.execute();
    };*/

//2nd method, easier to understand
if (condMid<3) {
    floor1.updateMany(
        {"_id": "F1.1", state: "ON"},
        {$set: {"_id": "F1.1", name: "Front Light", state: "OFF"}},

    );
};
    db.close();
});





